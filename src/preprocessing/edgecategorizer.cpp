#include "edgecategorizer.hpp"

EdgeCategorizer::EdgeCategorizer(BlockingQueue<boost::shared_ptr<OSMEdge> >* inQueue, BlockingQueue<boost::shared_ptr<RoutingEdge> >* outQueue) :
    _edgeQueue(inQueue), _outQueue(outQueue)
{
    
}
EdgeCategorizer::EdgeCategorizer(BlockingQueue<boost::shared_ptr<OSMWay> >* inQueue, BlockingQueue<boost::shared_ptr<RoutingEdge> >* outQueue) :
    _wayQueue(inQueue), _outQueue(outQueue)
{
    
}

boost::shared_ptr<RoutingEdge> EdgeCategorizer::categorizeEdge(const OSMEdge &osmEdge)
{
    return boost::shared_ptr<RoutingEdge>();
}

bool EdgeCategorizer::startCategorizerLoop()
{
    if (_edgeQueue != 0)
        return categorizeEdges();
    else if (_wayQueue != 0)
        return categorizeWays();
    else
        return false;
}

bool EdgeCategorizer::categorizeEdges()
{
    boost::shared_ptr<OSMEdge> osmEdge;
    while (_edgeQueue->dequeue(osmEdge))
    {
        _outQueue->enqueue(categorizeEdge(*osmEdge));
    }
    _outQueue->destroyQueue();
    return false;
}
bool EdgeCategorizer::categorizeWays()
{
    boost::shared_ptr<OSMWay> osmWay;
    while (_wayQueue->dequeue(osmWay))
    {
        QVector<OSMEdge> edgeList = osmWay->getEdgeList();
        for(int i = 0; i < edgeList.size(); i++)
        {
            _outQueue->enqueue(categorizeEdge(edgeList[i]));
        }
    }
    _outQueue->destroyQueue();
    return false;
}
