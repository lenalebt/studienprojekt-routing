#ifndef DIJKSTRA_HPP
#define DIJKSTRA_HPP

#include "router.hpp"
#include "routingnode.hpp"
#include "database.hpp"

/**
 * @brief Implementiert den klassischen Dijkstra-Algorithmus in einer
 *      Version mit einem Thread.
 * 
 * @author Lena Brüder
 * @date 2011-12-23
 * @copyright GNU GPL v3
 * @todo Implementieren!
 * @ingroup routing
 */
class DijkstraRouter : public Router
{
private:
    DatabaseConnection* _db;
    
    GPSRoute calculateShortestRoute(const RoutingNode& startNode, const RoutingNode& endNode);
public:
    DijkstraRouter(DatabaseConnection* db);
    GPSRoute calculateShortestRoute(const GPSPosition& startPosition, const GPSPosition& endPosition);
};

/**
 * @brief Implementiert den Algorithmus von Dijkstra mit zwei Threads:
 *      Einen vom Start-, und einen vom Zielpunkt aus.
 * 
 * @author Lena Brüder
 * @date 2011-12-23
 * @copyright GNU GPL v3
 * @todo Implementieren!
 * @ingroup routing
 */
class MultithreadedDijkstraRouter : public Router
{
private:
    
public:
    
};

namespace biker_tests
{
    int testDijkstraRouter();
}

#endif //DIJKSTRA_HPP
