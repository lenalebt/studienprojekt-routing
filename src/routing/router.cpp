#include "router.hpp" 

GPSRoute Router::calculateShortestRoute(QVector<GPSPosition> pointList)
{
    GPSRoute route;
    
    std::cerr << pointList.size() << std::endl;
    
    for (int i=0; i<pointList.size()-1; i++)
    {
        std::cerr << pointList[i] << " to " << pointList[i+1] << std::endl;
        boost::shared_ptr<GPSRoute> routePtr = RouteCache::getInstance()->getRoute(pointList[i], pointList[i+1], _metric->getParameterDetails());
        if (routePtr.get() != NULL)
        {
            std::cerr << "route part cached, delivering cached route..." << std::endl;
            route << *routePtr;
        }
        else
        {
            std::cerr << "route part not in cache, calculating..." << std::endl;
            GPSRoute routepart = calculateShortestRoute(pointList[i], pointList[i+1]);
            route << routepart;
            RouteCache::getInstance()->addRoute(route, pointList[i], pointList[i+1], _metric->getParameterDetails());
        }
    }
    return route;
}


boost::shared_ptr<GPSRoute> RouteCache::getRoute(const GPSPosition& startPos, const GPSPosition& endPos, QString parameters)
{
    QString cacheString = "(%1/%2)-(%3/%4)-" + parameters;
    cacheString = cacheString.arg(startPos.getLat()).arg(startPos.getLon()).arg(endPos.getLat()).arg(endPos.getLon());
    lock.lockForRead();
    if (routeCache.contains(cacheString))
    {
        boost::shared_ptr<GPSRoute> retPtr = *routeCache[cacheString];
        lock.unlock();
        return retPtr;
    }
    else
    {
        lock.unlock();
        return boost::shared_ptr<GPSRoute>();
    }
}

void RouteCache::addRoute(const GPSRoute& route, const GPSPosition& startPos, const GPSPosition& endPos, QString parameters)
{
    QString cacheString = "(%1/%2)-(%3/%4)-" + parameters;
    cacheString = cacheString.arg(startPos.getLat()).arg(startPos.getLon()).arg(endPos.getLat()).arg(endPos.getLon());
    lock.lockForWrite();
    routeCache.insert(cacheString, new boost::shared_ptr<GPSRoute>(new GPSRoute(route)));
    lock.unlock();
}

RouteCache* RouteCache::instance = NULL;
