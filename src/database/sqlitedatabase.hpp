#ifndef SQLITEDATABASE_HPP
#define SQLITEDATABASE_HPP

#include "database.hpp"
#include <sqlite3.h>

/**
 * @brief Implementierung einer DatabaseConnection mit einer
 *  SQLite3-Datenbank.
 * 
 * Diese Datenbank ist für die Systeme vorgesehen, auf denen
 * Spatialite nicht verfügbar ist.
 * 
 * @author Lena Brüder
 * @date 2011-12-23
 * @copyright GNU GPL v3
 * @ingroup database
 * @todo Umbauen von Spatialite auf SQLite.
 * @todo Z-Kurve einbauen
 */
class SQLiteDatabaseConnection : public DatabaseConnection
{
private:
    bool _dbOpen;
    sqlite3* _db;
    sqlite3_stmt* _saveNodeStatement;
    sqlite3_stmt* _getNodeStatement;
    sqlite3_stmt* _getNodeByIDStatement;
    sqlite3_stmt* _saveEdgeStatement;
    sqlite3_stmt* _getEdgeStatementID;
    sqlite3_stmt* _getEdgeStatementStartNode;
    sqlite3_stmt* _getEdgeStatementEndNode;
    sqlite3_stmt* _deleteEdgeStatement;
    
    /**
     * @brief Erstellt die Tabellen in der Datenbank
     * @return Ob die Tabellen erstellt werden konnten
     */
    bool createTables();
    
    /**
     * @brief Führt die Statements aus, die die Tabellen erstellen sollen
     * @return Ob die Ausführung erfolgreich war, oder nicht
     */
    bool execCreateTableStatement(std::string);
public:
    SQLiteDatabaseConnection();
    void close();
    void open(QString dbConnectionString);
    bool isDBOpen();
    boost::shared_ptr<RoutingNode> getNodeByID(boost::uint64_t id);
    /** @todo Implementieren! */
    QVector<boost::shared_ptr<RoutingNode> > getNodes(const GPSPosition& searchMidpoint, double radius);
    QVector<boost::shared_ptr<RoutingNode> > getNodes(const GPSPosition& ulCorner, const GPSPosition& brCorner);
    bool saveNode(const RoutingNode& node);
    QVector<boost::shared_ptr<RoutingEdge> > getEdgesByStartNodeID(boost::uint64_t startNodeID);
    QVector<boost::shared_ptr<RoutingEdge> > getEdgesByEndNodeID(boost::uint64_t endNodeID);
    boost::shared_ptr<RoutingEdge> getEdgeByEdgeID(boost::uint64_t edgeID);
    bool saveEdge(const RoutingEdge& edge);
    /** @todo Speichern von Straßen implementieren */
    bool saveEdge(const RoutingEdge& edge, const QString& name);
    bool deleteEdge(boost::uint64_t startNodeID, boost::uint64_t endNodeID);
    /** @todo implementieren */
    QString getStreetName(const RoutingEdge& edge);
	~SQLiteDatabaseConnection();
    
    bool beginTransaction();
    bool endTransaction();
    
    bool createIndexes();
};

namespace biker_tests
{
    /**
     * @ingroup tests
     */
    int testSQLiteDatabaseConnection();
}

#endif //SQLITEDATABASE_HPP
