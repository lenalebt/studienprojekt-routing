#include "osmway.hpp"

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
