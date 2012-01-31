#include "routingnode.hpp"

bool RoutingNode::isIDInLongFormat()
{
    boost::uint64_t mark = 0x4000000000000000llu;
    return (id & mark);
}

bool RoutingNode::isIDInLongFormat(const boost::uint64_t id)
{
    boost::uint64_t mark = 0x4000000000000000llu;
    return (id & mark);
}

boost::uint64_t RoutingNode::convertIDToLongFormat(const boost::uint64_t id)
{
    if (RoutingNode::isIDInLongFormat(id))
        return id;
    boost::uint64_t mask = 0x00FFFFFFFFFFFFFFllu;
    boost::uint64_t mark = 0x4000000000000000llu;
    return ((id & mask) << 8) | mark;
}

boost::uint64_t RoutingNode::convertIDToShortFormat(const boost::uint64_t id)
{
    if (!RoutingNode::isIDInLongFormat(id))
        return id;
    boost::uint64_t mark = 0x4000000000000000llu;
    return ((id & ~mark) >> 8);
}

bool RoutingNode::operator==(const RoutingNode& other)
{
    return (this->id == other.id) &&
        (this->getLat() == other.getLat()) &&
        (this->getLon() == other.getLon());
}

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
