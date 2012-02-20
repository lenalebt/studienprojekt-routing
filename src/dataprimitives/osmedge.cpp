#include "osmedge.hpp"

bool operator==(const OSMEdge& e1, const OSMEdge& e2)
{
    if ((e1.getID() == e2.getID()) && (e1.getForward() == e2.getForward()) &&(e1.getStartNode() == e2.getStartNode()) &&
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
    os << "wayid: " << e.getID() << " forward: " << e.getForward() << " startnode: " << e.getStartNode() << 
        " endnode: " << e.getEndNode();
    QVector<OSMProperty> props = e.getProperties();
    for (QVector<OSMProperty>::const_iterator it = props.constBegin(); it != props.constEnd(); it++)
    {
        os << " " << *it;
    }
    return os;
}

/* Es folgt: Kram, der nicht unbedingt toll und ganz logisch ist,
 * aber am Ende halbwegs richtige Ergebnisse produzieren sollte.
 * Bis zum markierten Ende.
 * Äquivalent zu OSMWay
 */
QVector<OSMProperty> OSMEdge::onewayProperties()
{
    static QVector<OSMProperty> retVec;
    if (retVec.isEmpty())
    {
        retVec << OSMProperty("oneway", "yes");
        retVec << OSMProperty("oneway", "true");
        retVec << OSMProperty("oneway", "1");
        retVec << OSMProperty("highway", "motorway");
        retVec << OSMProperty("highway", "motorway_link");
        retVec << OSMProperty("highway", "motorway_junction");
        retVec << OSMProperty("junction", "roundabout");
    }
    return retVec;

}
QVector<OSMProperty> OSMEdge::noOnewayProperties()
{
    static QVector<OSMProperty> retVec;
    if (retVec.isEmpty())
    {
        retVec << OSMProperty("oneway", "no");
    }
    return retVec;
}
QVector<OSMProperty> OSMEdge::reverseOnewayProperties()
{
    static QVector<OSMProperty> retVec;
    if (retVec.isEmpty())
    {
        retVec << OSMProperty("oneway", "-1");
        retVec << OSMProperty("oneway", "reverse");
    }
    return retVec;
}
QVector<OSMProperty> OSMEdge::noBikeOnewayProperties()
{
    static QVector<OSMProperty> retVec;
    if (retVec.isEmpty())
    {   //fehlt: bicycle:forward=yes/no und bicycle:backward=yes/no
        retVec << OSMProperty("oneway:bicycle", "no");
        retVec << OSMProperty("cycleway", "opposite");
        retVec << OSMProperty("cycleway", "opposite_lane");
        retVec << OSMProperty("cycleway", "opposite_track");
    }
    return retVec;
}
QVector<OSMProperty> OSMEdge::bikeOnewayProperties()
{
    static QVector<OSMProperty> retVec;
    if (retVec.isEmpty())
    {
        retVec << OSMProperty("oneway:bicycle", "yes");
        retVec << OSMProperty("oneway:bicycle", "true");
        retVec << OSMProperty("oneway:bicycle", "1");
    }
    return retVec;
}
QVector<OSMProperty> OSMEdge::bikeReverseOnewayProperties()
{
    static QVector<OSMProperty> retVec;
    if (retVec.isEmpty())
    {
        retVec << OSMProperty("oneway:bicycle", "-1");
        retVec << OSMProperty("oneway:bicycle", "reverse");
    }
    return retVec;
}

int OSMEdge::isOneway()
{
    for (QVector<OSMProperty>::const_iterator it = noOnewayProperties().constBegin();
        it != noOnewayProperties().constEnd(); it++)
    {
        if (properties.contains(*it))
            return 0;
    }
    for (QVector<OSMProperty>::const_iterator it = onewayProperties().constBegin();
        it != onewayProperties().constEnd(); it++)
    {
        if (properties.contains(*it))
            return 1;
    }
    for (QVector<OSMProperty>::const_iterator it = reverseOnewayProperties().constBegin();
        it != reverseOnewayProperties().constEnd(); it++)
    {
        if (properties.contains(*it))
            return -1;
    }
    return false;
}

int OSMEdge::isOneWayForBikes()
{
    int _isOneway = isOneway();
    if (_isOneway == 0)
    {
        //TODO: Ist das echt nötig, diesen Sonderfall zu betrachten?
        //Fahrräder dürfen durch die nicht-Einbahnstraße nur in eine Richtung
        for (QVector<OSMProperty>::const_iterator it = bikeOnewayProperties().constBegin();
            it != bikeOnewayProperties().constEnd(); it++)
        {
            if (properties.contains(*it))
                return 1;
        }
        //entgegengesetzt
        for (QVector<OSMProperty>::const_iterator it = bikeReverseOnewayProperties().constBegin();
            it != bikeReverseOnewayProperties().constEnd(); it++)
        {
            if (properties.contains(*it))
                return -1;
        }
        return 0;
    }
    else
    {
        for (QVector<OSMProperty>::const_iterator it = noBikeOnewayProperties().constBegin();
            it != noBikeOnewayProperties().constEnd(); it++)
        {
            if (properties.contains(*it))
                return 0;
        }
        return _isOneway;
    }
}

/*
 * Bis hier.
 */

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
		OSMEdge edge2(0, true, 1, 7,propList);		
		
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
