#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "heap.hpp"
#include "gpsposition.hpp"
#include "gpsroute.hpp"
#include <QVector>

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
    
public:
    /**
     * @brief Berechnet eine Route in einem Graphen.
     * 
     * 
     * 
     * @param startPosition Die Position, von der aus die Route berechnet werden soll
     * @param endPosition Das Ziel der Route
     * @return Eine Route vom Start zum Ziel, die nach der angegebenen RoutingMetric die beste ist.
     */
    virtual GPSRoute calculateShortestRoute(const GPSPosition& startPosition, const GPSPosition& endPosition)=0;
    
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

#endif //ROUTER_HPP
