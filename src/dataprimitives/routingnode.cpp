#include "routingnode.hpp"

bool RoutingNode::isIDInLongFormat()
{
    boost::uint64_t mask = 0xFF00000000000000llu;
    return (id & mask);
}

boost::uint64_t RoutingNode::convertIDToLongFormat(const boost::uint64_t id)
{
    boost::uint64_t mask = 0xFFFFFFFFFFFFFFllu;
    return ((id & mask) << 8);
}

boost::uint64_t RoutingNode::convertIDToShortFormat(const boost::uint64_t id)
{
    return (id >> 8);
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
