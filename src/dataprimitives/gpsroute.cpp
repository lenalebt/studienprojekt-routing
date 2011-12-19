#include "gpsroute.hpp" 

#include "gpsposition.hpp"
#include "routingnode.hpp"

namespace biker_tests
{
    int testGPSRoute()
    {
        RoutingNode node;
        GPSPosition pos;
        
        QVector<GPSPosition> list;
        list << pos;
        list << node;
        
        return EXIT_SUCCESS;
    }
}
