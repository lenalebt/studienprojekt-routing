#ifndef DATAPREPROCESSING_HPP
#define DATAPREPROCESSING_HPP

#include <boost/cstdint.hpp>
#include <boost/logic/tribool.hpp>
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
 * @ingroup preprocessing
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
     * @brief Analysiert den Inhalt eines <code>OSMProperty</code>-Vectors und ordnet ihn den Kategorien einer <code>RoutingEdge</code> zu. Die Ergebnisse werden in je einen <code>boost::uint64_t</code>-Wert für Hin- und Rückrichtung eingetragen.
     *
     * Wann immer eine Kante oder ein Weg mit den entsprechenden <code>OSMProperty</code>s nicht von einem Fahrrad passiert werden kann, wird die Eigenschaft der Zugangsbeschränkung auf den Wert für <code>ACCESS_NOT_USABLE_FOR_BIKES</code> gesetzt.
     * Solche Wege und Kanten brauchen nicht in die finale Datenbank eingetragen werden.
     *
     * @param properties Der zu kategorisierende <code>QVector<OSMProperty></code>.
     * @param propForward Der Rückgabeparameter, in den die <code>RoutingEdge</code>-Kategorien bezüglich Forwärtsrichtung eingetragen werden.
     * @param propBackward Der Rückgabeparameter, in den die <code>RoutingEdge</code>-Kategorien bezüglich Gegenrichtung eingetragen werden.
     * @return Ein Pointer auf die aus der <code>OSMEdge</code> Kante erstellte <code>RoutingEdge</code> Kante.
     */
    void categorize(const QVector<OSMProperty> properties, boost::uint64_t& propForward,boost::uint64_t& propBackward);

    
    boost::shared_ptr<OSMParser> _osmParser;
    boost::shared_ptr<PBFParser> _pbfParser;
    
    TemporaryOSMDatabaseConnection _tmpDBConnection;
    boost::shared_ptr<DatabaseConnection> _finalDBConnection;
    
public:    
    DataPreprocessing(boost::shared_ptr<DatabaseConnection> finaldb);
    ~DataPreprocessing();
 
    bool startparser(QString osmFilename, QString dbFilename);
    bool deQueue();
    bool enQueue();
    void saveNodeToTmpDatabase();
    void saveEdgeToTmpDatabase();
    void saveTurnRestrictionToTmpDatabase();
    void saveNodeToDatabase(const RoutingNode& node);
    void saveEdgeToDatabase(const RoutingEdge& edge);
    void saveTurnRestrictionToDatabase();

    void categorizeEdge(const RoutingEdge& edge);

    int getStreetType(const RoutingEdge& edge);
    int getStreetSurfaceQuality(const RoutingEdge& edge);
    int getStreetSurfaceType(const RoutingEdge& edge);
};

namespace biker_tests
{
    /**
     * @ingroup tests
     */
    int testDataPreprocessing();
}
#endif //DATAPREPROCESSING_HPP
