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
    return QVector<boost::shared_ptr<RoutingNode> >();
}


QVector<boost::shared_ptr<RoutingNode> > DatabaseRAMCache::getNodes(const GPSPosition &ulCorner, const GPSPosition &brCorner)
{
    return QVector<boost::shared_ptr<RoutingNode> >();
}


bool DatabaseRAMCache::saveNode(const RoutingNode &node)
{
    return false;
}


QVector<boost::shared_ptr<RoutingEdge> > DatabaseRAMCache::getEdgesByStartNodeID(boost::uint64_t startNodeID)
{
    return QVector<boost::shared_ptr<RoutingEdge> >();
}


QVector<boost::shared_ptr<RoutingEdge> > DatabaseRAMCache::getEdgesByEndNodeID(boost::uint64_t endNodeID)
{
    return QVector<boost::shared_ptr<RoutingEdge> >();
}


boost::shared_ptr<RoutingEdge> DatabaseRAMCache::getEdgeByEdgeID(boost::uint64_t edgeID)
{
    return boost::shared_ptr<RoutingEdge>();
}


bool DatabaseRAMCache::saveEdge(const RoutingEdge &edge)
{
    return false;
}


bool DatabaseRAMCache::saveEdge(const RoutingEdge &edge, QString name)
{
    return false;
}


QString DatabaseRAMCache::getStreetName(const RoutingEdge &edge)
{
    return "";
}


bool DatabaseRAMCache::deleteEdge(boost::uint64_t startNodeID, boost::uint64_t endNodeID)
{
    return false;
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
