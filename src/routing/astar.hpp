#ifndef ASTAR_HPP
#define ASTAR_HPP

#include "router.hpp"
#include "routingnode.hpp"
#include "database.hpp"
#include "routingmetric.hpp"
#include "closedlist.hpp"

template<typename K, typename V>
class NodeCostLessAndQHashFunctorStar
{
private:
    QHash<K, V> hashMap;
public:
    NodeCostLessAndQHashFunctorStar()
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
template class NodeCostLessAndQHashFunctorStar<boost::uint64_t, double>;

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
    boost::shared_ptr<DatabaseConnection> _db;
    boost::shared_ptr<RoutingMetric> _metric;
    
    GPSRoute calculateShortestRoute(const RoutingNode& startNode, const RoutingNode& endNode);
public:
    AStarRouter(boost::shared_ptr<DatabaseConnection> db, boost::shared_ptr<RoutingMetric> metric);
    GPSRoute calculateShortestRoute(const GPSPosition& startPosition, const GPSPosition& endPosition);
};

/**
 * @brief Implementiert den Algorithmus von Dijkstra mit zwei Threads:
 *      Einen vom Start-, und einen vom Zielpunkt aus.
 * 
 * @author Lena Brüder
 * @date 2011-12-23
 * @copyright GNU GPL v3
 * @todo Umbauen, atm ist das nur der Mehrthreaddijkstra.
 * @ingroup routing
 */
class MultithreadedAStarRouter : public Router
{
private:
    boost::shared_ptr<DatabaseConnection> _dbA;
    boost::shared_ptr<DatabaseConnection> _dbB;
    boost::shared_ptr<RoutingMetric> _metric;
    
    GPSRoute calculateShortestRoute(const RoutingNode& startNode, const RoutingNode& endNode);
    /**
     * @brief Berechnet eine Route vom Startpunkt aus in Richtung Ziel.
     * 
     * Das Ziel wird durch den anderen Thread festgelegt. Wenn sie sich treffen, ist das
     * Routing abgeschlossen.
     * 
     * @return Die Route vom Startpunkt zum gemeinsamen Punkt
     */
    GPSRoute calculateShortestRouteThreadA(const RoutingNode& startNode, const RoutingNode& endNode, MultiThreadedHashClosedList* closedList);
    /**
     * @brief Berechnet eine Route vom Ziel aus in Richtung Startpunkt.
     * 
     * Der Start wird durch den anderen Thread festgelegt. Wenn sie sich treffen, ist das
     * Routing abgeschlossen.
     * 
     * @return Die Route vom gemeinsamen Punkt zum Endpunkt
     */
    GPSRoute calculateShortestRouteThreadB(const RoutingNode& startNode, const RoutingNode& endNode, MultiThreadedHashClosedList* closedList);
public:
    /**
     * @brief Erstellt einen neuen Router, der den A*-Algorithmus
     *  in einer mehr-Thread-Version ausführt.
     * 
     * Es müssen 2 Datenbankverbindungen angegeben werden, weil SQLite nicht
     * aus mehreren Threads aufgerufen werden kann. Diese Einschränkung haben
     * wir bisher noch nicht schöner implementieren können, als einfach 2
     * Datenbankobjekte zu übergeben.
     * 
     * @param dbA Die Datenbank für Thread A
     * @param dbB Die Datenbank für Thread B
     * @param metric Die Routingmetrik, die zum Bewerten von Kanten verwendet werden soll
     */
    MultithreadedAStarRouter(boost::shared_ptr<DatabaseConnection> dbA, boost::shared_ptr<DatabaseConnection> dbB, boost::shared_ptr<RoutingMetric> metric);
    GPSRoute calculateShortestRoute(const GPSPosition& startPosition, const GPSPosition& endPosition);
};

namespace biker_tests
{
    int testAStarRouter();
    int testMultithreadedAStarRouter();
}

#endif //ASTAR_HPP
