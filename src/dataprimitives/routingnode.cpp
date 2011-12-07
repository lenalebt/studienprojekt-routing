#include "routingnode.hpp"

boost::uint64_t RoutingNode::convertIDToLongFormat(const boost::uint64_t id)
{
    
}

boost::uint64_t RoutingNode::convertIDToShortFormat(const boost::uint64_t id)
{
    
}

namespace biker_tests
{
    int testRoutingNode()
    {
        RoutingNode node1(1), node2(2);
        CHECK_EQ(node1.getID(), 1u);
        
        node1.setAndConvertID(1);
        CHECK_EQ(node1.getID(), 256u)
        
        node1.setAndConvertID(2);
        CHECK_EQ(node1.getID(), 512u)
        
        node1.setAndConvertID(3);
        CHECK_EQ(node1.getID(), 768u)
        
        node1.setAndConvertID(4316256737ll);
        CHECK_EQ(node1.getID(), 1104961724672ull)
        
        return EXIT_SUCCESS;
    }
}
