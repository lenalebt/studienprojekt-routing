#include "osmedge.hpp"

namespace biker_tests
{
    int testOSMEdge()
    {
        OSMEdge edge(0);
        
        CHECK_EQ_TYPE(edge.getID(), 0, boost::uint64_t);
        edge.setID(1);
        CHECK_EQ_TYPE(edge.getID(), 1, boost::uint64_t);
        
        return EXIT_SUCCESS;
    }
} 
