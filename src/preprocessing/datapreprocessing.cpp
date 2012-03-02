#include "datapreprocessing.hpp" 
#include <QSet>


DataPreprocessing::DataPreprocessing(boost::shared_ptr<DatabaseConnection> finaldb)
    : _nodeQueue(1000), _wayQueue(1000), _turnRestrictionQueue(1000),
    _osmParser(),
    #ifdef PROTOBUF_FOUND
        _pbfParser(),
    #endif
    _finalDBConnection(finaldb)
{
    
}

DataPreprocessing::~DataPreprocessing()
{
    
}

bool DataPreprocessing::isStreet(const OSMWay& way)
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

bool DataPreprocessing::startparser(QString fileToParse, QString dbFilename)
{
    _finalDBConnection->open(dbFilename);
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
        preprocess();        
        future.waitForFinished();

        //TODO: Graphen aus kategorisierten Daten erstellen
        //createNewGraph();

        _finalDBConnection->close();
        _tmpDBConnection.close();
        tmpFile.remove();
        return true;
    }
    else if (fileToParse.endsWith(".pbf"))
    {
        _pbfParser.reset(new PBFParser(&_nodeQueue, &_wayQueue, &_turnRestrictionQueue));
        QFuture<bool> future = QtConcurrent::run(_pbfParser.get(), &PBFParser::parse, fileToParse);
        preprocess();
        future.waitForFinished();

        //TODO: Graphen aus kategorisierten Daten erstellen

        _finalDBConnection->close();
        _tmpDBConnection.close();
        tmpFile.remove();
        return true;
    }

    #ifdef PROTOBUF_FOUND
        else if (fileToParse.endsWith(".pbf"))
        {
            _pbfParser.reset(new PBFParser(&_nodeQueue, &_wayQueue, &_turnRestrictionQueue));
            QFuture<bool> future = QtConcurrent::run(_pbfParser.get(), &PBFParser::parse, fileToParse);
            
            saveNodeToTmpDatabase();
            saveEdgeToTmpDatabase();
            saveTurnRestrictionToTmpDatabase();
            
            std::cerr << "creating indexes" << std::endl;
            _tmpDBConnection.createIndexes();
            _finalDBConnection->createIndexes();
            
            future.waitForFinished();
            return true;
        }
    #endif

    else
    {
        return false;
    }
}

void DataPreprocessing::createRoutingGraph()
{
    Vector<boost::shared_ptr<OSMNode> > OSMNodes = _tmpDBConnection.getOSMNodesByID(1, 10000);
    for(int i = 0; i < OSMNodes.size(); i++)
    {
        QVector< boost::shared_ptr< OSMEdge > > OSMEdgesIncome = _tmpDBConnectiongetOSMEdgesByStartNodeIDWithoutProperties(OSMNodes[i]);
        QVector< boost::shared_ptr< OSMEdge > > OSMEdgesOutgoing = _tmpDBConnectiongetOSMEdgesByEndNodeIDWithoutProperties(OSMNodes[i]);

        if(OSMEdgesIncome.size() > 2 && OSMEdgesOutgoing.size() > 2)
        {
            //sort all edges
            _tmpDBConnection.updateOSMEdgeStartNode(OSMEdgesOutgoing);
            _tmpDBConnection.updateOSMEdgeEndNode(OSMEdgeIncome);
            //handle turnRestrictions here

            for(int j = 0; j < OSMEdgesIncome.size(); j++)
            {
            }
            //save osmNode als routingNode in finalDB
        }
        else if(OSMEdgesIncome.size() >= 1 & OSMEdgesOutgoing.size() >= 1 ) // Keine Kreuzung
        {
            for( int i = 0; i < OSMEdgesIncome.size(); i++)
            {
                //ersetze aktuelle KnotenID durch lange KnotenID
                _tmpDBConnection.updateOSMEdgeEndNode(OSMEdgesIncome[i]);
            }

            for( int i = 0; i < OSMEdgesOutgoing.size(); i++)
            {
                //ersetze aktuelle KnotenID durch lange KnotenID
                _tmpDBConnection.updateOSMEdgeStartNode(OSMEdgesOutgoing[i]);
            }
            routingNode rNode = OSMNodes[i];
        }
    }
    QVector< boost::uint64_t > wayIDs = _tmpDBConnection.getWayIDsInRange(1, 10000);
    for(int i = o ; i < wayIDs.size(); i++)
    {
        OSMEdge osmEdge = _tmpDBConnection.getOSMEdgesByWayIDWithoutProperties(i);
        //TODO: fuer alle wayIDs vorwaerts- und rueckwaertseigenschaften erstellen
        if()
        {
        }
        if()
        {
        }
    }

    /*for( all OSMNODES, aufsteigend nach ID (tmpDB-funktion))
    {
        erzeuge Liste aller eingehenden OSMEdges
        erzeuge Liste aller ausgehenden OSMEdges
        if(# eingehende Kante > 2) && (# ausgehende Kante > 2)//Kreuzung
        {
            -ordne allen Kanten, abhaengig der Himmelsrichung, die neuen, langen IDs zu //Funktion noch in der Mache
             //bei ausgehender Kante ersetze die StartID und bei eingehenden Kanten die EndID durch die langen IDs
            -update die betrachteten Kanten in tmpDB
            - Hier Abbiegebeschraenkung beachten
            for(alle eingehenden Kanten)
            {
                erstelle neue innere Kanten zu allen ausgehenden Kanten
                //Dabei: Fuege Datentypen zu den Kanten und lege neue kanten als RoutingEdge in finalDB ab
            }
            -lege OSMNode als RoutingNode in finalDB ab
        }                                               
        else if(# eingehende & #ausgehende Kanten >= 1) //Keine Kreuzung
        {
            for(alle ein- und ausgehende Kanten)
            {
                -ersetze aktuelle KnotenID durch lange KnotenID
                -Update Edge in tmpDB
            }
            -lege OSMNode als RoutingNode in finalDB ab
        }
    }
    for(all wayIDs aufsteigend)
    {
        -hole alle OSMEdges mit gleicher WayID aus tmpDB;
        -(fuer wayID) erstelle Vorwaertseigenschaften;
        -(fuer wayID) erstelle Rueckwaertseigenschaften;
        if(Strasse kann vorwaerts befahren werden)
        {
            lege vorwaertsKanten mit Vorwaertseigenschaft als RoutingEdge in finalDB ab;
        }
        if(Strasse kann rueckwaerts befahren werden)
        {
            lege rueckwaertsKanten mit Rueckwaertseigenschaft als RoutingEdge in finalDB ab;
        }
    }
*/
}

bool DataPreprocessing::preprocess()
{
    //Nodes in tmp DB speichern
    _tmpDBConnection.beginTransaction();
    int nodeCount = 0;
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

    //alle Edges bearbeiten
    //_finalDBConnection->beginTransaction(); ist hier noch nicht noetig
    boost::uint64_t edgeID=0;
    QSet<boost::uint64_t> nodeIDSet;
    std::cerr << "Parsing Ways..." << std::endl;
    _finalDBConnection->beginTransaction();
    _tmpDBConnection.beginTransaction();
    //boost::uint64_t edgeID=0;
    //TODO: nochmal ueberlegen, ob if-Abfrage nicht sinnvoller als while-loop
    int wayCount=0;
    while(_wayQueue.dequeue(_osmWay))
    {
        //edges aus way extrahieren
        if (isStreet(*_osmWay))
        {
            QVector<OSMEdge> edgeList = _osmWay->getEdgeList();
            for(int i = 0; i < edgeList.size(); i++)
            {                
                categorizeEdge(edgeList[i]);
                RoutingEdge routingEdge(edgeID++, RoutingNode::convertIDToLongFormat(edgeList[i].getStartNode()), RoutingNode::convertIDToLongFormat(edgeList[i].getEndNode()));
               // _finalDBConnection->saveEdge(routingEdge); ist hier noch nicht noetig
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
                    //_finalDBConnection->saveNode(routingNode); ist hier noch nicht noetig
                    nodeIDSet.insert(_osmNode->getID());
                }
            }

            //routingEdge = boost::shared_ptr<RoutingEdge>(new RoutingEdge(edgeList[i].getID(), edgeList[i].getStartNode(), edgeList[i].getEndNode()));
            
            //TODO: Bevor in finale Datenbank gespeichert wird, Hier die Kategorisierung starten
            //categorizeEdge(*routingEdge);

            if (++wayCount == 100000)
            {
                wayCount = 0;
                _finalDBConnection->endTransaction();
                _finalDBConnection->beginTransaction();
            }
        }
    }
    _finalDBConnection->endTransaction();
    
    //Die Queues müssen alle geleert werden, sonst kann das Programm nicht beendet werden!
    while (_turnRestrictionQueue.dequeue(_osmTurnRestriction))
    {
        
    }
    
    //Am Schluss noch Indexe erstellen
    std::cerr << "creating indexes..." << std::endl;
    _finalDBConnection->createIndexes();
    return true;
}

//TODO kategorisierungsfunktionen implementieren
boost::shared_ptr<RoutingEdge> DataPreprocessing::categorizeEdge(const OSMEdge &osmEdge)
{
    bool hasTrafficLights;
    bool hasTrafficCalmingBumps;
    bool hasStopSign;
    bool hasStairs;
    bool hasCycleBarrier;
    boost::uint8_t streetType;
    boost::uint8_t cyclewayType;
    boost::uint8_t streetSurfaceType;
    boost::uint8_t streetSurfaceQuality;
    boost::uint8_t turnType;

    routingEdge = boost::shared_ptr<RoutingEdge>(new RoutingEdge(osmEdge.getID(), osmEdge.getStartNode(), osmEdge.getEndNode()));

    // Hier würden jetzt mit viel if und else, durch Betrachtung der OSMPropertys der OSMEdge, die jeweiligen Werte der routingEdge gesetzt.
    QVector<OSMProperty> props = osmEdge.getProperties();
    
    for(int i = 0; i < props.size(); i++)
    {
        OSMProperty _osmProp = props[i];
        QString _key = _osmProp.getKey();
        QString _value = _osmProp.getValue();

        //if(_osmProp.getKey()=="highway" && _osmProp.getValue()=="unclassified")
        //{
        //    std::cerr << "its a highway, und alle so: yeahh..." << std::endl;
        //}

        //if(_value == "impassable")
        //{
        //    streetSurfaceQuality = STREETSURFACEQUALITY_IMPASSABLE;
        //    break;
        //}
        if(_key == "restriction")
        {
            std::cerr << "---------------------------- restriction" << std::endl;
            if(_value == "no_right_turn")
            {

            }
        }
        else if(_key != "restriction")
        {
            std::cerr << "no restriction" << std::endl;
        }

        else if(_key == "smoothness")
        {
            if(_value == "excellent")
            {
                streetSurfaceQuality = STREETSURFACEQUALITY_EXCELLENT;
            }
            else if(_value == "good")
            {
                streetSurfaceQuality = STREETSURFACEQUALITY_GOOD;
            }
            else if(_value == "intermediate")
            {
                streetSurfaceQuality = STREETSURFACEQUALITY_INTERMEDIATE;
            }
            else if(_value == "bad")
            {
                streetSurfaceQuality = STREETSURFACEQUALITY_BAD;
            }
            else if(_value == "very_bad")
            {
                streetSurfaceQuality = STREETSURFACEQUALITY_VERYBAD;
            }
            else if(_value == "horrible")
            {
                streetSurfaceQuality = STREETSURFACEQUALITY_HORRIBLE;
            }
            else if(_value == "very_horrible")
            {
                streetSurfaceQuality = STREETSURFACEQUALITY_VERYHORRIBLE;
            }
            else if(_value == "unknown")
            {
                streetSurfaceType = STREETSURFACEQUALITY_UNKNOWN;
            }
        }//End if key == smoothness

        else if (_key == "surface" || _key == "tracktype")
        {
            if(_value == "artificial_turf")
            {
                streetSurfaceType = STREETSURFACETYPE_GRASS;
            }
            else if (_value == "asphalt")
            {
                streetSurfaceType = STREETSURFACETYPE_ASPHALT;
            }
            else if (_value.contains("cobblestone"))
            {
                streetSurfaceType = STREETSURFACETYPE_COBBLESTONE;
            }
            else if (_value == "compacted")
            {
                streetSurfaceType = STREETSURFACETYPE_COMPACTED;
            }
            else if (_value.contains("concrete"))
            {
                streetSurfaceType = STREETSURFACETYPE_CONCRETE;
            }
            else if (_value == "dirt")
            {
                streetSurfaceType = STREETSURFACETYPE_GROUND;
            }
            else if (_value == "earth")
            {
                streetSurfaceType = STREETSURFACETYPE_GROUND;
            }
            else if (_value == "fine_gravel")
            {
                streetSurfaceType = STREETSURFACETYPE_FINEGRAVEL;
            }
            else if (_value == "grass")
            {
                streetSurfaceType = STREETSURFACETYPE_GRASS;
            }
            else if (_value == "grass_paver")
            {
                streetSurfaceType = STREETSURFACETYPE_GRASSPAVER;
            }
            else if (_value.contains("paving_stones"))
            {
                streetSurfaceType = STREETSURFACETYPE_PAVING_STONES;
            }
            else if (_value == "gravel")
            {
                streetSurfaceType = STREETSURFACETYPE_GRAVEL;
            }

            else if (_value == "ground" || _value == "dirt" || _value == "mud" || _value == "earth" || _value == "clay" || _value == "sand")
            {
                streetSurfaceType = STREETSURFACETYPE_GROUND;
            }
            //else if (_value == )
            //{
            //}
        }
    }
    
    routingEdge->setTrafficLights(hasTrafficLights);
    routingEdge->setTrafficCalmingBumps(hasTrafficCalmingBumps);
    routingEdge->setStopSign(hasStopSign);
    routingEdge->setStairs(hasStairs);
    routingEdge->setCycleBarrier(hasCycleBarrier);

boost::shared_ptr<RoutingEdge> DataPreprocessing::categorizeEdge(const OSMEdge &osmEdge) //sollte ich das hier als boost::shared_ptr<OSMEdge> bekommen?
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

    routingEdge = boost::shared_ptr<RoutingEdge>(new RoutingEdge(osmEdge.getID(), osmEdge.getStartNode(), osmEdge.getEndNode()));

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
    routingEdge->setTurnType(turnType);

    return routingEdge;
}

//int DataPreprocessing::getStreetType();
//int DataPreprocessing::getStreetSurfaceQuality();
//int DataPreprocessing::getStreetSurfaceType();
}
namespace biker_tests
{    
    int testDataPreprocessing()
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
        DataPreprocessing dataPreprocessing(finalDB);
        dataPreprocessing.startparser("data/rub.osm", "rub.db");
        return EXIT_SUCCESS;
        //return EXIT_FAILURE;
    }
}
