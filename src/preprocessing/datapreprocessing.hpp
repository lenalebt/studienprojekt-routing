#ifndef DATAPREPROCESSING_HPP
#define DATAPREPROCESSING_HPP

#include <boost/cstdint.hpp>
#include <boost/logic/tribool.hpp>
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
     * @brief Analysiert den Inhalt eines <code>OSMProperty</code>-Vectors und ordnet ihn den Kategorien einer <code>RoutingEdge</code> zu. Die Ergebnisse werden in je einen <code>boost::uint64_t</code>-Wert für Hin- und Rückrichtung eingetragen.
     *
     * Wann immer eine Kante oder ein Weg mit den entsprechenden <code>OSMProperty</code>s nicht von einem Fahrrad passiert werden kann, wird die Eigenschaft der Zugangsbeschränkung auf den Wert für <code>ACCESS_NOT_USABLE_FOR_BIKES</code> gesetzt.
     * Solche Wege und Kanten brauchen nicht in die finale Datenbank eingetragen werden.
     *
     * @param properties Der zu kategorisierende <code>QVector<OSMProperty></code>.
     * @param propForward Der Rückgabeparameter, in den die <code>RoutingEdge</code>-Kategorien bezüglich Forwärtsrichtung eingetragen werden.
     * @param propBackward Der Rückgabeparameter, in den die <code>RoutingEdge</code>-Kategorien bezüglich Gegenrichtung eingetragen werden.
     */
    void categorize(const QVector<OSMProperty> properties, boost::uint64_t& propForward,boost::uint64_t& propBackward);

    /**
     * @brief Skaliert eine Winkel(Gradmaß) auf den Wertebereich mod 128 und gibt den nächsten geraden Wert <= dem ermittelten Wert zurück.
     *
     *
     * @param angle Ein Winkel im Gradmaß.
     * @return Ein gerader Wert von 0 bis 126.
     */
    int getSector(double angle);

    
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

    /**
     * @brief Gibt die korrekte longID fuer die <code>edge</code> zurueck, die zu der Kreuzung an <code>node</code> passt
     *
     * @param edge die konkrete Edge
     * @param node der zu der Edge gehoerende Node (der Kreuzungspunkt)
     *
     * @return die korrekte longID
     */
    int setNodeBorderingLongID(boost::shared_ptr<OSMEdge> edge, const RoutingNode& junction);

    /**
     * @brief Gibt den passenden TurnType zurueck
     *
     * @param startSector sektor des startNodes
     * @param endSector sektor des endNodes
     *
     * @return turntType als int-Wert
     * @see RoutingEdge
     */
    int getTurnTypeBySectorNumbers(int startSector, int endSector);
    
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
    /**
     * @brief Erstellt aus dem gegebenden Graphen einen neuen, der in der finalen Datenbank abgelegt wird
     */
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
