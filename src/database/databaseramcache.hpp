#ifndef DATABASERAMCACHE_HPP
#define DATABASERAMCACHE_HPP

#include "database.hpp"
#include "tests.hpp"
#include <boost/shared_ptr.hpp>
#include <QCache>
#include <QMutex>

/**
 * @brief Implementiert einen RAM-Cache für Datenbankelemente.
 * 
 * Wenn ein Element bereits im Speicher ist, wird es von dort geholt,
 * statt es aus der Datenbank zu holen. In allen anderen Fällen wird es
 * aus der Datenbank geladen.
 * 
 * @author Lena Brüder
 * @date 2011-12-23
 * @copyright GNU GPL v3
 * @ingroup database
 * @todo Cacht bisher noch nicht den Zugriff auf Knoten über eine Gebietsangabe, und keine Straßennamen (muss er das überhaupt?)
 */
class DatabaseRAMCache : public DatabaseConnection
{
private:
    static QMutex _startNodeEdgeCacheMutex;
    static QCache<boost::uint64_t, QVector<boost::shared_ptr<RoutingEdge> > > _startNodeEdgeCache;
    
    static QMutex _endNodeEdgeCacheMutex;
    static QCache<boost::uint64_t, QVector<boost::shared_ptr<RoutingEdge> > > _endNodeEdgeCache;
    
    static QMutex _idEdgeCacheMutex;
    static QCache<boost::uint64_t, boost::shared_ptr<RoutingEdge> > _idEdgeCache;
    
    static QMutex _positionNodeCacheMutex;
    static QCache<boost::uint64_t, QVector<boost::shared_ptr<RoutingNode> > > _positionNodeCache;
    
    static QMutex _idNodeCacheMutex;
    static QCache<boost::uint64_t, boost::shared_ptr<RoutingNode> > _idNodeCache;
    
    
    boost::shared_ptr<DatabaseConnection> _connection;
    
    void clearEdgeCaches();
    void clearNodeCaches();
public:
    DatabaseRAMCache(boost::shared_ptr<DatabaseConnection> connection, int cacheSize);
    ~DatabaseRAMCache();
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
    QString getStreetName(const RoutingEdge& edge);
    bool deleteEdge(boost::uint64_t startNodeID, boost::uint64_t endNodeID);
    bool beginTransaction();
    bool endTransaction();
    bool createIndexes();
};

namespace biker_tests
{
    /**
     * @ingroup tests
     */
    int testDatabaseRAMCache();
}
#endif //DATABASERAMCACHE_HPP
