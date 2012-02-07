#include "routingnode.hpp"

std::ostream& operator<<(std::ostream& os, const RoutingNode& node)
{
    os << "RoutingNode, id:" << node.getID() << ", lat:" << node.getLat()
        << ", lon:" << node.getLon();
    return os;
}

namespace biker_tests
{
    int testRoutingNode()
    {
        RoutingNode node1(1), node2(2);
        CHECK_EQ_TYPE(node1.getID(), 1u, boost::uint64_t);
        
        node1.setAndConvertID(1);
        CHECK_EQ_TYPE(node1.getID(), 4611686018427388160ull, boost::uint64_t)
        
        node1.setAndConvertID(2);
        CHECK_EQ_TYPE(node1.getID(), 4611686018427388416ull, boost::uint64_t)
        
        node1.setAndConvertID(3);
        CHECK_EQ_TYPE(node1.getID(), 4611686018427388672ull, boost::uint64_t)
        
        node1.setAndConvertID(4316256737ll);
        CHECK_EQ_TYPE(node1.getID(), 4611687123389112576ull, boost::uint64_t)
        
        CHECK_EQ_TYPE(node1.convertIDToLongFormat(4316256737ll), 4611687123389112576ull, boost::uint64_t);
        CHECK_EQ_TYPE(node1.convertIDToShortFormat(4611687123389112576ull), 4316256737ll, boost::uint64_t);
        CHECK_EQ_TYPE(node1.convertIDToShortFormat(768), 768u, boost::uint64_t);
        CHECK_EQ_TYPE(node1.convertIDToShortFormat(512), 512u, boost::uint64_t);
        CHECK_EQ_TYPE(node1.convertIDToShortFormat(256), 256u, boost::uint64_t);
        
        CHECK_EQ_TYPE(RoutingNode::convertIDToShortFormat(RoutingNode::convertIDToLongFormat(26)), 26u, boost::uint64_t);
        
        return EXIT_SUCCESS;
    }
}
