#ifndef SPATIALITEDATABASE_HPP
#define SPATIALITEDATABASE_HPP

#include "database.hpp"
#include <sqlite3.h>

class SpatialiteDatabaseConnection : public DatabaseConnection
{
private:
    bool _dbOpen;
    sqlite3* _db;
    sqlite3_stmt* _saveNodeStatement;
    sqlite3_stmt* _getNodeStatement;
    sqlite3_stmt* _saveEdgeStatement;
    sqlite3_stmt* _getEdgeStatementID;
    sqlite3_stmt* _getEdgeStatementStartNode;
    sqlite3_stmt* _getEdgeStatementEndNode;
    
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
    SpatialiteDatabaseConnection();
    void close();
    void open(QString dbConnectionString);
    bool isDBOpen();
    QVector<boost::shared_ptr<RoutingNode> > getNodes(const GPSPosition& searchMidpoint, double radius);
    QVector<boost::shared_ptr<RoutingNode> > getNodes(const GPSPosition& ulCorner, const GPSPosition& brCorner);
    bool saveNode(const RoutingNode& node);
    QVector<boost::shared_ptr<RoutingEdge> > getEdgesByStartNodeID(boost::uint64_t startNodeID);
    QVector<boost::shared_ptr<RoutingEdge> > getEdgesByEndNodeID(boost::uint64_t endNodeID);
    boost::shared_ptr<RoutingEdge> getEdgeByEdgeID(boost::uint64_t edgeID);
    bool saveEdge(const RoutingEdge& edge);
    /** @todo Speichern von Straßen implementieren */
    bool saveEdge(const RoutingEdge& edge, QString name);
    /** @todo implementieren */
    QString getStreetName(const RoutingEdge& edge);
	~SpatialiteDatabaseConnection();
    
    /**
     * @brief Beginnt eine neue Transaktion auf der Datenbank.
     * 
     * Wenn große Datenmengen in der Datenbank abgelegt werden
     * sollen ist sie wesentlich schneller, wenn man viele Daten
     * in einer Transaktion ablegt. Daher sollten Transaktionen
     * immer verwendet werden, wenn mehr als nur zwei oder drei
     * Datensätze in der Datenbank abgelegt werden.
     * 
     * @return Ob das Starten einer Transaktion erfolgreich war,
     *  oder nicht
     * @todo Auch für eine normale DatabaseConnection ermöglichen?
     */
    bool beginTransaction();
    
    /**
     * @brief Beendet die letzte Transaktion auf der Datenbank.
     * 
     * Wenn große Datenmengen in der Datenbank abgelegt werden
     * sollen ist sie wesentlich schneller, wenn man viele Daten
     * in einer Transaktion ablegt. Daher sollten Transaktionen
     * immer verwendet werden, wenn mehr als nur zwei oder drei
     * Datensätze in der Datenbank abgelegt werden.
     * 
     * @return Ob das Starten einer Transaktion erfolgreich war,
     *  oder nicht
     * @todo Auch für eine normale DatabaseConnection ermöglichen?
     */
    bool endTransaction();
};

namespace biker_tests
{
    int testSpatialiteDatabaseConnection();
}

#endif //SPATIALITEDATABASE_HPP
