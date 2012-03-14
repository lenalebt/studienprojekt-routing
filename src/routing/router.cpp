#include "router.hpp" 

GPSRoute Router::calculateShortestRoute(QVector<GPSPosition> pointList)
{
    GPSRoute route;
    
    std::cerr << pointList.size() << std::endl;
    
    for (int i=0; i<pointList.size()-1; i++)
    {
        std::cerr << pointList[i] << " to " << pointList[i+1] << std::endl;
        route << calculateShortestRoute(pointList[i], pointList[i+1]);
    }
    return route;
}
