#ifndef SIMPLEDATAPREPROCESSING_HPP
#define SIMPLEDATAPREPROCESSING_HPP

#include <boost/cstdint.hpp>
#include "gpsposition.hpp"
#include "database.hpp"
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
#include "temporarydatabase.hpp"
//~ #include <QtConcurrentRun>
//~ #include <QDebug>
//~ #include <QThread>
//~ #include <QString>
//~ #include <qtconcurrentrun.h>
//~ #include <QApplication>
#include <QtConcurrentRun>
#include <QString>
/**
 * @brief Diese Klasse macht "einfache" Datenvorverarbeitung, d.h. legt
 *      die OSM-Daten 1:1 in die Datenbank ab, ohne wirkliche
 *      Datenbetrachtungen zu machen.
 * 
 * @ingroup preprocessing
 * @author Sebastian Koehlert, Lena Brüder
 * @date 2011-12-21
 * @copyright GNU GPL v3
 * @todo Aufräumen, Kommentieren, Doxygen-Kommentare.
 */

class SimpleDataPreprocessing
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
    
    boost::shared_ptr<OSMParser> _osmParser;
    boost::shared_ptr<PBFParser> _pbfParser;
    
    boost::shared_ptr<DatabaseConnection> _finalDBConnection;
    TemporaryOSMDatabaseConnection _tmpDBConnection;
    
    bool preprocess();
    bool isStreet(const OSMWay& way);
    
public:    
    SimpleDataPreprocessing(boost::shared_ptr<DatabaseConnection> finaldb);
    ~SimpleDataPreprocessing();
 
    bool preprocess(QString osmFilename, QString dbFilename);
};

namespace biker_tests
{
    /**
     * @ingroup tests
     */
    int testSimpleDataPreprocessing();
}
#endif //SIMPLEDATAPREPROCESSING_HPP
