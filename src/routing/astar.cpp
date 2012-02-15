#include "astar.hpp"
#include "tests.hpp"
#include <limits>
#include "closedlist.hpp"
#include "heap.hpp"
#include "spatialitedatabase.hpp"
#include "sqlitedatabase.hpp"

/**
 * @todo Implementieren
 * @todo Testen
 * @bug Wenn der User eine Position aussucht, bei der ein Knoten, der nicht
 *      am Graphen beteiligt ist, am nächsten ist, wird dieser als Startpunkt
 *      ausgewählt. Fälschlicherweise wird dann keine Route berechnet!
 */
GPSRoute AStarRouter::calculateShortestRoute(const GPSPosition& startPosition, const GPSPosition& endPosition)
{
    if (!_db->isDBOpen())
    {
        return GPSRoute();
    }
    else
    {
        RoutingNode startNode, endNode;
        QVector<boost::shared_ptr<RoutingNode> > nodeList;
        
        /* TODO: Fehlerhafte Annahme hier ist, dass alle Knoten auch Ways
         *    haben, die bei ihnen losgehen. Soll erstmal reichen.
         */
        //Suche zuerst den Startknoten raus, dann den Endknoten. Umkreissuche.
        nodeList = _db->getNodes(startPosition, 50.0);
        if (nodeList.isEmpty())
        {
            nodeList = _db->getNodes(startPosition, 500.0);
            if (nodeList.isEmpty())
            {
                nodeList = _db->getNodes(startPosition, 5000.0);
                if (nodeList.isEmpty())
                {
                    //Okay, im Umkreis von 5000m nix gefunden: Dann keine Route gefunden.
                    return GPSRoute();
                }
            }
        }
        //nodeList nach nächstem Knoten durchsuchen.
        float minDistance = std::numeric_limits<float>::max();
        for (QVector<boost::shared_ptr<RoutingNode> >::const_iterator it = nodeList.constBegin();
            it != nodeList.constEnd(); it++)
        {
            float distance = (*it)->calcDistance(startPosition);
            if (distance < minDistance)
            {
                startNode = **it;   //Doppelt dereferenzieren, weil in der Liste boost::shared_ptr stehen
                minDistance = distance;
            }
        }
        //startNode ist der Knoten mit der kürzesten Entfernung zu startPosition.
        
        nodeList = _db->getNodes(endPosition, 50.0);
        if (nodeList.isEmpty())
        {
            nodeList = _db->getNodes(endPosition, 500.0);
            if (nodeList.isEmpty())
            {
                nodeList = _db->getNodes(endPosition, 5000.0);
                if (nodeList.isEmpty())
                {
                    //Okay, im Umkreis von 5000m nix gefunden: Dann keine Route gefunden.
                    return GPSRoute();
                }
            }
        }
        //nodeList nach nächstem Knoten durchsuchen.
        minDistance = std::numeric_limits<float>::max();
        for (QVector<boost::shared_ptr<RoutingNode> >::const_iterator it = nodeList.constBegin();
            it != nodeList.constEnd(); it++)
        {
            float distance = (*it)->calcDistance(endPosition);
            if (distance < minDistance)
            {
                endNode = **it;   //Doppelt dereferenzieren, weil in der Liste boost::shared_ptr stehen
                minDistance = distance;
            }
        }
        //endNode ist der Knoten mit der kürzesten Entfernung zu endPosition.
        
        //TODO: Start- und Endknoten müssen Graphen-Knoten sein, sonst haut das mit den IDs nicht hin.
        
        return calculateShortestRoute(startNode, endNode);
    }
}

/**
 * @todo: Implementieren
 */
GPSRoute AStarRouter::calculateShortestRoute(const RoutingNode& startNode, const RoutingNode& endNode)
{
    if (!_db->isDBOpen())
    {
        return GPSRoute();
    }
    else
    {
        //Initialisiere Datenstrukturen
        HashClosedList closedList;
        NodeCostLessAndQHashFunctorStar<boost::uint64_t, double> estimatedCosts;
        QHash<boost::uint64_t, boost::uint64_t> nodeCosts;
        //estimated für nodeCosts eingesetzt
        BinaryHeap<boost::uint64_t, NodeCostLessAndQHashFunctorStar<boost::uint64_t, double> > heap(estimatedCosts);
        QHash<boost::uint64_t, boost::uint64_t> predecessor;
        QHash<boost::uint64_t, boost::shared_ptr<RoutingNode> > nodeMap;

        
        
        
        //Startknoten: Kosten auf Null setzen, zum Heap und Puffer hinzufügen, Vorgänger auf Null setzen
        nodeCosts[startNode.getID()] = 0.0;
        heap.add(startNode.getID());
        nodeMap.insert(RoutingNode::convertIDToShortFormat(startNode.getID()), _db->getNodeByID(startNode.getID()));
        predecessor.insert(startNode.getID(), 0);
        
        boost::uint64_t activeNodeLongID = 0;
        boost::shared_ptr<RoutingNode> activeNode;
        
        //TODO: Knoten vorladen, damit die DB nicht so oft gefragt werden muss (einmal am Stück ist schneller)
        //TODO: nodeMap evtl ersetzen durch den DatabaseRAMCache?
        
        while (!heap.isEmpty())
        {
            //Aktuelles Element wird jetzt abschließend betrachtet.
            activeNodeLongID = heap.removeMinimumCostElement();
            activeNode = nodeMap[activeNodeLongID];
            closedList.addElement(activeNodeLongID);
            
            //Wenn der jetzt abschließend zu betrachtende Knoten der Endkonten ist: Fertig.
            if (startNode.getID() == endNode.getID())
            {
                break;
            }
            
            //Hole Liste von Kanten, die in abschließend betrachtetem
            //Knoten beginnen und bearbeite sie.
            QVector<boost::shared_ptr<RoutingEdge> > edgeList = 
                _db->getEdgesByStartNodeID(startNode.getID());
            for (QVector<boost::shared_ptr<RoutingEdge> >::iterator it =
                edgeList.begin(); it < edgeList.end(); it++)
            {
                //ID zwischenspeichern, damit sie nicht immer neu berechnet werden muss
                boost::uint64_t activeEdgeEndNodeLongID = (*it)->getEndNodeID();
                //Wenn der Endknoten noch nicht abschließend betrachtet wurde (müsste immer wahr sein...)
                if (!closedList.contains((*it)->getEndNodeID()))
                {
                    //Erstmal Knoten aus der DB holen und puffern, wenn er noch nicht geladen wurde
                    boost::uint64_t activeEdgeEndNodeShortID =
                        RoutingNode::convertIDToShortFormat((*it)->getEndNodeID());
                    boost::shared_ptr<RoutingNode> activeEdgeEndNode;
                    if (!nodeMap.contains(activeEdgeEndNodeShortID))
                    {
                        activeEdgeEndNode = _db->getNodeByID(activeEdgeEndNodeShortID);
                        nodeMap.insert(activeEdgeEndNodeShortID, activeEdgeEndNode);
                    }
                    else
                    {
                        activeEdgeEndNode = nodeMap[activeEdgeEndNodeShortID];
                    }
                    
                    
                    //Wurde der Knoten schon einmal betrachtet, oder nicht?
                    if (!heap.contains(activeEdgeEndNodeLongID))
                    {
                        
                        nodeCosts[activeEdgeEndNodeLongID] = nodeCosts[activeNodeLongID] +
                            _metric->rateEdge(**it, *activeNode, *activeEdgeEndNode);
                        //---------*activeNode oder *it benutzen??//
                        float distance = (*activeNode).calcDistance(endNode);
                        estimatedCosts.setValue(activeEdgeEndNodeLongID, nodeCosts[activeNodeLongID] + 
                            _metric->rateEdge(**it, *activeNode, *activeEdgeEndNode) + distance);

                        heap.add(activeEdgeEndNodeLongID);
                        //Vorgänger-Zeiger setzen
                        predecessor.insert(activeEdgeEndNodeLongID, activeNodeLongID);
                    }
                    else
                    {
                        double newCosts = nodeCosts[activeNodeLongID] + 
                            _metric->rateEdge(**it, *activeNode, *activeEdgeEndNode);
                        if (newCosts < nodeCosts[activeEdgeEndNodeLongID])
                        {
                            nodeCosts[activeEdgeEndNodeLongID] = newCosts;
                            //-----------------------*activeNode oder *it benutzen?? ----- 
                            float distance = (*activeNode).calcDistance(endNode);
                            estimatedCosts.setValue(activeEdgeEndNodeLongID, nodeCosts[activeNodeLongID] + 
                            _metric->rateEdge(**it, *activeNode, *activeEdgeEndNode) + distance); 
                            heap.decreaseKey(activeEdgeEndNodeLongID);
                            predecessor.insert(activeEdgeEndNodeLongID, activeNodeLongID);
                        }
                    }
                }
            }
        }
        
        std::cerr << "finished, search space contains " << nodeMap.size() << " elements." << std::endl;
        
        if (activeNodeLongID == endNode.getID())
        {
            boost::uint64_t activeNodeID = activeNodeLongID;
            GPSRoute route;
            while (activeNodeID != 0)
            {
                route.insertForward(*nodeMap[RoutingNode::convertIDToShortFormat(activeNodeID)]);
                activeNodeID = predecessor[activeNodeID];
            }
            return route;
        }
        else
        {
            std::cerr << "did not find a route." << std::endl;
            return GPSRoute();
        }
        
        
        /* TODO:
         * - Fertige Route heraussuchen und zurückgeben.
         * - Evtl. Routenbeschreibung erzeugen?
         */
    }
}

AStarRouter::AStarRouter(boost::shared_ptr<DatabaseConnection> db, boost::shared_ptr<RoutingMetric> metric) :
    _db(db), _metric(metric)
{
    
}

namespace biker_tests
{
    int testAStarRouter()
    {
        #ifdef SPATIALITE_FOUND
            boost::shared_ptr<SpatialiteDatabaseConnection> db(new SpatialiteDatabaseConnection());
        #else
            boost::shared_ptr<SQLiteDatabaseConnection> db(new SQLiteDatabaseConnection());
        #endif
        boost::shared_ptr<RoutingMetric> metric(new EuclidianRoutingMetric());
        db->open("rub.db");
        
        CHECK(db->isDBOpen());
        
        GPSRoute route;
        CHECK(route.isEmpty());
        
        AStarRouter router(db, metric);
        std::cerr << "routing...." << std::endl;
        route = router.calculateShortestRoute(GPSPosition(51.447, 7.2676), GPSPosition(51.4492, 7.2592));
        
        CHECK(!route.isEmpty());
        route.exportGPX("astar.gpx");
        route.exportJSON("astar.js");
        
        return EXIT_SUCCESS;
    }
}
