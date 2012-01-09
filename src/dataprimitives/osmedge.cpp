#include "osmedge.hpp"

bool operator==(const OSMEdge& e1, const OSMEdge& e2)
{
    if ((e1.getID() == e2.getID()) && (e1.getStartNode() == e2.getStartNode()) &&
        (e1.getEndNode() == e2.getEndNode()) && (e1.getProperties().size() == e2.getProperties().size()))
    {
        QVector<OSMProperty> props1 = e1.getProperties();
        QVector<OSMProperty> props2 = e2.getProperties();
        for (QVector<OSMProperty>::const_iterator it = props1.constBegin(); it != props1.constEnd(); it++)
        {
            if (!props2.contains(*it))
                return false;
        }
        return true;
    }
    else
        return false;
    
    return false;
}
std::ostream& operator<<(std::ostream& os, const OSMEdge& e)
{
    os << "wayid: " << e.getID() << " startnode: " << e.getStartNode() << 
        " endnode: " << e.getEndNode();
    QVector<OSMProperty> props = e.getProperties();
    for (QVector<OSMProperty>::const_iterator it = props.constBegin(); it != props.constEnd(); it++)
    {
        os << " " << *it;
    }
    return os;
}



namespace biker_tests
{
    int testOSMEdge()
    {
        OSMEdge edge(0);
        
        CHECK_EQ_TYPE(edge.getID(), 0, boost::uint64_t);
        
        return EXIT_SUCCESS;
    }
} 
