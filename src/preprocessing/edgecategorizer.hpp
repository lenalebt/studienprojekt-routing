#ifndef EDGECATEGORIZER_HPP
#define EDGECATEGORIZER_HPP

#include <boost/shared_ptr.hpp>
#include "blockingqueue.hpp"
#include "osmedge.hpp"
#include "osmway.hpp"
#include "routingedge.hpp"
#include "routingnode.hpp"
#include <boost/logic/tribool.hpp>

class EdgeCategorizer
{
private:
    BlockingQueue<boost::shared_ptr<OSMEdge> >* _edgeQueue;
    BlockingQueue<boost::shared_ptr<OSMWay> >* _wayQueue;
    
    BlockingQueue<boost::shared_ptr<RoutingEdge> >* _outQueue;
    
    bool categorizeEdges();
    bool categorizeWays();
    
    void categorize(const QVector<OSMProperty> properties, boost::uint64_t& propForward, boost::uint64_t& propBackward);
public:
    EdgeCategorizer(BlockingQueue<boost::shared_ptr<OSMEdge> >* inQueue, BlockingQueue<boost::shared_ptr<RoutingEdge> >* outQueue);
    EdgeCategorizer(BlockingQueue<boost::shared_ptr<OSMWay> >* inQueue, BlockingQueue<boost::shared_ptr<RoutingEdge> >* outQueue);
    
    bool startCategorizerLoop();
};

#endif //EDGECATEGORIZER_HPP
