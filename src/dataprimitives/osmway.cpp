#include "osmway.hpp"
#include "QVectorIterator"

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

namespace biker_tests
{
    int testOSMWay()
    {
        OSMWay way(0);
        
        CHECK_EQ_TYPE(way.getID(), 0, boost::uint64_t);
        
        return EXIT_SUCCESS;
    }
} 
