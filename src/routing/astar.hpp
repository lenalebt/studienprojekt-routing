#ifndef ASTAR_HPP
#define ASTAR_HPP

#include "router.hpp"
#include "routingnode.hpp"
#include "database.hpp"
#include "routingmetric.hpp"

template<typename K, typename V>
class NodeCostLessAndQHashFunctor
{
private:
    QHash<K, V> hashMap;
public:
    NodeCostLessAndQHashFunctor()
    {
        
    }
    bool operator()(K a, K b)
    {
        return (hashMap[a] < hashMap[b]);
    }
    void setValue(K key, V value)
    {
        //Setzt wirklich den Wert und fügt nicht was neues ein
        hashMap.insert(key, value);
    }
    V getValue(K key)
    {
        return hashMap[key];
    }
};
template class NodeCostLessAndQHashFunctor<boost::uint64_t, double>;

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
class AStarRouter : public Router
{
private:
    DatabaseConnection* _db;
    RoutingMetric* _metric;
    
    GPSRoute calculateShortestRoute(const RoutingNode& startNode, const RoutingNode& endNode);
public:
    AStar(DatabaseConnection* db, RoutingMetric* metric);
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
    int testAStarRouter();
}

#endif //ASTAR_HPP
