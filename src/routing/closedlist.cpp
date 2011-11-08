#include "closedlist.hpp" 

bool HashClosedList::contains(boost::uint64_t nodeID, RoutingThread thread) const
{
    QReadLocker locker(&_lock);
    switch (thread)
    {
        case S_THREAD:
            return _sSet.contains(nodeID);
            break;
        case T_THREAD:
            return _tSet.contains(nodeID);
            break;
        default:
            return false;
    }
    return false;
}


void HashClosedList::addNode(boost::uint64_t nodeID, RoutingThread thread)
{
    QWriteLocker locker(&_lock);
    switch (thread)
    {
        case S_THREAD:
            _sSet << nodeID;
            if (_tSet.contains(nodeID))
                _overlappingNode = nodeID;
            break;
        case T_THREAD:
            _tSet << nodeID;
            if (_sSet.contains(nodeID))
                _overlappingNode = nodeID;
            break;
        default:
            ;
    }
}


void HashClosedList::removeNode(boost::uint64_t nodeID, RoutingThread thread)
{
    QWriteLocker locker(&_lock);
    switch (thread)
    {
        case S_THREAD:
            _sSet.remove(nodeID);
            break;
        case T_THREAD:
            _tSet.remove(nodeID);
            break;
        default:
            ;
    }
}


int HashClosedList::size(RoutingThread thread) const
{
    QReadLocker locker(&_lock);
    switch (thread)
    {
        case S_THREAD:
            return _sSet.size();
            break;
        case T_THREAD:
            return _tSet.size();
            break;
        default:
            return 0;
    }
    return 0;
}


boost::uint64_t HashClosedList::getOverlappingNode() const
{
    return _overlappingNode;
}


