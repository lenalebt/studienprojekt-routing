#ifndef TEMPORARYDATABASE_HPP
#define TEMPORARYDATABASE_HPP

#include "osmnode.hpp"
#include "osmturnrestriction.hpp"
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
    
    sqlite3_stmt* _saveOSMNodeStatement;
    sqlite3_stmt* _getOSMNodeByIDStatement;
    sqlite3_stmt* _saveOSMNodePropertyStatement;
    sqlite3_stmt* _getOSMNodePropertyStatement;
    
    sqlite3_stmt* _saveOSMEdgeStatement;
    sqlite3_stmt* _getOSMEdgeStatement;
    sqlite3_stmt* _saveOSMEdgePropertyStatement;
    sqlite3_stmt* _getOSMEdgePropertyStatement;
    
    sqlite3_stmt* _saveOSMTurnRestrictionStatement;
    sqlite3_stmt* _getOSMTurnRestrictionByViaIDStatement;
    
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
    /**
     * @brief Schließt eine geöffnete DB.
     */
    void close();
    
    /**
     * @brief Öffnet eine DB.
     */
    void open(QString dbConnectionString);
    
    /**
     * @brief Gibt an, ob die DB aktuell geöffnet ist, oder nicht
     * @return Ob die DB geöffnet ist
     */
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
    
    /**
     * @brief Legt eine OSMNode in der temporären Datenbank ab.
     * @return Ob das Ablegen in der Datenbank erfolgreich war, oder nicht
     */
    bool saveOSMNode(const OSMNode& node);
    /**
     * @brief Legt eine OSMEdge in der temporären Datenbank ab.
     * @return Ob das Ablegen in der Datenbank erfolgreich war, oder nicht
     * @todo implementieren. Evtl. Sinnvoller, OSMWay abzulegen und OSMEdge zu laden?
     */
    bool saveOSMEdge(const OSMEdge& edge);
    /**
     * @brief Legt eine OSMTurnRestriction in der temporären Datenbank ab.
     * @return Ob das Ablegen in der Datenbank erfolgreich war, oder nicht
     * @todo implementieren
     */
    bool saveOSMTurnRestriction(const OSMTurnRestriction& turnRestriction);
    
    
    /**
     * @brief Lädt einen Knoten nach ID.
     * @return Den Knoten mit seinen Eigenschaften
     */
    boost::shared_ptr<OSMNode> getOSMNodeByID(boost::uint64_t nodeID);
    /**
     * @brief Lädt ein paar Knoten, deren IDs in einem Intervall liegen.
     * @return Eine Liste der entsprechenden Knoten
     * @todo implementieren
     */
    QVector<boost::shared_ptr<OSMNode> > getOSMNodesByID(boost::uint64_t fromNodeID, boost::uint64_t toNodeID);
    /**
     * @brief Lädt eine Liste von Kanten nach Angabe des Startknotens
     */
    QVector<boost::shared_ptr<OSMEdge> > getOSMEdgesByStartNodeID(boost::uint64_t startNodeID);
    /**
     * @brief Lädt eine Liste von Kanten nach Angabe des Endknotens
     */
    QVector<boost::shared_ptr<OSMEdge> > getOSMEdgesByEndNodeID(boost::uint64_t endNodeID);
    
    /**
     * @brief Lädt eine OSMTurnRestriction aus der DB über die Angabe der via-ID (Knoten).
     */
    QVector<boost::shared_ptr<OSMTurnRestriction> > getOSMTurnRestrictionByViaID(boost::uint64_t viaID);
    
    friend int biker_tests::testTemporaryOSMDatabaseConnection();
    ~TemporaryOSMDatabaseConnection();
};


#endif //TEMPORARYDATABASE_HPP
