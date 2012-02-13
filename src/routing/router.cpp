#include "router.hpp" 

GPSRoute Router::calculateShortestRoute(QVector<GPSPosition> pointList)
{
    GPSRoute route;
    
    for (int i=0; i<pointList.size()-1; i++)
    {
        route << calculateShortestRoute(pointList[i], pointList[i+1]);
    }
    return route;
}
