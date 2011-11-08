#include "closedlist.hpp" 

void HashClosedList::addNode(boost::uint64_t nodeID)
{
	hashtable << nodeID;
}
void HashClosedList::removeNode(boost::uint64_t nodeID)
{
	hashtable.remove(nodeID);
}
bool HashClosedList::contains(boost::uint64_t nodeID)
{
	return hashtable.contains(nodeID);
}
int HashClosedList::size()
{
	return hashtable.size();
}
