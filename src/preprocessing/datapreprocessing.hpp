#ifndef DATAPREPROCESSING_HPP
#define DATAPREPROCESSING_HPP

#include <boost/cstdint.hpp>
#include "gpsposition.hpp"
#include "database.hpp"
#include "temporarydatabase.hpp"
#include "blockingqueue.hpp"
#include "routingedge.hpp"
#include "routingnode.hpp"
#include "osmparser.hpp"
#include "pbfparser.hpp"
#include "osmway.hpp"
#include "osmnode.hpp"
#include "osmedge.hpp"
#include "osmturnrestriction.hpp"
#include "routingnode.hpp"
#include "routingedge.hpp"
#include "tests.hpp"
//~ #include <QtConcurrentRun>
//~ #include <QDebug>
//~ #include <QThread>
//~ #include <QString>
//~ #include <qtconcurrentrun.h>
//~ #include <QApplication>
#include <QtConcurrentRun>
#include <QString>
#include "spatialitedatabase.hpp"
#include "sqlitedatabase.hpp"

/**
 * @brief Diese Klasse kuemmert sich um jegliche Form der Datenvorverarbeitung
 * 
 * @ingroup preprocessingj
 * @author Sebastian Koehlert
 * @date 2011-12-21
 * @copyright GNU GPL v3
 * @todo
 */

class DataPreprocessing
{
private:
    boost::shared_ptr<OSMNode> _osmNode;
    boost::shared_ptr<OSMWay> _osmWay;
    boost::shared_ptr<OSMTurnRestriction> _osmTurnRestriction;
    
    boost::shared_ptr<RoutingNode> routingNode;
    boost::shared_ptr<RoutingEdge> routingEdge;
    
    BlockingQueue<boost::shared_ptr<OSMNode> > _nodeQueue;
    BlockingQueue<boost::shared_ptr<OSMWay> > _wayQueue;
    BlockingQueue<boost::shared_ptr<OSMTurnRestriction> > _turnRestrictionQueue;        

    /**
     * @brief Wandelt eine <code>OSMEdge</code> Kante in eine <code>RoutingEdge</code> Kante um und kategorisiert diese.
     *
     * Wann immer eine Kante nicht von einem Fahrrad passiert werden kann, wird die Eigenschaft der Zugangsbeschränkung auf den Wert für <code>ACCESS_NOT_USABLE_FOR_BIKES</code> gesetzt.
     * Eigenschaften wie Ampeln beziehen sich immer auf des Ende einer Kante.
     *
     * @param osmEdge Die umzuwandelnde <code>OSMEdge</code> Kante.
     * @return Ein Pointer auf die aus der <code>OSMEdge</code> Kante erstellte <code>RoutingEdge</code> Kante.
     */
    boost::shared_ptr<RoutingEdge> categorizeEdge(const OSMEdge& osmEdge);

    
    boost::shared_ptr<OSMParser> _osmParser;
    #ifdef PROTOBUF_FOUND
        boost::shared_ptr<PBFParser> _pbfParser;
    #endif
    
    TemporaryOSMDatabaseConnection _tmpDBConnection;
    boost::shared_ptr<DatabaseConnection> _finalDBConnection;
    
    /**
     * @brief Stellt fest, ob es sich bei einer Kante um eine Straße
     *      handelt, oder nicht.
     * 
     * Prüft lediglich, ob eine Eigenschaft mit highway-Tag vorhanden ist.
     * 
     * @return Ob es sich um eine Straße handelt.
     */
    bool isStreet(const OSMWay& way);
    
public:    
    DataPreprocessing(boost::shared_ptr<DatabaseConnection> finaldb);
    ~DataPreprocessing();
 
    /**
     * @brief Parsed ueber .osm/.pbf-Dateien und wirft 
     * 
     * @return Ob Parse-Prozess erfolgreich war
     */
    bool startparser(QString osmFilename, QString dbFilename);
    bool preprocess();
    bool deQueue();
    bool enQueue();
    int getStreetType(const RoutingEdge& edge);
    int getStreetSurfaceQuality(const RoutingEdge& edge);
    int getStreetSurfaceType(const RoutingEdge& edge);
    void categorizeEdge(const RoutingEdge& edge);
    void createRoutingGraph();
};

namespace biker_tests
{
    /**
     * @ingroup tests
     */
    int testDataPreprocessing();
}
#endif //DATAPREPROCESSING_HPP
