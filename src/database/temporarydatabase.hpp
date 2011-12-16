#ifndef TEMPORARYDATABASE_HPP
#define TEMPORARYDATABASE_HPP

#include "osmnode.hpp"
#include "osmrelation.hpp"
#include "osmproperty.hpp"
#include "osmedge.hpp"
#include <QVector>
#include <QString>
#include <boost/cstdint.hpp>
#include <sqlite3.h>
#include "tests.hpp"
#include <boost/shared_ptr.hpp>


namespace biker_tests
{
    int testTemporaryOSMDatabaseConnection();
}

/**
 * @brief Diese Klasse ist ein temporärer Speicher für OpenStreetMap-Daten.
 * 
 * Die Daten werden in einer SQLite-Datenbank abgelegt. Diese Datenbank kann
 * dann abgefragt werden nach Knoten, Kanten und Relationen - so, wie
 * sie in der DB abgelegt wurden.
 * 
 * @author Lena Brüder
 * @date 2011-12-14
 * @copyright GNU GPL v3
 * @todo implementieren, Schnittstellen definieren
 */
class TemporaryOSMDatabaseConnection
{
private:
    bool _dbOpen;
    sqlite3* _db;
    
    sqlite3_stmt* _getLastInsertRowIDStatement;
    sqlite3_stmt* _saveOSMPropertyStatement;
    sqlite3_stmt* _getOSMPropertyStatement;
    
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
    
    /**
     * @brief Legt eine OSMProperty in der Datenbank ab
     * @return Die ID, unter der sie abgelegt wurde
     */
    boost::uint64_t saveOSMProperty(const OSMProperty& property);
    
    boost::shared_ptr<OSMProperty> getOSMPropertyByID(boost::uint64_t propertyID);
    
    /**
     * @brief Gibt die ID der letzten Insert-Operation zurück.
     * 
     * Wird gebraucht, um PRIMARY KEY-IDs erzeugen zu lassen
     * 
     * @return Die ID des zuletzt eingefügten INSERTs
     */
    boost::uint64_t getLastInsertRowID();
public:
    TemporaryOSMDatabaseConnection();
    void close();
    void open(QString dbConnectionString);
    bool isDBOpen();
    
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
     */
    bool endTransaction();
    
    bool saveOSMNode(const OSMNode& node);
    bool saveOSMEdge(const OSMEdge& edge);
    bool saveOSMTurnRestriction(const OSMRelation& relation);
    
    
    
    QVector<OSMNode> getNodesByID(boost::uint64_t nodeID);
    QVector<OSMNode> getNodesByID(boost::uint64_t fromNodeID, boost::uint64_t toNodeID);
    QVector<OSMEdge> getEdgesByStartNodeID(boost::uint64_t startNodeID);
    QVector<OSMEdge> getEdgesByEndNodeID(boost::uint64_t endNodeID);
    
    friend int biker_tests::testTemporaryOSMDatabaseConnection();
};


#endif //TEMPORARYDATABASE_HPP
