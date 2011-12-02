#ifndef SPATIALITEDATABASE_HPP
#define SPATIALITEDATABASE_HPP

#include "database.hpp"
//TODO: sqlite als Voraussetzung? Auf welchen Systemen existiert <spatialite/sqlite3.h>,
//      und wo nur <sqlite3.h>?
#include <sqlite3.h>

class SpatialiteDatabaseConnection : public DatabaseConnection
{
private:
    bool _dbOpen;
    sqlite3* _db;
public:
    SpatialiteDatabaseConnection();
    void close();
    void open(QString dbConnectionString);
    bool isDBOpen();
    QVector<boost::shared_ptr<RoutingNode> > getNodes(const GPSPosition& searchMidpoint, double radius);
    QVector<boost::shared_ptr<RoutingNode> > getNodes(const GPSPosition& ulCorner, const GPSPosition& brCorner);
    void saveNode(const RoutingNode& node);
    QVector<boost::shared_ptr<RoutingEdge> > getEdgesByStartNodeID(boost::uint64_t startNodeID);
    QVector<boost::shared_ptr<RoutingEdge> > getEdgesByEndNodeID(boost::uint64_t endNodeID);
    boost::shared_ptr<RoutingEdge> getEdgesByEdgeID(boost::uint64_t edgeID);
    void saveEdge(const RoutingEdge& edge);
    void saveEdge(const RoutingEdge& edge, QString name);
    QString getStreetName(const RoutingEdge& edge);

};

namespace biker_tests
{
    int testSpatialiteDatabaseConnection();
}

#endif //SPATIALITEDATABASE_HPP
