#include "osmway.hpp"

namespace biker_tests
{
    int testOSMWay()
    {
        OSMWay way(0);
        
        CHECK_EQ(way.getID(), 0ul);
        way.setID(1);
        CHECK_EQ(way.getID(), 1ul);
        
        return EXIT_SUCCESS;
    }
} 
