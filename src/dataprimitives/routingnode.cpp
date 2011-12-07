#include "routingnode.hpp"

boost::uint64_t RoutingNode::convertIDToLongFormat(const boost::uint64_t id)
{
    boost::uint64_t mask = 0xFFFFFFFFFFFFFFllu;
    return ((id & mask) << 8);
}

boost::uint64_t RoutingNode::convertIDToShortFormat(const boost::uint64_t id)
{
    return (id >> 8);
}

namespace biker_tests
{
    int testRoutingNode()
    {
        RoutingNode node1(1), node2(2);
        CHECK_EQ_TYPE(node1.getID(), 1u, boost::uint64_t);
        
        node1.setAndConvertID(1);
        CHECK_EQ_TYPE(node1.getID(), 256u, boost::uint64_t)
        
        node1.setAndConvertID(2);
        CHECK_EQ_TYPE(node1.getID(), 512u, boost::uint64_t)
        
        node1.setAndConvertID(3);
        CHECK_EQ_TYPE(node1.getID(), 768u, boost::uint64_t)
        
        node1.setAndConvertID(4316256737ll);
        CHECK_EQ_TYPE(node1.getID(), 1104961724672ull, boost::uint64_t)
        
        CHECK_EQ_TYPE(node1.convertIDToLongFormat(4316256737ll), 1104961724672ull, boost::uint64_t);
        CHECK_EQ_TYPE(node1.convertIDToShortFormat(1104961724672ull), 4316256737ll, boost::uint64_t);
        CHECK_EQ_TYPE(node1.convertIDToShortFormat(768), 3u, boost::uint64_t);
        CHECK_EQ_TYPE(node1.convertIDToShortFormat(512), 2u, boost::uint64_t);
        CHECK_EQ_TYPE(node1.convertIDToShortFormat(256), 1u, boost::uint64_t);
        
        return EXIT_SUCCESS;
    }
}
