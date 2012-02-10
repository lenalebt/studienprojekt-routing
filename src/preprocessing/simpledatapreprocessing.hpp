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
#include <QtConcurrentRun>
#include <QString>
/**
 * @brief Diese Klasse macht "einfache" Datenvorverarbeitung, d.h. legt
 *      die OSM-Daten fast 1:1 in die Datenbank ab, ohne eine komplette
 *      Graphberechnung zu machen.
 * 
 * @ingroup preprocessing
 * @author Sebastian Koehlert, Lena Brüder
 * @date 2011-12-21
 * @copyright GNU GPL v3
 * @todo Kantenkategorisierung, evtl. Graphen ausdünnen?.
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
    
    /**
     * @brief Dies ist die eigentliche Vorverarbeitung.
     * 
     * Arbeitet alle Kanten und Knoten in der OSM-Datei ab,
     * Abbiegebeschränkungen werden ignoriert. Knoten werden zuerst
     * in der temporären DB abgelegt. Es werden dann nur die Knoten in
     * die finale DB abgelegt, die auch wirklich in einer Kante verwendet
     * werden. Da die Datenbank kein "INSERT OR IGNORE" bei Knoten unterstützt,
     * wird eine Liste der Knoten angelegt, die schon in die Datenbank
     * gelegt wurden (->ID) - dies braucht den meisten Speicher der Anwendung
     * beim Parsen.
     * 
     * @return Ob der Vorgang erfolgreich war
     * @todo Kanten kategorisieren
     */
    bool preprocess();
    
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
    SimpleDataPreprocessing(boost::shared_ptr<DatabaseConnection> finaldb);
    ~SimpleDataPreprocessing();
    
    /**
     * @brief Wird aufgerufen, um die Vorverarbeitung zu starten.
     * 
     * @return Ob der Prozess erfolgreich war.
     * @param osmFilename Der Dateiname der OSM-Quelldaten (OSM- oder PBF-Format)
     * @param dbFilename Der Dateiname der Datenbankdatei, in der der Graph abgelegt werden soll. Sollte eine neue, zumindest leere Datei sein.
     */
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
