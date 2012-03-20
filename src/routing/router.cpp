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
            RouteCache::getInstance()->addRoute(routepart, pointList[i], pointList[i+1], _metric->getParameterDetails());
        }
    }
    std::cerr << "route ready - delivering now..." << std::endl;
    return route;
}

GPSRoute Router::calculateShortestRoute(const GPSPosition& startPosition, const GPSPosition& endPosition)
{
    if (!_dbA->isDBOpen())
    {
        std::cerr << "database file A is closed." << std::endl;
        return GPSRoute();
    }
    else if (!_dbB->isDBOpen())
    {
        std::cerr << "database file B is closed." << std::endl;
        return GPSRoute();
    }
    else
    {
        RoutingNode startNode, endNode;
        QVector<boost::shared_ptr<RoutingNode> > nodeList;
        
        //Suche zuerst den Startknoten raus, dann den Endknoten. Umkreissuche.
        nodeList = _dbA->getNodes(startPosition, 50.0);
        if (nodeList.isEmpty())
        {
            nodeList = _dbA->getNodes(startPosition, 500.0);
            if (nodeList.isEmpty())
            {
                nodeList = _dbA->getNodes(startPosition, 5000.0);
                if (nodeList.isEmpty())
                {
                    std::cerr << "did not find a matching starting point." << std::endl;
                    //Okay, im Umkreis von 5000m nix gefunden: Dann keine Route gefunden.
                    return GPSRoute();
                }
            }
        }
        //nodeList nach nächstem Knoten durchsuchen.
        float minDistance = std::numeric_limits<float>::max();
        for (QVector<boost::shared_ptr<RoutingNode> >::const_iterator it = nodeList.constBegin();
            it != nodeList.constEnd(); it++)
        {
            float distance = (*it)->calcDistance(startPosition);
            if (distance < minDistance)
            {
                startNode = **it;   //Doppelt dereferenzieren, weil in der Liste boost::shared_ptr stehen
                minDistance = distance;
            }
        }
        //startNode ist der Knoten mit der kürzesten Entfernung zu startPosition.
        
        nodeList = _dbB->getNodes(endPosition, 50.0);
        if (nodeList.isEmpty())
        {
            nodeList = _dbB->getNodes(endPosition, 500.0);
            if (nodeList.isEmpty())
            {
                nodeList = _dbB->getNodes(endPosition, 5000.0);
                if (nodeList.isEmpty())
                {
                    std::cerr << "did not find a matching end point" << std::endl;
                    //Okay, im Umkreis von 5000m nix gefunden: Dann keine Route gefunden.
                    return GPSRoute();
                }
            }
        }
        //nodeList nach nächstem Knoten durchsuchen.
        minDistance = std::numeric_limits<float>::max();
        for (QVector<boost::shared_ptr<RoutingNode> >::const_iterator it = nodeList.constBegin();
            it != nodeList.constEnd(); it++)
        {
            float distance = (*it)->calcDistance(endPosition);
            if (distance < minDistance)
            {
                endNode = **it;   //Doppelt dereferenzieren, weil in der Liste boost::shared_ptr stehen
                minDistance = distance;
            }
        }
        //endNode ist der Knoten mit der kürzesten Entfernung zu endPosition.
        
        return calculateShortestRoute(startNode, endNode);
    }
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
