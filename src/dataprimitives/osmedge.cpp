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
        OSMEdge edge1(0);
        CHECK_EQ_TYPE(edge1.getID(), 0, boost::uint64_t);
        
        OSMProperty prop1("key1", "val1");
        OSMProperty prop2("key2", "val2");
        QVector<OSMProperty> propList;
        propList << prop1 << prop2;
		OSMEdge edge2(0, 1, 7,propList);		
		
        CHECK_EQ_TYPE(edge2.getID(), 0, boost::uint64_t);
        CHECK_EQ_TYPE(edge2.getStartNode(), 1, boost::uint64_t);
        CHECK_EQ_TYPE(edge2.getEndNode(), 7, boost::uint64_t);
        CHECK_EQ(edge2.getProperties()[0], prop1);
        CHECK_EQ(edge2.getProperties()[1], prop2);
        
        
        CHECK_EQ_TYPE(edge2.getNodes()[0], 1, boost::uint64_t);
        CHECK_EQ_TYPE(edge2.getNodes()[1], 7, boost::uint64_t);
 
		edge2.setID(4);
		edge2.setNodes(3,4);
		
        OSMProperty prop3("key3", "val3");
		edge2.addProperty(prop3);
		
		propList << prop3;
		
		
		CHECK_EQ_TYPE(edge2.getID(), 4, boost::uint64_t);
        CHECK_EQ_TYPE(edge2.getStartNode(), 3, boost::uint64_t);
        CHECK_EQ_TYPE(edge2.getEndNode(), 4, boost::uint64_t);
        CHECK_EQ(edge2.getProperties()[2], prop3);
		

        
        
        return EXIT_SUCCESS;
    }
} 
