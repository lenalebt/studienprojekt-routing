#ifndef ROUTER_HPP
#define ROUTER_HPP

#include "heap.hpp"
#include "gpsposition.hpp"
#include "gpsroute.hpp"

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
     * @todo 
     */
    virtual GPSRoute calculateShortestRoute(GPSPosition startPosition, GPSPosition endPosition)=0;
};

#endif //ROUTER_HPP
