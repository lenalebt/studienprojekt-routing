#include "databaseramcache.hpp"
#include "spatialitedatabase.hpp"
#include <QFile>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>

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

bool DatabaseRAMCache::beginTransaction()
{
    return _connection->beginTransaction();
}
bool DatabaseRAMCache::endTransaction()
{
    return _connection->endTransaction();
}

namespace biker_tests
{
    int testDatabaseRAMCache()
    {
        boost::shared_ptr<DatabaseConnection> connection(new SpatialiteDatabaseConnection());
        DatabaseRAMCache cache(connection);
        QFile file("cache.db");
        
        std::cerr << "Removing database test file \"cache.db\"..." << std::endl;
        if (file.exists())
            file.remove();
        
        std::cerr << "Opening \"cache.db\"..." << std::endl;
        cache.open("cache.db");
        CHECK(cache.isDBOpen());
        
        std::cerr << "Closing database..." << std::endl;
        cache.close();
        CHECK(!cache.isDBOpen());
        
        std::cerr << "Reopening \"cache.db\"..." << std::endl;
        cache.open("cache.db");
        CHECK(cache.isDBOpen());
        
        RoutingNode node(25, 51.0, 7.0);
        std::cerr << "Save Node..." << std::endl;
        CHECK(cache.saveNode(node));
        node = RoutingNode(26, 51.5, 7.5);
        CHECK(cache.saveNode(node));
        
        RoutingEdge edge(45, 25, 26);
        std::cerr << "Save Edge..." << std::endl;
        edge.setCycleBarrier(true);
        edge.setCyclewayType(5);
        CHECK(cache.saveEdge(edge));
        edge = RoutingEdge(46, 26, 25);
        CHECK(cache.saveEdge(edge));
        
        GPSPosition min(50.0, 6.0);
        GPSPosition max(52.0, 8.0);
        QVector<boost::shared_ptr<RoutingNode> > list = cache.getNodes(min, max);
        CHECK(!list.isEmpty());
        CHECK(list.size() == 2);
        
        std::cerr << "Node 0 from DB: " << *(list[0]) << std::endl;
        std::cerr << "Node 1 from DB: " << *(list[1]) << std::endl;
        CHECK((*list[0] == node) || (*list[1] == node));
        
        boost::minstd_rand generator(42u);
        boost::uniform_real<> uni_dist(50, 52);
        boost::variate_generator<boost::minstd_rand&, boost::uniform_real<> > uni(generator, uni_dist);
        
        std::cerr << "Inserting 10000 Nodes..." << std::endl;
        bool successInsertManyNodes = true;
        CHECK(cache.beginTransaction());
        for (int i=0; i<10000; i++)
        {
            node = RoutingNode(i + 100, uni(), uni() - (51.0 - 7.0));
            successInsertManyNodes = successInsertManyNodes && cache.saveNode(node);
        }
        CHECK(successInsertManyNodes);
        CHECK(cache.endTransaction());
        std::cerr << "Hier erwartet: Resultcode 19 (-> Constraint failed)" << std::endl;
        CHECK(!cache.saveNode(node));
        
        boost::shared_ptr<RoutingEdge> dbEdge(cache.getEdgeByEdgeID(46));
        CHECK_EQ(edge, *dbEdge);
        
        QVector<boost::shared_ptr<RoutingEdge> > edgeList;
        edgeList = cache.getEdgesByStartNodeID(26);
        CHECK_EQ(edge, *edgeList[0]);
        edgeList = cache.getEdgesByStartNodeID(26);
        CHECK_EQ(edge, *edgeList[0]);
        
        edgeList = cache.getEdgesByEndNodeID(25);
        CHECK_EQ(edge, *edgeList[0]);
        edgeList = cache.getEdgesByEndNodeID(25);
        CHECK_EQ(edge, *edgeList[0]);
        
        
        std::cerr << "Inserting 10000 Edges..." << std::endl;
        bool successInsertManyEdges = true;
        CHECK(cache.beginTransaction());
        for (int i=0; i<10000; i++)
        {
            edge = RoutingEdge(i + 100, i+99, i+100);
            successInsertManyEdges = successInsertManyEdges && cache.saveEdge(edge);
        }
        CHECK(successInsertManyEdges);
        CHECK(cache.endTransaction());
        std::cerr << "Hier erwartet: Resultcode 19 (-> Constraint failed)" << std::endl;
        CHECK(!cache.saveEdge(edge));
        
        
        edgeList = cache.getEdgesByStartNodeID(99);
        CHECK(!edgeList.isEmpty());
        edgeList = cache.getEdgesByStartNodeID(100);
        CHECK(!edgeList.isEmpty());
        
        CHECK(cache.beginTransaction());
        CHECK(cache.deleteEdge(99, 100));
        CHECK(cache.deleteEdge(100, 101));
        CHECK(cache.endTransaction());
        
        edgeList = cache.getEdgesByStartNodeID(99);
        CHECK(edgeList.isEmpty());
        edgeList = cache.getEdgesByStartNodeID(100);
        CHECK(edgeList.isEmpty());
        
        return EXIT_SUCCESS;
    }
}
