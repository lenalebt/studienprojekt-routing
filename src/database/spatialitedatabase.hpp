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
    sqlite3_stmt* _saveNodeStatement;
    sqlite3_stmt* _getNodeStatement;
    sqlite3_stmt* _saveEdgeStatement;
    sqlite3_stmt* _getEdgeStatement;
    
    bool createTables();
    bool execCreateTableStatement(std::string);
public:
    SpatialiteDatabaseConnection();
    void close();
    void open(QString dbConnectionString);
    bool isDBOpen();
    QVector<boost::shared_ptr<RoutingNode> > getNodes(const GPSPosition& searchMidpoint, double radius);
    QVector<boost::shared_ptr<RoutingNode> > getNodes(const GPSPosition& ulCorner, const GPSPosition& brCorner);
    bool saveNode(const RoutingNode& node);
    QVector<boost::shared_ptr<RoutingEdge> > getEdgesByStartNodeID(boost::uint64_t startNodeID);
    QVector<boost::shared_ptr<RoutingEdge> > getEdgesByEndNodeID(boost::uint64_t endNodeID);
    boost::shared_ptr<RoutingEdge> getEdgesByEdgeID(boost::uint64_t edgeID);
    bool saveEdge(const RoutingEdge& edge);
    bool saveEdge(const RoutingEdge& edge, QString name);
    QString getStreetName(const RoutingEdge& edge);
	~SpatialiteDatabaseConnection();
};

namespace biker_tests
{
    int testSpatialiteDatabaseConnection();
}

#endif //SPATIALITEDATABASE_HPP
