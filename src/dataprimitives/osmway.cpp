#include "osmway.hpp"

int OSMWay::isOneway()
{
    if (noOnewayProperties.isEmpty())
    {
        noOnewayProperties << OSMProperty("oneway", "no");
    }
    if (onewayProperties.isEmpty())
    {
        onewayProperties << OSMProperty("oneway", "yes");
        onewayProperties << OSMProperty("oneway", "true");
        onewayProperties << OSMProperty("oneway", "1");
        onewayProperties << OSMProperty("highway", "motorway");
        onewayProperties << OSMProperty("highway", "motorway_link");
        onewayProperties << OSMProperty("highway", "motorway_junction");
        onewayProperties << OSMProperty("junction", "roundabout");
    }
    if (reverseOnewayProperties.isEmpty())
    {
        reverseOnewayProperties << OSMProperty("oneway", "-1");
        reverseOnewayProperties << OSMProperty("oneway", "reverse");
    }
    
    for (QVector<OSMProperty>::const_iterator it = noOnewayProperties.begin();
        it != noOnewayProperties.end(); it++)
    {
        if (properties.contains(*it))
            return 0;
    }
    for (QVector<OSMProperty>::const_iterator it = onewayProperties.begin();
        it != onewayProperties.end(); it++)
    {
        if (properties.contains(*it))
            return 1;
    }
    for (QVector<OSMProperty>::const_iterator it = reverseOnewayProperties.begin();
        it != reverseOnewayProperties.end(); it++)
    {
        if (properties.contains(*it))
            return -1;
    }
    return false;
}

int OSMWay::isOneWayForBikes()
{
    int _isOneway = isOneway();
    if (_isOneway == 0)
        return 0;
    else
    {
        //TODO
        //if ()
    }
}

namespace biker_tests
{
    int testOSMWay()
    {
        OSMWay way(0);
        
        CHECK_EQ_TYPE(way.getID(), 0, boost::uint64_t);
        way.setID(1);
        CHECK_EQ_TYPE(way.getID(), 1, boost::uint64_t);
        
        return EXIT_SUCCESS;
    }
} 
