#include "spatialitedatabase.hpp"

void SpatialiteDatabaseConnection::close()
{
    
}


void SpatialiteDatabaseConnection::open(QString dbConnectionString)
{
    
}


bool SpatialiteDatabaseConnection::isDBOpen()
{
    
}


QVector<boost::shared_ptr<RoutingNode>> SpatialiteDatabaseConnection::getNodes(const GPSPosition &searchMidpoint, double radius)
{
    
}


QVector<boost::shared_ptr<RoutingNode>>
SpatialiteDatabaseConnection::getNodes(const GPSPosition &ulCorner, const GPSPosition &brCorner)
{
    
}


void SpatialiteDatabaseConnection::saveNode(const RoutingNode &node)
{
    
}


QVector<boost::shared_ptr<RoutingEdge>>
SpatialiteDatabaseConnection::getEdgesByStartNodeID(boost::uint64_t startNodeID)
{
    
}


QVector<boost::shared_ptr<RoutingEdge>>
SpatialiteDatabaseConnection::getEdgesByEndNodeID(boost::uint64_t endNodeID)
{
    
}


boost::shared_ptr<RoutingEdge>
SpatialiteDatabaseConnection::getEdgesByEdgeID(boost::uint64_t edgeID)
{
    
}


void SpatialiteDatabaseConnection::saveEdge(const RoutingEdge &edge)
{
    
}


void SpatialiteDatabaseConnection::saveEdge(const RoutingEdge &edge, QString name)
{
    
}


QString SpatialiteDatabaseConnection::getStreetName(const RoutingEdge &edge)
{
    
}
