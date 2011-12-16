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
    
    bool createTables();
    bool execCreateTableStatement(std::string);
    
    boost::uint64_t saveOSMProperty(const OSMProperty& property);
    boost::uint64_t getLastInsertRowID();
public:
    TemporaryOSMDatabaseConnection();
    void close();
    void open(QString dbConnectionString);
    bool isDBOpen();
    
    bool beginTransaction();
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
