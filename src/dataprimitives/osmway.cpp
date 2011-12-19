#include "osmway.hpp"
#include "QVectorIterator"

QVector<OSMEdge> OSMWay::getEdgeList(){
    QVector<OSMEdge> edgeList;
    OSMEdge newEdge(id, properties);    
    
    if (!memberIDList.isEmpty()){
        QVectorIterator<boost::uint64_t> i(memberIDList);
        i.toFront();
        i.next();
        while (i.hasNext()){
            newEdge.setNodes(i.peekPrevious(), i.next());
            edgeList << newEdge;
        } 
    }
    
    return edgeList;      
}

namespace biker_tests
{
    int testOSMWay()
    {
        OSMWay way(0);
        
        CHECK_EQ_TYPE(way.getID(), 0, boost::uint64_t);
        way.setID(1);
        CHECK_EQ_TYPE(way.getID(), 1, boost::uint64_t);
        
        return EXIT_SUCCESS;
    }
} 
