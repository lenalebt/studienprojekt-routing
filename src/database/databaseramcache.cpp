#include "databaseramcache.hpp"
#include "spatialitedatabase.hpp"

DatabaseRAMCache::DatabaseRAMCache(boost::shared_ptr<DatabaseConnection> connection) :
    _connection(connection)
{
    
}

DatabaseRAMCache::~DatabaseRAMCache()
{
    
}

void DatabaseRAMCache::close()
{
    _connection->close();
}


void DatabaseRAMCache::open(QString dbConnectionString)
{
    _connection->open(dbConnectionString);
}


bool DatabaseRAMCache::isDBOpen()
{
    return _connection->isDBOpen();
}


QVector<boost::shared_ptr<RoutingNode> > DatabaseRAMCache::getNodes(const GPSPosition &searchMidpoint, double radius)
{
    return _connection->getNodes(searchMidpoint, radius);
}


QVector<boost::shared_ptr<RoutingNode> > DatabaseRAMCache::getNodes(const GPSPosition &ulCorner, const GPSPosition &brCorner)
{
    return _connection->getNodes(ulCorner, brCorner);
}


bool DatabaseRAMCache::saveNode(const RoutingNode &node)
{
    return _connection->saveNode(node);
}


QVector<boost::shared_ptr<RoutingEdge> > DatabaseRAMCache::getEdgesByStartNodeID(boost::uint64_t startNodeID)
{
    return _connection->getEdgesByStartNodeID(startNodeID);
}


QVector<boost::shared_ptr<RoutingEdge> > DatabaseRAMCache::getEdgesByEndNodeID(boost::uint64_t endNodeID)
{
    return _connection->getEdgesByEndNodeID(endNodeID);
}


boost::shared_ptr<RoutingEdge> DatabaseRAMCache::getEdgeByEdgeID(boost::uint64_t edgeID)
{
    return _connection->getEdgeByEdgeID(edgeID);
}


bool DatabaseRAMCache::saveEdge(const RoutingEdge &edge)
{
    return _connection->saveEdge(edge);
}


bool DatabaseRAMCache::saveEdge(const RoutingEdge &edge, QString name)
{
    return _connection->saveEdge(edge, name);
}


QString DatabaseRAMCache::getStreetName(const RoutingEdge &edge)
{
    return _connection->getStreetName(edge);
}


bool DatabaseRAMCache::deleteEdge(boost::uint64_t startNodeID, boost::uint64_t endNodeID)
{
    return _connection->deleteEdge(startNodeID, endNodeID);
}




namespace biker_tests
{
    int testDatabaseRAMCache()
    {
        boost::shared_ptr<DatabaseConnection> connection(new SpatialiteDatabaseConnection());
        DatabaseRAMCache cache(connection);
        return EXIT_FAILURE;
    }
}
