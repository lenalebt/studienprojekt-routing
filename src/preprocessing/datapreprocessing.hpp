#ifndef DATAPREPROCESSING_HPP
#define DATAPREPROCESSING_HPP

#include <boost/cstdint.hpp>
#include "gpsposition.hpp"
#include "database.hpp"
#include "temporarydatabase.hpp"
#include "spatialitedatabase.hpp"
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

    boost::shared_ptr<RoutingEdge> categorizeEdge(const OSMEdge& osmEdge);

    
    boost::shared_ptr<OSMParser> _osmParser;
    boost::shared_ptr<PBFParser> _pbfParser;
    
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
};

namespace biker_tests
{
    /**
     * @ingroup tests
     */
    int testDataPreprocessing();
}
#endif //DATAPREPROCESSING_HPP
