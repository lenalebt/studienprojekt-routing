#include "dijkstra.hpp"
#include "tests.hpp"
#include <limits>
#include "heap.hpp"
#include "spatialitedatabase.hpp"
#include "sqlitedatabase.hpp"
#include <QtConcurrentRun>


GPSRoute DijkstraRouter::calculateShortestRoute(const RoutingNode& startNode, const RoutingNode& endNode)
{
    if (!_db->isDBOpen())
    {
        std::cerr << "database file is closed." << std::endl;
        return GPSRoute();
    }
    else
    {
        //Lesen in einer Transaktion soll helfen bei Geschwindigkeit
        _db->beginTransaction();
        
        //Initialisiere Datenstrukturen
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
        
        boost::uint64_t endNodeShortID = RoutingNode::convertIDToShortFormat(endNode.getID());
        
        QVector<boost::shared_ptr<RoutingNode> > nodes = _db->getNodes(startNode, startNode.calcDistance(endNode)/1.5);
        for (QVector<boost::shared_ptr<RoutingNode> >::const_iterator it = nodes.constBegin(); it != nodes.constEnd(); it++)
        {
            nodeMap.insert(RoutingNode::convertIDToShortFormat((*it)->getID()), *it);
        }
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
        _db->endTransaction();
        
        std::cerr << "finished, search space contains " << nodeMap.size() << " nodes." << std::endl;
        
        if (activeNodeShortID == endNodeShortID)
        {
            boost::uint64_t activeNodeID = activeNodeLongID;
            GPSRoute route;
            if (_metric->getMeasurementUnit() == SECONDS)
            {
                route.setDuration(nodeCosts.getValue(activeNodeID));
            }
            else
            {
                //TODO: Kanten mit Zeit bewerten!
                route.setDuration(nodeCosts.getValue(activeNodeID)/4.0);
            }
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
         * - Evtl. Routenbeschreibung erzeugen?
         */
    }
}

DijkstraRouter::DijkstraRouter(boost::shared_ptr<DatabaseConnection> db, boost::shared_ptr<RoutingMetric> metric) :
    Router(metric), _db(db)
{
    
}

MultithreadedDijkstraRouter::MultithreadedDijkstraRouter(boost::shared_ptr<DatabaseConnection> dbA, boost::shared_ptr<DatabaseConnection> dbB, boost::shared_ptr<RoutingMetric> metric) :
    Router(metric), _dbA(dbA), _dbB(dbB)
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
        //Transaktionen sollen auch beim Lesen helfen
        _dbA->beginTransaction();
        
        //Initialisiere Datenstrukturen
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
        _dbA->endTransaction();
        
        std::cerr << "finished, search space of thread A contains " << nodeMap.size() << " nodes." << std::endl;
        
        if (closedList->getOverlappingElement() != 0)
        {
            boost::uint64_t activeNodeID = closedList->getOverlappingElement();
            GPSRoute route;
            if (_metric->getMeasurementUnit() == SECONDS)
                route.setDuration(nodeCosts.getValue(activeNodeID));
            else
            {
                //TODO: Kanten mit Zeit bewerten!
                route.setDuration(nodeCosts.getValue(activeNodeID)/4.0);
            }
            
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
        //Transaktionen sollen auch beim Lesen helfen
        _dbB->beginTransaction();
        
        //Initialisiere Datenstrukturen
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
        _dbB->endTransaction();
        
        std::cerr << "finished, search space of thread B contains " << nodeMap.size() << " nodes." << std::endl;
        
        if (closedList->getOverlappingElement() != 0)
        {
            boost::uint64_t activeNodeID = closedList->getOverlappingElement();
            GPSRoute route;
            if (_metric->getMeasurementUnit() == SECONDS)
                route.setDuration(nodeCosts.getValue(activeNodeID));
            else
            {
                //TODO: Kanten mit Zeit bewerten!
                route.setDuration(nodeCosts.getValue(activeNodeID)/4.0);
            }
            
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

GPSRoute MultithreadedDijkstraRouter::calculateShortestRoute(const RoutingNode& startNode, const RoutingNode& endNode)
{
    //QFuture<bool> future = QtConcurrent::run(_pbfParser.get(), &PBFParser::parse, fileToParse);
    MultiThreadedHashClosedList closedList;
    QFuture<GPSRoute> futureA = QtConcurrent::run(this, &MultithreadedDijkstraRouter::calculateShortestRouteThreadA, startNode, &closedList);
    QFuture<GPSRoute> futureB = QtConcurrent::run(this, &MultithreadedDijkstraRouter::calculateShortestRouteThreadB, endNode, &closedList);
    
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
