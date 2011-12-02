#include "osmnode.hpp"
#include <boost/cstdint.hpp>

namespace biker_tests
{
    int testOSMNode()
    {
        OSMNode node;
        CHECK_EQ(node.getLat(), 0.0);
        CHECK_EQ(node.getLon(), 0.0);
        CHECK_EQ_TYPE(node.getID(), 0, boost::uint64_t);
        
        node.setLat(5.0);
        CHECK_EQ(node.getLat(), 5.0);
        
        node.setLon(7.0);
        CHECK_EQ(node.getLon(), 7.0);
        
        OSMNode node2(5, node);
        CHECK_EQ_TYPE(node2.getID(), 5, boost::uint64_t);
        CHECK_EQ(node2.getLat(), 5.0);
        CHECK_EQ(node2.getLon(), 7.0);
        
        node2.setID(7);
        CHECK_EQ_TYPE(node2.getID(), 7, boost::uint64_t);
        
        node2.addProperty(OSMProperty("key", "value"));
        CHECK_EQ(node2.getProperties()[0], OSMProperty("key", "value"));
        
        return EXIT_SUCCESS;
    }
} 
