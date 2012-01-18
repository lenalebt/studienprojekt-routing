#include "dijkstra.hpp"
#include "tests.hpp"

/**
 * @todo: Implementieren
 */
GPSRoute DijkstraRouter::calculateShortestRoute(const GPSPosition& startPosition, const GPSPosition& endPosition)
{
    if (!_db->isDBOpen())
    {
        GPSRoute route;
        return route;
    }
    else
    {
        RoutingNode startNode, endNode;
        QVector<boost::shared_ptr<RoutingNode> > nodeList;
        
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
                    GPSRoute route;
                    return route;
                }
            }
        }
        //TODO: nodeList nach nächstem Knoten durchsuchen.
        
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
                    GPSRoute route;
                    return route;
                }
            }
        }
        //TODO: nodeList nach nächstem Knoten durchsuchen.
        
        //TODO
        GPSRoute route;
        return route;
    }
}

/**
 * @todo: Implementieren
 */
GPSRoute DijkstraRouter::calculateShortestRoute(const RoutingNode& startNode, const RoutingNode& endNode)
{
    if (!_db->isDBOpen())
    {
        GPSRoute route;
        return route;
    }
    else
    {
        //TODO
        GPSRoute route;
        return route;
    }
}

DijkstraRouter::DijkstraRouter(DatabaseConnection* db) :
    _db(db)
{
    
}

namespace biker_tests
{
    int testDijkstraRouter()
    {
        return EXIT_FAILURE;
    }
}
