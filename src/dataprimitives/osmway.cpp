#include "osmway.hpp"
#include "QVectorIterator"

/**
 * @bug Fügt nur die Edges in eine Richtung ein und achtet nicht
 *  auf Einbahnstraßen (und deren Richtung! Kann umgekehrt sein!)
 */
QVector<OSMEdge> OSMWay::getEdgeList(){
    QVector<OSMEdge> edgeList;
    OSMEdge newEdge(id, properties);    
    
    if (!memberIDList.isEmpty()){
        QVectorIterator<boost::uint64_t> i(memberIDList);
        i.toFront();  // Iterator springt vor den ersten Eintrag in memberIDList
        i.next();     // Iterator geht einen Schirtt weiter (gibt auch Wert zurück, der hier nicht verwendet wird)
        while (i.hasNext()){
            newEdge.setNodes(i.peekPrevious(), i.next());
            edgeList << newEdge;
        } 
    }
    
    return edgeList;      
}

/* Es folgt: Kram, der nicht unbedingt toll und ganz logisch ist,
 * aber am Ende halbwegs richtige Ergebnisse produzieren sollte.
 * Bis zum markierten Ende.
 */
QVector<OSMProperty> OSMWay::onewayProperties()
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
QVector<OSMProperty> OSMWay::noOnewayProperties()
{
    static QVector<OSMProperty> retVec;
    if (retVec.isEmpty())
    {
        retVec << OSMProperty("oneway", "no");
    }
    return retVec;
}
QVector<OSMProperty> OSMWay::reverseOnewayProperties()
{
    static QVector<OSMProperty> retVec;
    if (retVec.isEmpty())
    {
        retVec << OSMProperty("oneway", "-1");
        retVec << OSMProperty("oneway", "reverse");
    }
    return retVec;
}
QVector<OSMProperty> OSMWay::noBikeOnewayProperties()
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
QVector<OSMProperty> OSMWay::bikeOnewayProperties()
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
QVector<OSMProperty> OSMWay::bikeReverseOnewayProperties()
{
    static QVector<OSMProperty> retVec;
    if (retVec.isEmpty())
    {
        retVec << OSMProperty("oneway:bicycle", "-1");
        retVec << OSMProperty("oneway:bicycle", "reverse");
    }
    return retVec;
}

int OSMWay::isOneway()
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

int OSMWay::isOneWayForBikes()
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
    /**
     * @todo Dieser Test ist viel (!) zu kurz.
     */
    int testOSMWay()
    {
        OSMWay way(25);
        CHECK_EQ_TYPE(way.getID(), 25, boost::uint64_t);
        way.addMember(1);
        way.addMember(2);
        way.addMember(3);
        way.addMember(4);
        QVector<boost::uint64_t> wayMemberList = way.getMemberList();
        CHECK(!wayMemberList.isEmpty());
        CHECK_EQ(wayMemberList.size(), 4);
        CHECK_EQ_TYPE(wayMemberList[0], 1, boost::uint64_t);
        CHECK_EQ_TYPE(wayMemberList[1], 2, boost::uint64_t);
        CHECK_EQ_TYPE(wayMemberList[2], 3, boost::uint64_t);
        CHECK_EQ_TYPE(wayMemberList[3], 4, boost::uint64_t);
        
        way.addProperty(OSMProperty("key", "value"));
        way.addProperty(OSMProperty("key2", "value2"));
        CHECK_EQ(way.getProperties()[0], OSMProperty("key", "value"));
        CHECK_EQ(way.getProperties()[1], OSMProperty("key2", "value2"));
        
        //TODO: Test für OSMEdge erweitern
        QVector<OSMEdge> edgeList = way.getEdgeList();
        CHECK_EQ(edgeList.size(), 6);
        CHECK_EQ_TYPE(edgeList[0].getID(), 25, boost::uint64_t);
        CHECK_EQ_TYPE(edgeList[1].getID(), 25, boost::uint64_t);
        CHECK_EQ_TYPE(edgeList[2].getID(), 25, boost::uint64_t);
        CHECK(edgeList[0].getProperties().contains(way.getProperties()[0]));
        CHECK(edgeList[0].getProperties().contains(way.getProperties()[1]));
        
        return EXIT_SUCCESS;
    }
} 
