#ifndef RAMDATABASE_HPP
#define RAMDATABASE_HPP

#include "database.hpp"
#include "spacefillingcurves.hpp"

/**
 * @brief Implementierung einer DatabaseConnection 
 * ausschließlich im RAM.
 * 
 * 
 * 
 * @author Lena Brüder
 * @date 2012-03-12
 * @copyright GNU AGPL
 * @ingroup database
 */
class RAMDatabaseConnection : public DatabaseConnection
{
private:
    bool _dbOpen;
    QString _dbFilename;
public:
    RAMDatabaseConnection();
    void close();
    void open(QString dbConnectionString);
    bool isDBOpen();
    boost::shared_ptr<RoutingNode> getNodeByID(boost::uint64_t id);
    QVector<boost::shared_ptr<RoutingNode> > getNodes(const GPSPosition& searchMidpoint, double radius);
    QVector<boost::shared_ptr<RoutingNode> > getNodes(const GPSPosition& ulCorner, const GPSPosition& brCorner);
    bool saveNode(const RoutingNode& node);
    QVector<boost::shared_ptr<RoutingEdge> > getEdgesByStartNodeID(boost::uint64_t startNodeID);
    QVector<boost::shared_ptr<RoutingEdge> > getEdgesByEndNodeID(boost::uint64_t endNodeID);
    boost::shared_ptr<RoutingEdge> getEdgeByEdgeID(boost::uint64_t edgeID);
    bool saveEdge(const RoutingEdge& edge);
    bool saveEdge(const RoutingEdge& edge, const QString& name);
    bool deleteEdge(boost::uint64_t startNodeID, boost::uint64_t endNodeID);
    QString getStreetName(const RoutingEdge& edge);
	~SpatialiteDatabaseConnection();
    
    bool beginTransaction();
    bool endTransaction();
    
    bool createIndexes();
};

namespace biker_tests
{
    /**
     * @ingroup tests
     */
    int testRAMDatabaseConnection();
}

#endif  //RAMDATABASE_HPP
