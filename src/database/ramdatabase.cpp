#include "ramdatabase.hpp"

RAMDatabaseConnection::RAMDatabaseConnection()
{
}


void RAMDatabaseConnection::close()
{
}


void RAMDatabaseConnection::open(QString dbConnectionString)
{
    _dbOpen = true;
    _dbFilename = dbConnectionString;
    //TODO: Parser anwerfen und so
}


bool RAMDatabaseConnection::isDBOpen()
{
    return _dbOpen;
}


boost::shared_ptr<RoutingNode> RAMDatabaseConnection::getNodeByID(boost::uint64_t id)
{
}


QVector<boost::shared_ptr<RoutingNode>> RAMDatabaseConnection::getNodes(const GPSPosition &searchMidpoint, double radius)
{
}


QVector<boost::shared_ptr<RoutingNode>> RAMDatabaseConnection::getNodes(const GPSPosition &ulCorner, const GPSPosition &brCorner)
{
}


bool RAMDatabaseConnection::saveNode(const RoutingNode &node)
{
}


QVector<boost::shared_ptr<RoutingEdge>> RAMDatabaseConnection::getEdgesByStartNodeID(boost::uint64_t startNodeID)
{
}


QVector<boost::shared_ptr<RoutingEdge>> RAMDatabaseConnection::getEdgesByEndNodeID(boost::uint64_t endNodeID)
{
}


boost::shared_ptr<RoutingEdge> RAMDatabaseConnection::getEdgeByEdgeID(boost::uint64_t edgeID)
{
}


bool RAMDatabaseConnection::saveEdge(const RoutingEdge &edge)
{
}


bool RAMDatabaseConnection::saveEdge(const RoutingEdge &edge, const QString &name)
{
}


bool RAMDatabaseConnection::deleteEdge(boost::uint64_t startNodeID, boost::uint64_t endNodeID)
{
}


QString RAMDatabaseConnection::getStreetName(const RoutingEdge &edge)
{
}


RAMDatabaseConnection::~SpatialiteDatabaseConnection()
{
}


bool RAMDatabaseConnection::beginTransaction()
{
}


bool RAMDatabaseConnection::endTransaction()
{
}


bool RAMDatabaseConnection::createIndexes()
{
}


int biker_tests::testRAMDatabaseConnection()
{
    return EXIT_FAILURE;
}


