#include "routingmetric.hpp" 

RoutingMetric::~RoutingMetric()
{
    
}


double EuclidianRoutingMetric::rateEdge(const RoutingEdge &edge, const RoutingNode &startNode, const RoutingNode &endNode)
{
    return startNode.calcDistance(endNode);
}


double EuclidianRoutingMetric::timeEdge(const RoutingEdge &edge, const RoutingNode &startNode, const RoutingNode &endNode)
{
    //Rechne mit 5m/s Fahrgeschwindigkeit
    return this->rateEdge(edge, startNode, endNode) / 5;
}

