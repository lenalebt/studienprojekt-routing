#ifndef DIJKSTRA_HPP
#define DIJKSTRA_HPP

#include "router.hpp"
#include "routingnode.hpp"
#include "database.hpp"
#include "routingmetric.hpp"
#include "closedlist.hpp"

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
class DijkstraRouter : public Router
{
private:
    boost::shared_ptr<DatabaseConnection> _db;
    boost::shared_ptr<RoutingMetric> _metric;
    
    GPSRoute calculateShortestRoute(const RoutingNode& startNode, const RoutingNode& endNode);
public:
    DijkstraRouter(boost::shared_ptr<DatabaseConnection> db, boost::shared_ptr<RoutingMetric> metric);
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
     * @return Die Vorgängerzeigerliste, die der Algorithmus bestimmt hat.
     * @todo Implementieren
     */
    QHash<boost::uint64_t, boost::uint64_t> calculateShortestRouteThreadA(const RoutingNode& startNode, MultiThreadedHashClosedList* closedList);
    /**
     * @brief Berechnet eine Route vom Ziel aus in Richtung Startpunkt.
     * 
     * Der Start wird durch den anderen Thread festgelegt. Wenn sie sich treffen, ist das
     * Routing abgeschlossen.
     * 
     * @return Die Nachfolgerzeigerliste, die der Algorithmus bestimmt hat.
     * @todo Implementieren
     */
    QHash<boost::uint64_t, boost::uint64_t> calculateShortestRouteThreadB(const RoutingNode& endNode, MultiThreadedHashClosedList* closedList);
public:
    /**
     * @brief Erstellt einen neuen Router, der den Algorithmus von Dijkstra
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
    MultithreadedDijkstraRouter(boost::shared_ptr<DatabaseConnection> dbA, boost::shared_ptr<DatabaseConnection> dbB, boost::shared_ptr<RoutingMetric> metric);
    GPSRoute calculateShortestRoute(const GPSPosition& startPosition, const GPSPosition& endPosition);
};

namespace biker_tests
{
    int testDijkstraRouter();
    int testMultithreadedDijkstraRouter();
}

#endif //DIJKSTRA_HPP
