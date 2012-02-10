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

//Muss oben stehen, sonst kann man die Funktion nicht als friend unten benutzen.
namespace biker_tests
{
    /**
     * @ingroup tests
     */
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
 * @ingroup database
 */
class TemporaryOSMDatabaseConnection
{
private:
    bool _dbOpen;
    sqlite3* _db;
    
    sqlite3_stmt* _getLastInsertRowIDStatement;
    
    sqlite3_stmt* _saveOSMPropertyStatement;
    sqlite3_stmt* _getOSMPropertyStatement;
    sqlite3_stmt* _getOSMPropertyIDStatement;
    
    sqlite3_stmt* _saveOSMNodeStatement;
    sqlite3_stmt* _getOSMNodeByIDStatement;
    sqlite3_stmt* _getManyOSMNodesByIDStatement;
    sqlite3_stmt* _saveOSMNodePropertyStatement;
    sqlite3_stmt* _getOSMNodePropertyStatement;
    
    sqlite3_stmt* _saveOSMEdgeStatement;
    sqlite3_stmt* _getOSMEdgeByStartNodeIDStatement;
    sqlite3_stmt* _getOSMEdgeByEndNodeIDStatement;
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
     * @param paramCreateTableStatement Das CREATE TABLE-Statement, das ausgeführt
     *      werden soll.
     * @return Ob die Ausführung erfolgreich war, oder nicht
     */
    bool execCreateTableStatement(std::string paramCreateTableStatement);
    
    /**
     * @brief Legt eine OSMProperty in der Datenbank ab
     * @param property Die Eigenschaft, die in der Datenbank abgelegt werden soll
     * @return Die ID, unter der sie abgelegt wurde
     */
    boost::uint64_t saveOSMProperty(const OSMProperty& property);
    
    /**
     * @brief Lädt eine OSMProperty aus der Datenbank.
     * @param propertyID Die ID, unter der die Eigenschaft in der Datenbank abgelegt wurde.
     * @return Die entsprechende Eigenschaft als OSMProperty-Objekt
     */
    boost::shared_ptr<OSMProperty> getOSMPropertyByID(boost::uint64_t propertyID);
    
    /**
     * @brief Schaut in der DB nach, welche ID eine Eigenschaft hat
     * @param property Die Eigenschaft, dessen ID herausgesucht werden soll
     * @return Die ID, wenn sie gefunden wurde - 0, sonst.
     */
    boost::uint64_t getOSMPropertyID(const OSMProperty& property);
    
    /**
     * @brief Gibt die ID der letzten Insert-Operation zurück.
     * 
     * Wird gebraucht, um PRIMARY KEY-IDs erzeugen zu lassen
     * 
     * @return Die ID des zuletzt eingefügten INSERTs
     */
    boost::uint64_t getLastInsertRowID();
    
    /**
     * @brief Lädt für die übergebenen Kanten alle Eigenschaften nach.
     * 
     * Wird in 2 Funktionen verwendet, daher habe ich den Code ausgelagert.
     * Da die Funktion direkt auf den Objekten arbeitet, die sich hinter
     * den Zeigern in der Liste verbergen, gibt es keinen Rückgabewert
     * außer, ob die Funktion erfolgreich abgeschlossen wurde.
     * 
     * @return Ob die Funktion erfolgreich abgeschlossen wurde.
     * 
     */
    bool getOSMEdgeListProperties(QVector<boost::shared_ptr<OSMEdge> > edgeList);
public:
    TemporaryOSMDatabaseConnection();
    /**
     * @brief Schließt eine geöffnete DB.
     */
    void close();
    
    /**
     * @brief Öffnet eine DB.
     * @param dbConnectionString Gibt den Verbindungsstring für die Datenbank an,
     *      normalerweise der Dateiname.
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
     * @brief Erstellt die Indexe in der Datenbank
     * @return Ob die Indexe erstellt werden konnten
     */
    bool createIndexes();
    
    /**
     * @brief Legt eine OSMNode in der temporären Datenbank ab.
     * @param node Die OSMNode, die abgelegt wird.
     * @return Ob das Ablegen in der Datenbank erfolgreich war, oder nicht
     */
    bool saveOSMNode(const OSMNode& node);
    /**
     * @brief Legt eine OSMEdge in der temporären Datenbank ab.
     * @param edge Die OSMEdge, die abgelegt wird.
     * @return Ob das Ablegen in der Datenbank erfolgreich war, oder nicht
     * @todo testen
     */
    bool saveOSMEdge(const OSMEdge& edge);
    /**
     * @brief Legt eine OSMTurnRestriction in der temporären Datenbank ab.
     * @param turnResriction Die OSMTurnRestriction, die abgelegt wird.
     * @return Ob das Ablegen in der Datenbank erfolgreich war, oder nicht
     */
    bool saveOSMTurnRestriction(const OSMTurnRestriction& turnRestriction);
    
    
    /**
     * @brief Lädt einen Knoten nach ID.
     * @param nodeID Die ID des Knotens.
     * @return Den Knoten mit seinen Eigenschaften
     */
    boost::shared_ptr<OSMNode> getOSMNodeByID(boost::uint64_t nodeID);
    /**
     * @brief Lädt ein paar Knoten, deren IDs in einem Intervall liegen.
     * 
     * Die Knoten werden in aufsteigender Reihenfolge geladen. Wenn die
     * Anzahl Knoten begrenzt wurde gibt der letzte Knoten in der Liste an,
     * von wo aus im nächsten Schritt weitergeladen werden sollte.
     * 
     * @param fromNodeID Die ID des ersten Knotens, der geladen wird (einschließlich)
     * @param toNodeID Die ID des letzten Knotens, der geladen wird (einschließlich)
     * @param maxCount Die Anzahl der Knoten, die maximal gleichzeitig
     *      geladen werden. Bei 0 existiert kein Limit.
     * @return Eine Liste der entsprechenden Knoten
     * @todo testen
     */
    QVector<boost::shared_ptr<OSMNode> > getOSMNodesByID(boost::uint64_t fromNodeID, boost::uint64_t toNodeID, int maxCount=1000);
    
    /**
     * @brief Lädt eine Liste von Kanten nach Angabe des Startknotens
     * @param startNodeID Die ID des Startknotens.
     * @return Eine Liste mit entsprechenden Kanten
     */
    QVector<boost::shared_ptr<OSMEdge> > getOSMEdgesByStartNodeID(boost::uint64_t startNodeID);
    /**
     * @brief Lädt eine Liste von Kanten nach Angabe des Endknotens
     * @param endNodeID Die ID des Endknotens.
     * @return Eine Liste mit entsprechenden Kanten
     */
    QVector<boost::shared_ptr<OSMEdge> > getOSMEdgesByEndNodeID(boost::uint64_t endNodeID);
    
    /**
     * @brief Lädt eine OSMTurnRestriction aus der DB über die Angabe der via-ID (Knoten).
     * @param viaID Die ID des Via-Knotens (der in der Mitte).
     * @return Eine Liste mit entsprechenden Abbiegebeschränkungen
     * @todo Sollte in der To- und from-ID Knoten zurueckgeben
     */
    QVector<boost::shared_ptr<OSMTurnRestriction> > getOSMTurnRestrictionByViaID(boost::uint64_t viaID);
    
    friend int biker_tests::testTemporaryOSMDatabaseConnection();
    ~TemporaryOSMDatabaseConnection();
};


#endif //TEMPORARYDATABASE_HPP
