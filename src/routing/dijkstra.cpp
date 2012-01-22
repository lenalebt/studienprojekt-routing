#include "dijkstra.hpp"
#include "tests.hpp"
#include <limits>
#include "closedlist.hpp"
#include "heap.hpp"

/**
 * @todo Implementieren
 * @todo Testen
 * @bug Wenn der User eine Position aussucht, bei der ein Knoten, der nicht
 *      am Graphen beteiligt ist, am nächsten ist, wird dieser als Startpunkt
 *      ausgewählt. Fälschlicherweise wird dann keine Route berechnet!
 */
GPSRoute DijkstraRouter::calculateShortestRoute(const GPSPosition& startPosition, const GPSPosition& endPosition)
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
GPSRoute DijkstraRouter::calculateShortestRoute(const RoutingNode& startNode, const RoutingNode& endNode)
{
    if (!_db->isDBOpen())
    {
        return GPSRoute();
    }
    else
    {
        //Initialisiere Datenstrukturen
        HashClosedList closedList;
        NodeCostLessAndQHashFunctor<boost::uint64_t, double> nodeCosts;
        BinaryHeap<boost::uint64_t, NodeCostLessAndQHashFunctor<boost::uint64_t, double> > heap(nodeCosts);
        QHash<boost::uint64_t, boost::uint64_t> predecessor;
        
        //Startknoten zum Heap hinzufügen, Kosten auf Null setzen, Vorgänger auf Null setzen
        nodeCosts.setValue(startNode.getID(), 0.0);
        heap.add(startNode.getID());
        predecessor.insert(startNode.getID(), 0);
        
        boost::uint64_t activeNode = 0;
        
        //TODO: Vorgängerzeiger-Liste führen. Auch als QHash?
        
        while (!heap.isEmpty())
        {
            activeNode = heap.removeMinimumCostElement();
            closedList.addElement(activeNode);
            //TODO
        }
        
        return GPSRoute();
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
