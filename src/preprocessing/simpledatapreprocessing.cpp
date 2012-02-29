#include "simpledatapreprocessing.hpp"
#include <QSet>
#include "rangetree.hpp"

SimpleDataPreprocessing::SimpleDataPreprocessing(boost::shared_ptr<DatabaseConnection> finaldb)
    : _nodeQueue(10000), _wayQueue(10000), _turnRestrictionQueue(10000),
    _osmParser(),
    #ifdef PROTOBUF_FOUND
        _pbfParser(),
    #endif
    _finalDBConnection(finaldb)
{
    
}

SimpleDataPreprocessing::~SimpleDataPreprocessing()
{
    
}

boost::shared_ptr<RoutingEdge> SimpleDataPreprocessing::categorizeEdge(const OSMEdge &osmEdge) //sollte ich das hier als boost::shared_ptr<OSMEdge> bekommen?
{
    bool hasTrafficLights = false;
    bool hasTrafficCalmingBumps = false;
    //bool hasStopSign = false;
    bool hasStairs = false;
    bool hasCycleBarrier = false;
    bool isDesignated = false;
    boost::uint8_t streetType = STREETTYPE_UNKNOWN;
    boost::uint8_t cyclewayType = CYCLEWAYTYPE_UNKNOWN;
    boost::uint8_t streetSurfaceType = STREETSURFACETYPE_UNKNOWN;
    boost::uint8_t streetSurfaceQuality = STREETSURFACEQUALITY_UNKNOWN;    
    boost::uint8_t access = ACCESS_YES;
    //boost::uint8_t turnType = TURNTYPE_STRAIGHT; // TURNTYPE won't be categorized in this function
    
    static boost::uint64_t edgeID=0;
    
    routingEdge = boost::shared_ptr<RoutingEdge>(new RoutingEdge(edgeID++, RoutingNode::convertIDToLongFormat(osmEdge.getStartNode()), RoutingNode::convertIDToLongFormat(osmEdge.getEndNode())));

    //Flags
    bool isForward = osmEdge.getForward();
    bool isArea = false;
    bool isHighway = false;
    bool isOfficial = false;
    bool hasDesignation = false;


    QVector<OSMProperty> properties = osmEdge.getProperties();
    for (QVector<OSMProperty>::const_iterator it = properties.constBegin(); it != properties.constEnd(); it++)
    {
        QString osmKey = it->getKey();
        QString osmValue = it->getValue();

        //DONE:
        // Key: smoothness, surface, tracktype(only grade1), mtb:scale,
        //TODO:
        // highway, stop, traffic_signal, cycleway, oneway, bicycle, compulsory und designation zusammenstellen

        if(osmValue == "impassalbe"){
            streetSurfaceQuality = STREETSURFACEQUALITY_IMPASSABLE;
            access = ACCESS_NOT_USABLE_FOR_BIKES;
            break; //if the edge can't be passed by bike, there is no need for further categorization
        }

        else if(osmKey == "ice_road" || osmKey == "motorroad" || osmKey == "winterroad"){
            if(osmValue == "yes"){
                access = ACCESS_NOT_USABLE_FOR_BIKES;
                break; //if the edge can't be passed by bike, there is no need for further categorization
            }
        }

        else if(osmKey == "mtb:scale"){
            if(osmValue == "0"){
                cyclewayType = CYCLEWAYTYPE_MTB_0;
            }
            else if(osmValue == "1"){
                cyclewayType = CYCLEWAYTYPE_MTB_1;
            }
            else if(osmValue == "2"){
                cyclewayType = CYCLEWAYTYPE_MTB_2;
            }
            else if(osmValue == "3"){
                cyclewayType = CYCLEWAYTYPE_MTB_3;
            }
            else{
                cyclewayType = CYCLEWAYTYPE_MTB_HIGH;
            }
        }

        else if(osmKey == "highway"){
            isHighway = true;

            if(osmValue == "bus_guideway" ||
                    osmValue == "construction" ||
                    osmValue == "motorway" ||
                    osmValue == "motorway_link" ||
                    osmValue == "proposed" ||
                    osmValue == "raceway"){
                access = ACCESS_NOT_USABLE_FOR_BIKES;
                break; //if the edge can't be passed by bike, there is no need for further categorization
            }
            else if(osmValue == "pedestrian"){
                streetType = STREETTYPE_HIGHWAY_PEDESTRIAN;

            }
            else if(osmValue == "primary" || osmValue == "primary_link" || osmValue == "trunk" || osmValue == "trunk_link"){
                streetType = STREETTYPE_HIGHWAY_PRIMARY;

            }
            else if(osmValue == "secondary" || osmValue == "secondary_link"){
                streetType = STREETTYPE_HIGHWAY_SECONDARY;

            }
            else if(osmValue == "tertiary" || osmValue == "tertiary_link"){
                streetType = STREETTYPE_HIGHWAY_TERTIARY;

            }
            else if(osmValue == "track" || osmValue == "byway"){
                streetType = STREETTYPE_HIGHWAY_TRACK;

            }
            else if(osmValue == "path"){
                streetType = STREETTYPE_HIGHWAY_PATH;

            }
            else if(osmValue == "living_street"){
                streetType = STREETTYPE_HIGHWAY_LIVINGSTREET;

            }
            else if(osmValue == "residential"){
                streetType = STREETTYPE_HIGHWAY_RESIDENTIAL;

            }
            else if(osmValue == "junction"){
                streetType = STREETTYPE_HIGHWAY_JUNCTION;

            }
            else if(osmValue == "service"){
                streetType = STREETTYPE_HIGHWAY_SERVICE;

            }
            else if(osmValue == "ford"){
                streetType = STREETTYPE_HIGHWAY_FORD;

            }
            else{
                streetType = STREETTYPE_UNKNOWN;

            }

            if(osmValue == "steps"){
                hasStairs = true;

            }
        }

        else if(osmKey == "access"){
            if(osmValue == "no"){
                access = ACCESS_NOT_USABLE_FOR_BIKES;
                break; //if the edge can't be passed by bike, there is no need for further categorization
            }
            else if(osmValue == "private"){
                access = ACCESS_PRIVATE;
            }
            else if(osmValue == "delivery"){
                access = ACCESS_DELIVERY;
            }
            else if(osmValue == "destination"){
                access = ACCESS_DESTINATION;
            }
            else if(osmValue == "customer"){
                access = ACCESS_CUSTOMER;
            }
            else if(osmValue == "agricultural"){
                access = ACCESS_AGRICULTURAL;
            }
            else if(osmValue == "forestry"){
                access = ACCESS_FORESTRY;
            }
            else if(osmValue == "permissive"){
                access = ACCESS_PERMISSIVE;
            }
            else if(osmValue == "designated"){
                hasDesignation = true;
            }
            else if(osmValue == "official"){
                isOfficial = true;
            }
        }

        else if(osmKey == "smoothness"){
            if(osmValue == "exellent"){
                    streetSurfaceQuality = STREETSURFACEQUALITY_EXCELLENT;
            }
            else if(osmValue == "good"){
                    streetSurfaceQuality = STREETSURFACEQUALITY_GOOD;
            }
            else if(osmValue == "intermediate"){
                    streetSurfaceQuality = STREETSURFACEQUALITY_INTERMEDIATE;
            }
            else if(osmValue == "bad"){
                    streetSurfaceQuality = STREETSURFACEQUALITY_BAD;
            }
            else if(osmValue == "very_bad"){
                    streetSurfaceQuality = STREETSURFACEQUALITY_VERYBAD;
            }
            else if(osmValue == "horrible"){
                    streetSurfaceQuality = STREETSURFACEQUALITY_HORRIBLE;
            }
            else if(osmValue == "very_horrible"){
                    streetSurfaceQuality = STREETSURFACEQUALITY_VERYHORRIBLE;
            }
        }

        else if(osmKey == "surface" || osmKey.contains("tracktype")){
            if(osmValue == "paved" || osmValue == "grade:1"){
                streetSurfaceType = STREETSURFACETYPE_PAVED;
            }
            else if(osmValue == "asphalt"){
                    streetSurfaceType = STREETSURFACETYPE_ASPHALT;
            }
            else if(osmValue == "sett"){
                    streetSurfaceType = STREETSURFACETYPE_SETT;
            }
            else if(osmValue == "paving_stones"){
                    streetSurfaceType = STREETSURFACETYPE_PAVING_STONES;
            }
            else if(osmValue == "tartarn"){
                    streetSurfaceType = STREETSURFACETYPE_TARTAN;
            }
            else if(osmValue.contains("concrete")){
                    streetSurfaceType = STREETSURFACETYPE_CONCRETE;
            }
            else if(osmValue == "cobblestone"){
                    streetSurfaceType = STREETSURFACETYPE_COBBLESTONE;
            }
            else if(osmValue == "compacted"){
                    streetSurfaceType = STREETSURFACETYPE_COMPACTED;
            }
            else if(osmValue == "fine_gravel"){
                    streetSurfaceType = STREETSURFACETYPE_FINEGRAVEL;
            }
            else if(osmValue == "grass_paver"){
                    streetSurfaceType = STREETSURFACETYPE_GRASSPAVER;
            }
            else if(osmValue == "gravel" || "pebblestone"){
                    streetSurfaceType = STREETSURFACETYPE_GRAVEL;
            }
            else if(osmValue == "ground" || osmValue == "dirt" || osmValue == "mud" || osmValue == "earth" || osmValue == "clay" || osmValue == "sand"){
                    streetSurfaceType = STREETSURFACETYPE_GROUND;
            }
            else if(osmValue == "grass" || osmValue == "artificial_turf"){
                    streetSurfaceType = STREETSURFACETYPE_GRASS;
            }
            else if(osmValue == "metal"){
                    streetSurfaceType = STREETSURFACETYPE_METAL;
            }
            else{
                    streetSurfaceType = STREETSURFACETYPE_UNPAVED;
            }
         }

    }// END: osmEdge properties iterator

    //Flagauswertung//////////////////////////////////////
    if(isArea && !isHighway){
        access = ACCESS_NOT_USABLE_FOR_BIKES;
    }
    //Flagauswertung//END/////////////////////////////////

    routingEdge->setTrafficLights(hasTrafficLights);
    routingEdge->setTrafficCalmingBumps(hasTrafficCalmingBumps);
    //routingEdge->setStopSign(hasStopSign);
    routingEdge->setStairs(hasStairs);
    routingEdge->setCycleBarrier(hasCycleBarrier);
    routingEdge->setAccess(access);
    routingEdge->setStreetType(streetType);
    routingEdge->setCyclewayType(cyclewayType);
    routingEdge->setStreetSurfaceType(streetSurfaceType);
    routingEdge->setStreetSurfaceQuality(streetSurfaceQuality);
    //routingEdge->setTurnType(turnType);

    return routingEdge;
}

bool SimpleDataPreprocessing::isStreet(const OSMWay& way)
{
    //Wenn ein "highway"-key gefunden wird, ist es eine Straße - sonst nicht.
    QVector<OSMProperty> props = way.getProperties();
    for (QVector<OSMProperty>::const_iterator it = props.constBegin(); it != props.constEnd(); it++)
    {
        if (it->getKey() == "highway")
            return true;
    }
    return false;
}

bool SimpleDataPreprocessing::isPassable(const OSMWay& way)
{
    //Wenn ein "highway"-key gefunden wird, ist es eine Straße - sonst nicht.
    QVector<OSMProperty> props = way.getProperties();
    for (QVector<OSMProperty>::const_iterator it = props.constBegin(); it != props.constEnd(); it++)
    {
        if (it->getKey() == "highway")
        {
            if (it->getValue() == "motorway")
                return false;
            else if (it->getValue() == "motorway_link")
                return false;
            else if (it->getValue() == "trunk")
                return false;
            else if (it->getValue() == "trunk_link")
                return false;
        }
    }
    return true;
}

bool SimpleDataPreprocessing::preprocess()
{
    //lege zuerst alle Knoten in die temporäre DB ab.
    std::cerr << "parsing nodes..." << std::endl;
    _tmpDBConnection.beginTransaction();
    int nodeCount=0;
    while(_nodeQueue.dequeue(_osmNode))
    {
        _tmpDBConnection.saveOSMNode(*_osmNode);
        
        if (++nodeCount == 100000)
        {
            nodeCount = 0;
            _tmpDBConnection.endTransaction();
            _tmpDBConnection.beginTransaction();
        }
    }
    _tmpDBConnection.endTransaction();
    
    
    //bearbeite dann alle Kanten.
    std::cerr << "parsing ways..." << std::endl;
    _finalDBConnection->beginTransaction();
    _tmpDBConnection.beginTransaction();
    boost::uint64_t edgeID=0;
    //QSet<boost::uint64_t> nodeIDSet;
    //RangeTree<boost::uint64_t> nodeIDSet;
    AdvancedRangeTree<boost::uint64_t> nodeIDSet;
    int wayCount=0;
    while(_wayQueue.dequeue(_osmWay))
    {
        //edges aus way extrahieren
        if (isStreet(*_osmWay) && isPassable(*_osmWay))
        {
            QVector<OSMEdge> edgeList = _osmWay->getEdgeList();
            for(int i = 0; i < edgeList.size(); i++)
            {
                if (edgeList[i].isOneWayForBikes() != -1)
                {   //temporär, damit einbahnstraßen richtig gemacht werden
                    RoutingEdge routingEdge2(edgeID++, RoutingNode::convertIDToLongFormat(edgeList[i].getStartNode()), RoutingNode::convertIDToLongFormat(edgeList[i].getEndNode()));
                    RoutingEdge routingEdge = *categorizeEdge(edgeList[i]);
                    if (routingEdge.getID() != routingEdge2.getID())
                        std::cerr << "ouch, ID" << std::endl;
                    if (routingEdge.getStartNodeID() != routingEdge2.getStartNodeID())
                        std::cerr << "ouch, StartNodeID" << std::endl;
                    if (routingEdge.getEndNodeID() != routingEdge2.getEndNodeID())
                        std::cerr << "ouch, EndNodeID" << std::endl;
                    _finalDBConnection->saveEdge(routingEdge);
                }
            }
            //So werden nur die Knoten in die DB gelegt, die auch von Edges benutzt werden.
            QVector<boost::uint64_t> memberList = _osmWay->getMemberList();
            for(int i = 0; i < memberList.size(); i++)
            {
                //Das mit dem nodeIDSet mache ich, weil man der DB nicht sagen kann dass sie doppeltes Einfügen ignorieren soll.
                if (!nodeIDSet.contains(memberList[i]))
                {
                    _osmNode = _tmpDBConnection.getOSMNodeByID(memberList[i]);
                    RoutingNode routingNode(_osmNode->getID(), _osmNode->getLat(), _osmNode->getLon());
                    _finalDBConnection->saveNode(routingNode);
                    nodeIDSet.insert(_osmNode->getID());
                }
            }
            
            if (++wayCount == 100000)
            {
                wayCount = 0;
                _finalDBConnection->endTransaction();
                _finalDBConnection->beginTransaction();
            }
        }
    }
    _finalDBConnection->endTransaction();
    _tmpDBConnection.endTransaction();
    
    //Abbiegebeschränkungen werden einfach überlesen.
    std::cerr << "parsing turn restrictions..." << std::endl;
    //Die Queues müssen alle geleert werden, sonst kann das Programm nicht beendet werden!
    while (_turnRestrictionQueue.dequeue(_osmTurnRestriction))
    {
        
    }
    
    //Am Schluss noch Indexe erstellen
    std::cerr << "creating indexes..." << std::endl;
    _finalDBConnection->createIndexes();
    return true;
}

bool SimpleDataPreprocessing::preprocess(QString fileToParse, QString dbFilename)
{
    _finalDBConnection->open(dbFilename);
    if (!_finalDBConnection->isDBOpen())
    {
        std::cerr << "was not able to open database file \"" << dbFilename << "\"." << std::endl;
        return false;
    }
    QTemporaryFile tmpFile;
    tmpFile.open();
    QString tmpFilename = tmpFile.fileName();
    tmpFile.close();
    tmpFile.remove();
    _tmpDBConnection.open(tmpFilename);
    
    
    //Prueft, ob .osm oder .pbf am Ende vorhanden
    if(fileToParse.endsWith(".osm"))
    {
        _osmParser.reset(new OSMParser(&_nodeQueue, &_wayQueue, &_turnRestrictionQueue));
        QFuture<bool> future = QtConcurrent::run(_osmParser.get(), &OSMParser::parse, fileToParse);
        
        bool preprocessRetval = preprocess();
        future.waitForFinished();
        _finalDBConnection->close();
        _tmpDBConnection.close();
        tmpFile.remove();
        return (preprocessRetval && future.result());
    }
    #ifdef PROTOBUF_FOUND
        else if (fileToParse.endsWith(".pbf"))
        {
            _pbfParser.reset(new PBFParser(&_nodeQueue, &_wayQueue, &_turnRestrictionQueue));
            QFuture<bool> future = QtConcurrent::run(_pbfParser.get(), &PBFParser::parse, fileToParse);
            
            bool preprocessRetval = preprocess();
            future.waitForFinished();
            _finalDBConnection->close();
            _tmpDBConnection.close();
            tmpFile.remove();
            return (preprocessRetval && future.result());
        }
    #endif
    else
    {
        std::cerr << "no parser for file \"" << fileToParse << "\" available." << std::endl;
        return false;
    }
}

namespace biker_tests
{    
    int testSimpleDataPreprocessing()
    {
        QFile file("rub.db");
        
        std::cerr << "Removing database test file \"rub.db\"..." << std::endl;
        if (file.exists())
            file.remove();
        
        #ifdef SPATIALITE_FOUND
            boost::shared_ptr<SpatialiteDatabaseConnection> finalDB(new SpatialiteDatabaseConnection());
        #else
            boost::shared_ptr<SQLiteDatabaseConnection> finalDB(new SQLiteDatabaseConnection());
        #endif
        SimpleDataPreprocessing dataPreprocessing(finalDB);
        CHECK(dataPreprocessing.preprocess("data/rub.osm", "rub.db"));
        CHECK(dataPreprocessing.preprocess("data/bochum_city.osm", "bochum_city.db"));
        return EXIT_SUCCESS;
        //return EXIT_FAILURE;
    }
}
