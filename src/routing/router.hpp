#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "heap.hpp"
#include "gpsposition.hpp"
#include "gpsroute.hpp"
#include "routingmetric.hpp"
#include <QVector>
#include <QReadWriteLock>
#include <QCache>
#include "database.hpp"

/**
 * @brief Von diesem Interface erben alle Klassen, die eine Route in einem
 *      Grpahen berechnen können.
 * 
 * 
 * 
 * @ingroup routing
 * @author Lena Brueder
 * @date 2011-11-08
 * @copyright GNU GPL v3
 */
class Router
{
private:
    
protected:
    boost::shared_ptr<RoutingMetric> _metric;
    boost::shared_ptr<DatabaseConnection> _db;
    boost::shared_ptr<DatabaseConnection> _dbA;
    boost::shared_ptr<DatabaseConnection> _dbB;
    virtual GPSRoute calculateShortestRoute(const RoutingNode& startNode, const RoutingNode& endNode)=0;
public:
    Router(boost::shared_ptr<RoutingMetric> metric) : _metric(metric) {}
    Router(boost::shared_ptr<RoutingMetric> metric, boost::shared_ptr<DatabaseConnection> db) : _metric(metric), _db(db), _dbA(db), _dbB(db) {}
    Router(boost::shared_ptr<RoutingMetric> metric, boost::shared_ptr<DatabaseConnection> dbA, boost::shared_ptr<DatabaseConnection> dbB) : _metric(metric), _db(dbA), _dbA(dbA), _dbB(dbB) {}
    /**
     * @brief Berechnet eine Route in einem Graphen.
     * 
     * 
     * 
     * @param startPosition Die Position, von der aus die Route berechnet werden soll
     * @param endPosition Das Ziel der Route
     * @return Eine Route vom Start zum Ziel, die nach der angegebenen RoutingMetric die beste ist.
     */
    virtual GPSRoute calculateShortestRoute(const GPSPosition& startPosition, const GPSPosition& endPosition);
    
    /**
     * @brief Berechnet eine Route in einem Graphen über Transitpunkte.
     * 
     * In der Standardimplementierung werden alle Punkte nacheinander geroutet.
     * 
     * @param pointList Alle Punkte, die der Algorithmus anfahren soll, in der Reihenfolge vom Start zum Ziel.
     * @return Eine Route vom Start zum Ziel, die nach der angegebenen RoutingMetric die beste ist.
     */
    virtual GPSRoute calculateShortestRoute(QVector<GPSPosition> pointList);
};

class RouteCache
{
private:
    static RouteCache* instance;
    RouteCache() : routeCache(100), lock() {}
    
    QCache<QString, boost::shared_ptr<GPSRoute> > routeCache;
    QReadWriteLock lock;
public:
    static RouteCache* getInstance()
    {
        if (instance == NULL)
        {
            instance = new RouteCache();
            std::cerr << "created new route cache." << std::endl;
        }
        return instance;
    }
    
    boost::shared_ptr<GPSRoute> getRoute(const GPSPosition& startPos, const GPSPosition& endPos, QString parameters);
    void addRoute(const GPSRoute& route, const GPSPosition& startPos, const GPSPosition& endPos, QString parameters);
};

#endif //ROUTER_HPP
