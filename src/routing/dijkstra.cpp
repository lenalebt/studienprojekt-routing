#include "dijkstra.hpp"
#include "tests.hpp"
#include <limits>
#include "heap.hpp"
#include "spatialitedatabase.hpp"
#include "sqlitedatabase.hpp"
#include <QtConcurrentRun>

GPSRoute DijkstraRouter::calculateShortestRoute(const GPSPosition& startPosition, const GPSPosition& endPosition)
{
    if (!_db->isDBOpen())
    {
        std::cerr << "database file is closed." << std::endl;
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
                    std::cerr << "did not find a matching starting point." << std::endl;
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
                    std::cerr << "did not find a matching end point" << std::endl;
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
        
        return calculateShortestRoute(startNode, endNode);
    }
}

GPSRoute DijkstraRouter::calculateShortestRoute(const RoutingNode& startNode, const RoutingNode& endNode)
{
    if (!_db->isDBOpen())
    {
        std::cerr << "database file is closed." << std::endl;
        return GPSRoute();
    }
    else
    {
        //Initialisiere Datenstrukturen
        //std::cerr << "init data structures" << std::endl;
        HashClosedList closedList;
        NodeCostLessAndQHashFunctor<boost::uint64_t, double> nodeCosts;
        BinaryHeap<boost::uint64_t, NodeCostLessAndQHashFunctor<boost::uint64_t, double> > heap(nodeCosts);
        QHash<boost::uint64_t, boost::uint64_t> predecessor;
        QHash<boost::uint64_t, boost::shared_ptr<RoutingNode> > nodeMap;
        
        
        //Startknoten: Kosten auf Null setzen, zum Heap und Puffer hinzufügen, Vorgänger auf Null setzen
        boost::uint64_t activeNodeLongID = RoutingNode::convertIDToLongFormat(startNode.getID());
        boost::uint64_t activeNodeShortID = RoutingNode::convertIDToShortFormat(startNode.getID());
        
        nodeCosts.setValue(activeNodeLongID, 0.0);
        heap.add(activeNodeLongID);
        nodeMap.insert(activeNodeShortID, _db->getNodeByID(startNode.getID()));
        predecessor.insert(activeNodeLongID, 0);
        
        boost::shared_ptr<RoutingNode> activeNode;
        
        boost::uint64_t startNodeShortID = RoutingNode::convertIDToShortFormat(startNode.getID());
        boost::uint64_t endNodeShortID = RoutingNode::convertIDToShortFormat(endNode.getID());
        
        //TODO: Knoten vorladen, damit die DB nicht so oft gefragt werden muss (einmal am Stück ist schneller)
        //TODO: nodeMap evtl ersetzen durch den DatabaseRAMCache?
        
        //std::cerr << "starting while loop" << std::endl;
        while (!heap.isEmpty())
        {
            //std::cerr << ".";
            //Aktuelles Element wird jetzt abschließend betrachtet.
            activeNodeLongID = heap.removeMinimumCostElement();
            activeNodeShortID = RoutingNode::convertIDToShortFormat(activeNodeLongID);
            activeNode = nodeMap[activeNodeShortID];
            closedList.addElement(activeNodeLongID);
            
            //std::cerr << activeNodeLongID << ":" << activeNodeShortID;
            
            //Wenn der jetzt abschließend zu betrachtende Knoten der Endkonten ist: Fertig.
            if (activeNodeShortID == endNodeShortID)
            {
                //std::cerr << "found endnode!";
                break;
            }
            
            //Hole Liste von Kanten, die in abschließend betrachtetem
            //Knoten beginnen und bearbeite sie.
            QVector<boost::shared_ptr<RoutingEdge> > edgeList = 
                _db->getEdgesByStartNodeID(activeNodeLongID);
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
                        //Neuen Knoten zum Heap dazu, nachdem neue Kosten gesetzt wurden.
                        nodeCosts.setValue(activeEdgeEndNodeLongID, nodeCosts.getValue(activeNodeLongID) + 
                            _metric->rateEdge(**it, *activeNode, *activeEdgeEndNode));
                        heap.add(activeEdgeEndNodeLongID);
                        //Vorgänger-Zeiger setzen
                        predecessor.insert(activeEdgeEndNodeLongID, activeNodeLongID);
                    }
                    else
                    {
                        double newCosts = nodeCosts.getValue(activeNodeLongID) + 
                            _metric->rateEdge(**it, *activeNode, *activeEdgeEndNode);
                        if (newCosts < nodeCosts.getValue(activeEdgeEndNodeLongID))
                        {
                            nodeCosts.setValue(activeEdgeEndNodeLongID, newCosts);
                            heap.decreaseKey(activeEdgeEndNodeLongID);
                            predecessor.insert(activeEdgeEndNodeLongID, activeNodeLongID);
                        }
                    }
                }
            }
        }
        
        std::cerr << "finished, search space contains " << nodeMap.size() << " nodes." << std::endl;
        
        if (activeNodeShortID == endNodeShortID)
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

DijkstraRouter::DijkstraRouter(boost::shared_ptr<DatabaseConnection> db, boost::shared_ptr<RoutingMetric> metric) :
    _db(db), _metric(metric)
{
    
}

MultithreadedDijkstraRouter::MultithreadedDijkstraRouter(boost::shared_ptr<DatabaseConnection> dbA, boost::shared_ptr<DatabaseConnection> dbB, boost::shared_ptr<RoutingMetric> metric) :
    _dbA(dbA), _dbB(dbB), _metric(metric)
{
    
}

GPSRoute MultithreadedDijkstraRouter::calculateShortestRouteThreadA(const RoutingNode& startNode, MultiThreadedHashClosedList* closedList)
{
    if (!_dbA->isDBOpen())
    {
        std::cerr << "database file A is closed." << std::endl;
        return GPSRoute();
    }
    else
    {
        //Initialisiere Datenstrukturen
        //std::cerr << "init data structures" << std::endl;
        NodeCostLessAndQHashFunctor<boost::uint64_t, double> nodeCosts;
        BinaryHeap<boost::uint64_t, NodeCostLessAndQHashFunctor<boost::uint64_t, double> > heap(nodeCosts);
        QHash<boost::uint64_t, boost::uint64_t> predecessor;
        QHash<boost::uint64_t, boost::shared_ptr<RoutingNode> > nodeMap;
        
        //Startknoten: Kosten auf Null setzen, zum Heap und Puffer hinzufügen, Vorgänger auf Null setzen
        boost::uint64_t activeNodeLongID = RoutingNode::convertIDToLongFormat(startNode.getID());
        boost::uint64_t activeNodeShortID = RoutingNode::convertIDToShortFormat(startNode.getID());
        
        nodeCosts.setValue(activeNodeLongID, 0.0);
        heap.add(activeNodeLongID);
        nodeMap.insert(activeNodeShortID, _dbA->getNodeByID(startNode.getID()));
        predecessor.insert(activeNodeLongID, 0);
        
        boost::shared_ptr<RoutingNode> activeNode;
        
        boost::uint64_t startNodeShortID = RoutingNode::convertIDToShortFormat(startNode.getID());
        
        //TODO: Knoten vorladen, damit die DB nicht so oft gefragt werden muss (einmal am Stück ist schneller)
        //TODO: nodeMap evtl ersetzen durch den DatabaseRAMCache?
        
        //std::cerr << "starting while loop" << std::endl;
        while (!heap.isEmpty())
        {
            //std::cerr << ".";
            //Aktuelles Element wird jetzt abschließend betrachtet.
            activeNodeLongID = heap.removeMinimumCostElement();
            activeNodeShortID = RoutingNode::convertIDToShortFormat(activeNodeLongID);
            activeNode = nodeMap[activeNodeShortID];
            closedList->addElement(activeNodeLongID, S_THREAD);
            
            //std::cerr << activeNodeLongID << ":" << activeNodeShortID;
            
            //Wenn jetzt ein überlappendes Element gefunden wurde: Fertig.
            if (closedList->getOverlappingElement() != 0)
            {
                //std::cerr << "found endnode!";
                break;
            }
            //Hole Liste von Kanten, die in abschließend betrachtetem
            //Knoten beginnen und bearbeite sie.
            QVector<boost::shared_ptr<RoutingEdge> > edgeList = 
                _dbA->getEdgesByStartNodeID(activeNodeLongID);
            for (QVector<boost::shared_ptr<RoutingEdge> >::iterator it =
                edgeList.begin(); it < edgeList.end(); it++)
            {
                //ID zwischenspeichern, damit sie nicht immer neu berechnet werden muss
                boost::uint64_t activeEdgeEndNodeLongID = (*it)->getEndNodeID();
                //Wenn der Endknoten noch nicht abschließend betrachtet wurde (müsste immer wahr sein...)
                if (!closedList->contains((*it)->getEndNodeID(), S_THREAD))
                {
                    //Erstmal Knoten aus der DB holen und puffern, wenn er noch nicht geladen wurde
                    boost::uint64_t activeEdgeEndNodeShortID =
                        RoutingNode::convertIDToShortFormat((*it)->getEndNodeID());
                    boost::shared_ptr<RoutingNode> activeEdgeEndNode;
                    if (!nodeMap.contains(activeEdgeEndNodeShortID))
                    {
                        activeEdgeEndNode = _dbA->getNodeByID(activeEdgeEndNodeShortID);
                        nodeMap.insert(activeEdgeEndNodeShortID, activeEdgeEndNode);
                    }
                    else
                    {
                        activeEdgeEndNode = nodeMap[activeEdgeEndNodeShortID];
                    }
                    
                    
                    //Wurde der Knoten schon einmal betrachtet, oder nicht?
                    if (!heap.contains(activeEdgeEndNodeLongID))
                    {
                        //Neuen Knoten zum Heap dazu, nachdem neue Kosten gesetzt wurden.
                        nodeCosts.setValue(activeEdgeEndNodeLongID, nodeCosts.getValue(activeNodeLongID) + 
                            _metric->rateEdge(**it, *activeNode, *activeEdgeEndNode));
                        heap.add(activeEdgeEndNodeLongID);
                        //Vorgänger-Zeiger setzen
                        predecessor.insert(activeEdgeEndNodeLongID, activeNodeLongID);
                    }
                    else
                    {
                        double newCosts = nodeCosts.getValue(activeNodeLongID) + 
                            _metric->rateEdge(**it, *activeNode, *activeEdgeEndNode);
                        if (newCosts < nodeCosts.getValue(activeEdgeEndNodeLongID))
                        {
                            nodeCosts.setValue(activeEdgeEndNodeLongID, newCosts);
                            heap.decreaseKey(activeEdgeEndNodeLongID);
                            predecessor.insert(activeEdgeEndNodeLongID, activeNodeLongID);
                        }
                    }
                }
            }
        }
        
        std::cerr << "finished, search space of thread A contains " << nodeMap.size() << " nodes." << std::endl;
        
        if (closedList->getOverlappingElement() != 0)
        {
            boost::uint64_t activeNodeID = closedList->getOverlappingElement();
            GPSRoute route;
            while (activeNodeID != 0)
            {
                route.insertForward(*nodeMap[RoutingNode::convertIDToShortFormat(activeNodeID)]);
                activeNodeID = predecessor[activeNodeID];
            }
            return route;
        }
        else
            return GPSRoute();
    }
}
GPSRoute MultithreadedDijkstraRouter::calculateShortestRouteThreadB(const RoutingNode& endNode, MultiThreadedHashClosedList* closedList)
{
    if (!_dbB->isDBOpen())
    {
        std::cerr << "database file A is closed." << std::endl;
        return GPSRoute();
    }
    else
    {
        //Initialisiere Datenstrukturen
        //std::cerr << "init data structures" << std::endl;
        NodeCostLessAndQHashFunctor<boost::uint64_t, double> nodeCosts;
        BinaryHeap<boost::uint64_t, NodeCostLessAndQHashFunctor<boost::uint64_t, double> > heap(nodeCosts);
        QHash<boost::uint64_t, boost::uint64_t> successor;
        QHash<boost::uint64_t, boost::shared_ptr<RoutingNode> > nodeMap;
        
        //Startknoten: Kosten auf Null setzen, zum Heap und Puffer hinzufügen, Vorgänger auf Null setzen
        boost::uint64_t activeNodeLongID = RoutingNode::convertIDToLongFormat(endNode.getID());
        boost::uint64_t activeNodeShortID = RoutingNode::convertIDToShortFormat(endNode.getID());
        
        nodeCosts.setValue(activeNodeLongID, 0.0);
        heap.add(activeNodeLongID);
        nodeMap.insert(activeNodeShortID, _dbB->getNodeByID(endNode.getID()));
        successor.insert(activeNodeLongID, 0);
        
        boost::shared_ptr<RoutingNode> activeNode;
        
        boost::uint64_t endNodeShortID = RoutingNode::convertIDToShortFormat(endNode.getID());
        
        //TODO: Knoten vorladen, damit die DB nicht so oft gefragt werden muss (einmal am Stück ist schneller)
        //TODO: nodeMap evtl ersetzen durch den DatabaseRAMCache?
        
        //std::cerr << "starting while loop" << std::endl;
        while (!heap.isEmpty())
        {
            //std::cerr << ".";
            //Aktuelles Element wird jetzt abschließend betrachtet.
            activeNodeLongID = heap.removeMinimumCostElement();
            activeNodeShortID = RoutingNode::convertIDToShortFormat(activeNodeLongID);
            activeNode = nodeMap[activeNodeShortID];
            closedList->addElement(activeNodeLongID, T_THREAD);
            
            //std::cerr << activeNodeLongID << ":" << activeNodeShortID;
            
            //Wenn jetzt ein überlappendes Element gefunden wurde: Fertig.
            if (closedList->getOverlappingElement() != 0)
            {
                //std::cerr << "found endnode!";
                break;
            }
            //Hole Liste von Kanten, die in abschließend betrachtetem
            //Knoten beginnen und bearbeite sie.
            QVector<boost::shared_ptr<RoutingEdge> > edgeList = 
                _dbB->getEdgesByEndNodeID(activeNodeLongID);
            for (QVector<boost::shared_ptr<RoutingEdge> >::iterator it =
                edgeList.begin(); it < edgeList.end(); it++)
            {
                //ID zwischenspeichern, damit sie nicht immer neu berechnet werden muss
                boost::uint64_t activeEdgeStartNodeLongID = (*it)->getStartNodeID();
                //Wenn der Endknoten noch nicht abschließend betrachtet wurde (müsste immer wahr sein...)
                if (!closedList->contains((*it)->getStartNodeID(), T_THREAD))
                {
                    //Erstmal Knoten aus der DB holen und puffern, wenn er noch nicht geladen wurde
                    boost::uint64_t activeEdgeStartNodeShortID =
                        RoutingNode::convertIDToShortFormat((*it)->getStartNodeID());
                    boost::shared_ptr<RoutingNode> activeEdgeStartNode;
                    if (!nodeMap.contains(activeEdgeStartNodeShortID))
                    {
                        activeEdgeStartNode = _dbB->getNodeByID(activeEdgeStartNodeShortID);
                        nodeMap.insert(activeEdgeStartNodeShortID, activeEdgeStartNode);
                    }
                    else
                    {
                        activeEdgeStartNode = nodeMap[activeEdgeStartNodeShortID];
                    }
                    
                    
                    //Wurde der Knoten schon einmal betrachtet, oder nicht?
                    if (!heap.contains(activeEdgeStartNodeLongID))
                    {
                        //Neuen Knoten zum Heap dazu, nachdem neue Kosten gesetzt wurden.
                        nodeCosts.setValue(activeEdgeStartNodeLongID, nodeCosts.getValue(activeNodeLongID) + 
                            _metric->rateEdge(**it, *activeEdgeStartNode, *activeNode));
                        heap.add(activeEdgeStartNodeLongID);
                        //Vorgänger-Zeiger setzen
                        successor.insert(activeEdgeStartNodeLongID, activeNodeLongID);
                    }
                    else
                    {
                        double newCosts = nodeCosts.getValue(activeNodeLongID) + 
                            _metric->rateEdge(**it, *activeEdgeStartNode, *activeNode);
                        if (newCosts < nodeCosts.getValue(activeEdgeStartNodeLongID))
                        {
                            nodeCosts.setValue(activeEdgeStartNodeLongID, newCosts);
                            heap.decreaseKey(activeEdgeStartNodeLongID);
                            successor.insert(activeEdgeStartNodeLongID, activeNodeLongID);
                        }
                    }
                }
            }
        }
        
        std::cerr << "finished, search space of thread B contains " << nodeMap.size() << " nodes." << std::endl;
        
        if (closedList->getOverlappingElement() != 0)
        {
            boost::uint64_t activeNodeID = closedList->getOverlappingElement();
            GPSRoute route;
            while (activeNodeID != 0)
            {
                route.insertBackward(*nodeMap[RoutingNode::convertIDToShortFormat(activeNodeID)]);
                activeNodeID = successor[activeNodeID];
            }
            return route;
        }
        else
            return GPSRoute();
    }
}

GPSRoute MultithreadedDijkstraRouter::calculateShortestRoute(const GPSPosition& startPosition, const GPSPosition& endPosition)
{
    if (!_dbA->isDBOpen())
    {
        std::cerr << "database file A is closed." << std::endl;
        return GPSRoute();
    }
    else if (!_dbB->isDBOpen())
    {
        std::cerr << "database file B is closed." << std::endl;
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
        nodeList = _dbA->getNodes(startPosition, 50.0);
        if (nodeList.isEmpty())
        {
            nodeList = _dbA->getNodes(startPosition, 500.0);
            if (nodeList.isEmpty())
            {
                nodeList = _dbA->getNodes(startPosition, 5000.0);
                if (nodeList.isEmpty())
                {
                    std::cerr << "did not find a matching starting point." << std::endl;
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
        
        nodeList = _dbB->getNodes(endPosition, 50.0);
        if (nodeList.isEmpty())
        {
            nodeList = _dbB->getNodes(endPosition, 500.0);
            if (nodeList.isEmpty())
            {
                nodeList = _dbB->getNodes(endPosition, 5000.0);
                if (nodeList.isEmpty())
                {
                    std::cerr << "did not find a matching end point" << std::endl;
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
        
        return calculateShortestRoute(startNode, endNode);
    }
}

GPSRoute MultithreadedDijkstraRouter::calculateShortestRoute(const RoutingNode& startNode, const RoutingNode& endNode)
{
    //QFuture<bool> future = QtConcurrent::run(_pbfParser.get(), &PBFParser::parse, fileToParse);
    MultiThreadedHashClosedList closedList;
    QFuture<GPSRoute> futureA = QtConcurrent::run(this, &MultithreadedDijkstraRouter::calculateShortestRouteThreadA, startNode, &closedList);
    QFuture<GPSRoute> futureB = QtConcurrent::run(this, &MultithreadedDijkstraRouter::calculateShortestRouteThreadB, endNode, &closedList);
    
    futureA.waitForFinished();
    futureB.waitForFinished();
    
    //TODO: Route ausrechnen
    if (closedList.getOverlappingElement() == 0)
    {
        std::cerr << "no route found." << std::endl;
        return GPSRoute();
    }
    else
    {
        std::cerr << "overlapping element: " << closedList.getOverlappingElement() << std::endl;
        return (futureA.result() << futureB.result());
    }
    return GPSRoute();
}


namespace biker_tests
{
    int testDijkstraRouter()
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
        
        DijkstraRouter router(db, metric);
        std::cerr << "routing...." << std::endl;
        route = router.calculateShortestRoute(GPSPosition(51.447, 7.2676), GPSPosition(51.4492, 7.2592));
        
        CHECK(!route.isEmpty());
        route.exportGPX("dijkstra.gpx");
        route.exportJSON("dijkstra.js");
        
        return EXIT_SUCCESS;
    }
    
    int testMultithreadedDijkstraRouter()
    {
        #ifdef SPATIALITE_FOUND
            boost::shared_ptr<SpatialiteDatabaseConnection> dbA(new SpatialiteDatabaseConnection());
            boost::shared_ptr<SpatialiteDatabaseConnection> dbB(new SpatialiteDatabaseConnection());
        #else
            boost::shared_ptr<SQLiteDatabaseConnection> dbA(new SQLiteDatabaseConnection());
            boost::shared_ptr<SQLiteDatabaseConnection> dbB(new SQLiteDatabaseConnection());
        #endif
        boost::shared_ptr<RoutingMetric> metric(new EuclidianRoutingMetric());
        dbA->open("rub.db");
        dbB->open("rub.db");
        
        CHECK(dbA->isDBOpen());
        CHECK(dbB->isDBOpen());
        
        GPSRoute route;
        CHECK(route.isEmpty());
        
        MultithreadedDijkstraRouter router(dbA, dbB, metric);
        std::cerr << "routing...." << std::endl;
        route = router.calculateShortestRoute(GPSPosition(51.447, 7.2676), GPSPosition(51.4492, 7.2592));
        
        CHECK(!route.isEmpty());
        route.exportGPX("multithreadeddijkstra.gpx");
        route.exportJSON("multithreadeddijkstra.js");
        
        return EXIT_SUCCESS;

    }
}
