#include "astar.hpp"
#include "tests.hpp"
#include <limits>
#include "heap.hpp"
#include "spatialitedatabase.hpp"
#include "sqlitedatabase.hpp"
#include <QtConcurrentRun>
#include "dijkstra.hpp"

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

        boost::uint64_t activeNodeLongID = RoutingNode::convertIDToLongFormat(startNode.getID());
        boost::uint64_t activeNodeShortID = RoutingNode::convertIDToShortFormat(startNode.getID());
        
        //Startknoten: Kosten auf Null setzen, zum Heap und Puffer hinzufügen, Vorgänger auf Null setzen
        estimatedCosts.setValue(activeNodeLongID, _metric->estimateDistance(startNode, endNode));
        nodeCosts[activeNodeLongID] = 0.0;
        heap.add(activeNodeLongID);
        nodeMap.insert(activeNodeShortID, _db->getNodeByID(startNode.getID()));
        predecessor.insert(activeNodeLongID, 0);
        
        boost::shared_ptr<RoutingNode> activeNode;
        
        boost::uint64_t endNodeShortID = RoutingNode::convertIDToShortFormat(endNode.getID());
        
        //TODO: Knoten vorladen, damit die DB nicht so oft gefragt werden muss (einmal am Stück ist schneller)
        //TODO: nodeMap evtl ersetzen durch den DatabaseRAMCache?
        
        while (!heap.isEmpty())
        {
            //Aktuelles Element wird jetzt abschließend betrachtet.
            activeNodeLongID = heap.removeMinimumCostElement();
            activeNodeShortID = RoutingNode::convertIDToShortFormat(activeNodeLongID);
            activeNode = nodeMap[activeNodeShortID];
            closedList.addElement(activeNodeLongID);
            
            //Wenn der jetzt abschließend zu betrachtende Knoten der Endkonten ist: Fertig.
            if (activeNodeShortID == endNodeShortID)
            {
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
                        
                        nodeCosts[activeEdgeEndNodeLongID] = nodeCosts[activeNodeLongID] +
                            _metric->rateEdge(**it, *activeNode, *activeEdgeEndNode);
                        //---------*activeNode oder *it benutzen??//
                        float distance = _metric->estimateDistance(*activeNode, endNode);
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
                            float distance = _metric->estimateDistance(*activeNode, endNode);
                            estimatedCosts.setValue(activeEdgeEndNodeLongID, newCosts + distance); 
                            heap.decreaseKey(activeEdgeEndNodeLongID);
                            predecessor.insert(activeEdgeEndNodeLongID, activeNodeLongID);
                        }
                    }
                }
            }
        }
        
        std::cerr << "finished, search space contains " << nodeMap.size() << " elements." << std::endl;
        
        if (activeNodeShortID == endNodeShortID)
        {
            boost::uint64_t activeNodeID = activeNodeLongID;
            boost::uint64_t pred = 0;
            GPSRoute route;
            double duration = 0.0;
            
            
            while (activeNodeID != 0)
            {
                if ((pred != 0) && (RoutingNode::convertIDToShortFormat(activeNodeID) != RoutingNode::convertIDToShortFormat(pred)))
                {
                    QVector<boost::shared_ptr<RoutingEdge> > edges = _db->getEdgesByStartNodeID(activeNodeID);
                    boost::shared_ptr<RoutingEdge> rEdge;
                    for (QVector<boost::shared_ptr<RoutingEdge> >::iterator it = edges.begin(); it != edges.end(); it++)
                    {
                        if ((*it)->getEndNodeID() == pred)
                        {
                            rEdge = *it;
                        }
                    }
                    duration += _metric->timeEdge(*rEdge, *nodeMap[RoutingNode::convertIDToShortFormat(pred)], *nodeMap[RoutingNode::convertIDToShortFormat(activeNodeID)]);
                }
                
                route.insertForward(*nodeMap[RoutingNode::convertIDToShortFormat(activeNodeID)]);
                pred = activeNodeID;
                activeNodeID = predecessor[activeNodeID];
            }
            route.setDuration(duration);
            return route;
        }
        else
        {
            std::cerr << "did not find a route." << std::endl;
            return GPSRoute();
        }
        
        /* TODO:
         * - Evtl. Routenbeschreibung erzeugen?
         */
    }
}

AStarRouter::AStarRouter(boost::shared_ptr<DatabaseConnection> db, boost::shared_ptr<RoutingMetric> metric) :
    Router(metric, db)
{
    
}

MultithreadedAStarRouter::MultithreadedAStarRouter(boost::shared_ptr<DatabaseConnection> dbA, boost::shared_ptr<DatabaseConnection> dbB, boost::shared_ptr<RoutingMetric> metric) :
    Router(metric, dbA, dbB)
{
    
}

GPSRoute MultithreadedAStarRouter::calculateShortestRouteThreadA(const RoutingNode& startNode, const RoutingNode& endNode, MultiThreadedHashClosedList* closedList)
{
    if (!_dbA->isDBOpen())
    {
        std::cerr << "database file A is closed." << std::endl;
        return GPSRoute();
    }
    else
    {
        //Initialisiere Datenstrukturen
        NodeCostLessAndQHashFunctorStar<boost::uint64_t, double> estimatedCosts;
        QHash<boost::uint64_t, boost::uint64_t> nodeCosts;
        BinaryHeap<boost::uint64_t, NodeCostLessAndQHashFunctorStar<boost::uint64_t, double> > heap(estimatedCosts);
        QHash<boost::uint64_t, boost::uint64_t> predecessor;
        QHash<boost::uint64_t, boost::shared_ptr<RoutingNode> > nodeMap;
        
        //Startknoten: Kosten auf Null setzen, zum Heap und Puffer hinzufügen, Vorgänger auf Null setzen
        boost::uint64_t activeNodeLongID = RoutingNode::convertIDToLongFormat(startNode.getID());
        boost::uint64_t activeNodeShortID = RoutingNode::convertIDToShortFormat(startNode.getID());
        
        nodeCosts[activeNodeLongID] = 0.0;
        estimatedCosts.setValue(activeNodeLongID, _metric->estimateDistance(startNode, endNode));
        heap.add(activeNodeLongID);
        nodeMap.insert(activeNodeShortID, _dbA->getNodeByID(startNode.getID()));
        predecessor.insert(activeNodeLongID, 0);
        
        boost::shared_ptr<RoutingNode> activeNode;
        
        /*QVector<boost::shared_ptr<RoutingNode> > nodes = _dbA->getNodes(startNode, startNode.calcDistance(endNode)/2.5);
        for (QVector<boost::shared_ptr<RoutingNode> >::const_iterator it = nodes.constBegin(); it != nodes.constEnd(); it++)
        {
            nodeMap.insert(RoutingNode::convertIDToShortFormat((*it)->getID()), *it);
        }*/
        
        //TODO: Knoten vorladen, damit die DB nicht so oft gefragt werden muss (einmal am Stück ist schneller)
        //TODO: nodeMap evtl ersetzen durch den DatabaseRAMCache?
        
        while (!heap.isEmpty())
        {
            //Aktuelles Element wird jetzt abschließend betrachtet.
            activeNodeLongID = heap.removeMinimumCostElement();
            activeNodeShortID = RoutingNode::convertIDToShortFormat(activeNodeLongID);
            activeNode = nodeMap[activeNodeShortID];
            closedList->addElement(activeNodeLongID, S_THREAD);
            
            //Wenn jetzt ein überlappendes Element gefunden wurde: Fertig.
            if (closedList->getOverlappingElement() != 0)
            {
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
                        nodeCosts[activeEdgeEndNodeLongID] = nodeCosts[activeNodeLongID] +
                            _metric->rateEdge(**it, *activeNode, *activeEdgeEndNode);
                        float distance = _metric->estimateDistance(*activeNode, endNode);
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
                            float distance = _metric->estimateDistance(*activeNode, endNode);
                            estimatedCosts.setValue(activeEdgeEndNodeLongID, newCosts + distance); 
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
            boost::uint64_t pred = 0;
            GPSRoute route;
            double duration = 0.0;
            while (activeNodeID != 0)
            {
                if ((pred != 0) && (RoutingNode::convertIDToShortFormat(activeNodeID) != RoutingNode::convertIDToShortFormat(pred)))
                {
                    QVector<boost::shared_ptr<RoutingEdge> > edges = _db->getEdgesByStartNodeID(activeNodeID);
                    boost::shared_ptr<RoutingEdge> rEdge;
                    for (QVector<boost::shared_ptr<RoutingEdge> >::iterator it = edges.begin(); it != edges.end(); it++)
                    {
                        if ((*it)->getEndNodeID() == pred)
                        {
                            rEdge = *it;
                        }
                    }
                    duration += _metric->timeEdge(*rEdge, *nodeMap[RoutingNode::convertIDToShortFormat(pred)], *nodeMap[RoutingNode::convertIDToShortFormat(activeNodeID)]);
                }
                
                route.insertForward(*nodeMap[RoutingNode::convertIDToShortFormat(activeNodeID)]);
                pred = activeNodeID;
                activeNodeID = predecessor[activeNodeID];
            }
            route.setDuration(duration);
            return route;
        }
        else
            return GPSRoute();
    }
}
GPSRoute MultithreadedAStarRouter::calculateShortestRouteThreadB(const RoutingNode& startNode, const RoutingNode& endNode, MultiThreadedHashClosedList* closedList)
{
    if (!_dbB->isDBOpen())
    {
        std::cerr << "database file A is closed." << std::endl;
        return GPSRoute();
    }
    else
    {
        //Initialisiere Datenstrukturen
        NodeCostLessAndQHashFunctorStar<boost::uint64_t, double> estimatedCosts;
        QHash<boost::uint64_t, boost::uint64_t> nodeCosts;
        BinaryHeap<boost::uint64_t, NodeCostLessAndQHashFunctorStar<boost::uint64_t, double> > heap(estimatedCosts);
        QHash<boost::uint64_t, boost::uint64_t> successor;
        QHash<boost::uint64_t, boost::shared_ptr<RoutingNode> > nodeMap;
        
        //Startknoten: Kosten auf Null setzen, zum Heap und Puffer hinzufügen, Vorgänger auf Null setzen
        boost::uint64_t activeNodeLongID = RoutingNode::convertIDToLongFormat(endNode.getID());
        boost::uint64_t activeNodeShortID = RoutingNode::convertIDToShortFormat(endNode.getID());
        
        nodeCosts[activeNodeLongID] = 0.0;
        estimatedCosts.setValue(activeNodeLongID, _metric->estimateDistance(endNode, startNode));
        heap.add(activeNodeLongID);
        nodeMap.insert(activeNodeShortID, _dbB->getNodeByID(endNode.getID()));
        successor.insert(activeNodeLongID, 0);
        
        boost::shared_ptr<RoutingNode> activeNode;
        
        /*QVector<boost::shared_ptr<RoutingNode> > nodes = _dbB->getNodes(startNode, startNode.calcDistance(endNode)/2.5);
        for (QVector<boost::shared_ptr<RoutingNode> >::const_iterator it = nodes.constBegin(); it != nodes.constEnd(); it++)
        {
            nodeMap.insert(RoutingNode::convertIDToShortFormat((*it)->getID()), *it);
        }*/
        
        //TODO: Knoten vorladen, damit die DB nicht so oft gefragt werden muss (einmal am Stück ist schneller)
        //TODO: nodeMap evtl ersetzen durch den DatabaseRAMCache?
        
        while (!heap.isEmpty())
        {
            //Aktuelles Element wird jetzt abschließend betrachtet.
            activeNodeLongID = heap.removeMinimumCostElement();
            activeNodeShortID = RoutingNode::convertIDToShortFormat(activeNodeLongID);
            activeNode = nodeMap[activeNodeShortID];
            closedList->addElement(activeNodeLongID, T_THREAD);
            
            //Wenn jetzt ein überlappendes Element gefunden wurde: Fertig.
            if (closedList->getOverlappingElement() != 0)
            {
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
                        
                        nodeCosts[activeEdgeStartNodeLongID] = nodeCosts[activeNodeLongID] +
                            _metric->rateEdge(**it, *activeEdgeStartNode, *activeNode);
                        float distance = _metric->estimateDistance(startNode, *activeNode);
                        estimatedCosts.setValue(activeEdgeStartNodeLongID, nodeCosts[activeNodeLongID] + 
                            _metric->rateEdge(**it, *activeEdgeStartNode, *activeNode) + distance);

                        heap.add(activeEdgeStartNodeLongID);
                        //Vorgänger-Zeiger setzen
                        successor.insert(activeEdgeStartNodeLongID, activeNodeLongID);
                    }
                    else
                    {
                        
                        double newCosts = nodeCosts[activeNodeLongID] + 
                            _metric->rateEdge(**it,*activeEdgeStartNode, *activeNode);
                            
                        if (newCosts < nodeCosts[activeEdgeStartNodeLongID])
                        {
                            
                            
                            nodeCosts[activeEdgeStartNodeLongID] = newCosts;
                            float distance = _metric->estimateDistance(startNode, *activeNode);
                            estimatedCosts.setValue(activeEdgeStartNodeLongID, newCosts + distance); 
                            
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
            boost::uint64_t succ = 0;
            GPSRoute route;
            double duration = 0.0;
            while (activeNodeID != 0)
            {
                if ((succ != 0) && (RoutingNode::convertIDToShortFormat(activeNodeID) != RoutingNode::convertIDToShortFormat(succ)))
                {
                    QVector<boost::shared_ptr<RoutingEdge> > edges = _db->getEdgesByEndNodeID(activeNodeID);
                    boost::shared_ptr<RoutingEdge> rEdge;
                    for (QVector<boost::shared_ptr<RoutingEdge> >::iterator it = edges.begin(); it != edges.end(); it++)
                    {
                        if ((*it)->getStartNodeID() == succ)
                        {
                            rEdge = *it;
                        }
                    }
                    duration += _metric->timeEdge(*rEdge, *nodeMap[RoutingNode::convertIDToShortFormat(activeNodeID)], *nodeMap[RoutingNode::convertIDToShortFormat(succ)]);
                }
                
                route.insertBackward(*nodeMap[RoutingNode::convertIDToShortFormat(activeNodeID)]);
                succ = activeNodeID;
                activeNodeID = successor[activeNodeID];
            }
            route.setDuration(duration);
            return route;
        }
        else
            return GPSRoute();
    }
}

GPSRoute MultithreadedAStarRouter::calculateShortestRoute(const RoutingNode& startNode, const RoutingNode& endNode)
{
    //QFuture<bool> future = QtConcurrent::run(_pbfParser.get(), &PBFParser::parse, fileToParse);
    MultiThreadedHashClosedList closedList;
    QFuture<GPSRoute> futureA = QtConcurrent::run(this, &MultithreadedAStarRouter::calculateShortestRouteThreadA, startNode, endNode, &closedList);
    QFuture<GPSRoute> futureB = QtConcurrent::run(this, &MultithreadedAStarRouter::calculateShortestRouteThreadB, startNode, endNode, &closedList);
    
    futureA.waitForFinished();
    futureB.waitForFinished();
    
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
    int testAStarRouter()
    {
        #ifdef SPATIALITE_FOUND
            boost::shared_ptr<SpatialiteDatabaseConnection> db(new SpatialiteDatabaseConnection());
        #else
            boost::shared_ptr<SQLiteDatabaseConnection> db(new SQLiteDatabaseConnection());
        #endif
        boost::shared_ptr<RoutingMetric> metric(new EuclideanRoutingMetric());
        db->open("rub.db");
        
        CHECK(db->isDBOpen());
        
        GPSRoute route;
        CHECK(route.isEmpty());
        
        Router* router = new AStarRouter(db, metric);
        std::cerr << "routing...." << std::endl;
        route = router->calculateShortestRoute(GPSPosition(51.447, 7.2676), GPSPosition(51.4492, 7.2592));
        
        CHECK(!route.isEmpty());
        route.exportGPX("astar.gpx");
        route.exportJSON("astar.js");
        
        return EXIT_SUCCESS;
    }
    
    int testMultithreadedAStarRouter()
    {
        #ifdef SPATIALITE_FOUND
            boost::shared_ptr<SpatialiteDatabaseConnection> dbA(new SpatialiteDatabaseConnection());
            boost::shared_ptr<SpatialiteDatabaseConnection> dbB(new SpatialiteDatabaseConnection());
        #else
            boost::shared_ptr<SQLiteDatabaseConnection> dbA(new SQLiteDatabaseConnection());
            boost::shared_ptr<SQLiteDatabaseConnection> dbB(new SQLiteDatabaseConnection());
        #endif
        boost::shared_ptr<RoutingMetric> metric(new EuclideanRoutingMetric());
        dbA->open("rub.db");
        dbB->open("rub.db");
        
        CHECK(dbA->isDBOpen());
        CHECK(dbB->isDBOpen());
        
        GPSRoute route;
        CHECK(route.isEmpty());
        
        Router* router = new MultithreadedAStarRouter(dbA, dbB, metric);
        std::cerr << "routing...." << std::endl;
        route = router->calculateShortestRoute(GPSPosition(51.447, 7.2676), GPSPosition(51.4492, 7.2592));
        
        CHECK(!route.isEmpty());
        route.exportGPX("multithreadedastar.gpx");
        route.exportJSON("multithreadedastar.js");
        
        return EXIT_SUCCESS;

    }
}
