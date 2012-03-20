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

        createRoutingGraph();
        std::cerr << "creating indexes..." << std::endl;
        _finalDBConnection->createIndexes();
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
        preprocess();

        createRoutingGraph();

        std::cerr << "creating indexes..." << std::endl;

        _finalDBConnection->createIndexes();
        _finalDBConnection->close();
        _tmpDBConnection.close();
        tmpFile.remove();
        return true;
    }
    #endif
    else
    {
        return false;
    }
}

bool DataPreprocessing::preprocess()
{
    std::cerr << "parsing nodes..." << std::endl;
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

    std::cerr << "parsing ways..." << std::endl;
    //alle Edges bearbeiten
    //_finalDBConnection->beginTransaction(); ist hier noch nicht noetig
    boost::uint64_t edgeID=0;
    QSet<boost::uint64_t> nodeIDSet;
    _tmpDBConnection.beginTransaction();
    int wayCount=0;
    while(_wayQueue.dequeue(_osmWay))
    {
        //edges aus way extrahieren
        QVector<OSMEdge> edgeList = _osmWay->getEdgeList();
        for(int i = 0; i < edgeList.size(); i++)
        {
            _tmpDBConnection.saveOSMEdge(edgeList[i]);
        }

        if (++wayCount == 100000)
        {
            wayCount = 0;
            _tmpDBConnection.endTransaction();
            _tmpDBConnection.beginTransaction();
        }
    }
    _tmpDBConnection.endTransaction();

    _tmpDBConnection.beginTransaction();
    std::cerr << "parsing turn restrictions..." << std::endl;
    //Die Queues müssen alle geleert werden, sonst kann das Programm nicht beendet werden!
    while (_turnRestrictionQueue.dequeue(_osmTurnRestriction))
    {
        _tmpDBConnection.saveOSMTurnRestriction(*_osmTurnRestriction);
    }
    _tmpDBConnection.endTransaction();
    
    std::cerr << "creating indexes for temporary database..." << std::endl;
    _tmpDBConnection.createIndexes();
    return true;
}

void DataPreprocessing::categorize(const QVector<OSMProperty> properties, boost::uint64_t& propForward, boost::uint64_t& propBackward)
{
    bool hasTrafficLights = false;
    bool hasTrafficCalmingBumps = false;
    bool hasStopSign = false;
    bool hasStairs = false;
    bool hasCycleBarrier = false;
    boost::uint8_t streetType = STREETTYPE_UNKNOWN;
    boost::uint8_t cyclewayType = CYCLEWAYTYPE_NO_CYCLEWAY;
    boost::uint8_t streetSurfaceType = STREETSURFACETYPE_UNKNOWN;
    boost::uint8_t streetSurfaceQuality = STREETSURFACEQUALITY_UNKNOWN;
    boost::uint8_t access = ACCESS_UNKNOWN;


    routingEdge = boost::shared_ptr<RoutingEdge>(new RoutingEdge(0, 0, 0));


    //////////////////////////////////////////////////////
    //Flags///////////////////////////////////////////////
    bool isWay = false;
    bool isCycleway = false;
    bool isFootway = false;

    bool isOfficial = false;
    bool hasDesignation = false;

    boost::logic::tribool isOneway = boost::logic::indeterminate; // false means oneway in opposite direction
    boost::logic::tribool isOnewayBicycle = boost::logic::indeterminate; // false means bicycles may go both ways
    boost::logic::tribool isBicycleForward = boost::logic::indeterminate;
    boost::logic::tribool isBicycleBackward = boost::logic::indeterminate;

    bool isLane = false;
    bool isTrack = false;
    bool isOpposite = false;
    bool isBusway = false;
    bool isSegregated = false;

    bool definitelyDontUse = false;

    bool accessForward = true;
    bool accessBackward = true;

    boost::logic::tribool isExplicitlyBicycle = boost::logic::indeterminate; // false means no access for bikes
    boost::logic::tribool isExplicitlyFoot = boost::logic::indeterminate; // false means no access for pedestrian
    //Flags//END//////////////////////////////////////////


    //////////////////////////////////////////////////////
    //Keys/und/Values/betrachten//////////////////////////
    QVector<OSMProperty>::const_iterator it;
    for (it = properties.constBegin(); it != properties.constEnd(); it++)
    {
        QString osmKey = it->getKey();
        QString osmValue = it->getValue();


        //Einige K.O.-Argumente///////////////////////////////////
        if(osmValue == "impassalbe"){
            streetSurfaceQuality = STREETSURFACEQUALITY_IMPASSABLE;
            access = ACCESS_NOT_USABLE_FOR_BIKES;
            definitelyDontUse = true;
            break; //if the edge can't be passed by bike, there is no need for further categorization
        }

        else if(osmKey == "ice_road" || osmKey == "motorroad" || osmKey == "winterroad"){
            if(osmValue == "yes"){
                access = ACCESS_NOT_USABLE_FOR_BIKES;
                definitelyDontUse = true;
                break; //if the edge can't be passed by bike, there is no need for further categorization
            }
        }
        //Einige K.O.-Argumente//END//////////////////////////////


        else if(osmKey == "bicycle"){
            isWay = true;
            if(osmValue == "yes"){
                isExplicitlyBicycle = true;
            }
            else if(osmValue == "designated"){
                hasDesignation = true;
                isExplicitlyBicycle = true;
            }
            else if(osmValue == "no"){
                isExplicitlyBicycle = false;
                break; //if the edge can't be passed by bike, there is no need for further categorization
            }
        }
        else if(osmKey == "foot"){
            isWay = true;
            if(osmValue == "yes"){
                isExplicitlyFoot = true;
                isFootway = true;
            }
            else if(osmValue == "designated"){
                hasDesignation = true;
                isExplicitlyFoot = true;
                isFootway = true;
            }
            else if(osmValue == "no"){
                isExplicitlyFoot = false;
            }
        }

        else if(osmKey == "oneway"){
            isWay = true;
            if(osmValue == "yes" || osmValue == "1"){
                isOneway = true;
            }
            else if(osmValue == "-1"){
                isOneway = false;
            }
        }
        else if(osmKey == "vehicle:backward"){
            isWay = true;
            if(osmValue == "no"){
                isOneway = true;
            }
        }
        else if(osmKey == "vehicle:forward"){
            isWay = true;
            if(osmValue == "no"){
                isOneway = false;
            }
        }
        else if(osmKey == "junction"){
            isWay = true;
            if(osmValue == "roundabout"){
                isOneway = true;
            }
        }

        else if(osmKey == "oneway:bicycle"){
            isWay = true;
            if(osmValue == "no"){
                isOnewayBicycle = false;
            }
            if(osmValue == "yes"){
                isOnewayBicycle = true;
            }
        }
        else if(osmKey == "bicycle:backward"){
            isWay = true;
            if(osmValue == "yes"){
                isBicycleBackward = true;
            }
            else if(osmValue == "no"){
                isBicycleBackward = false;
            }
        }
        else if(osmKey == "bicycle:forward"){
            isWay = true;
            if(osmValue == "yes"){
                isBicycleForward = true;
            }
            else if(osmValue == "no"){
                isBicycleForward = false;
            }
        }

        else if(osmKey == "cycleway"){
            isWay = true;
            isCycleway = true;
            if(osmValue.contains("lane")){
                isLane = true;
            }
            else if(osmValue.contains("track")){
                isTrack = true;
            }
            if(osmValue.contains("opposite")){
                isOpposite = true;
            }
            if(osmValue.contains("share_busway")){
                isBusway = true;
            }
        }
        else if(osmKey == "segregated"){
            if(osmValue == "yes"){
                isSegregated = true;
            }
        }

        else if(osmKey == "mtb:scale"){
            isWay = true;
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
            isWay = true;

            if(osmValue == "bus_guideway" ||
                    osmValue == "construction" ||
                    osmValue == "motorway" ||
                    osmValue == "motorway_link" ||
                    osmValue == "proposed" ||
                    osmValue == "raceway"){
                access = ACCESS_NOT_USABLE_FOR_BIKES;
                definitelyDontUse = true;
                break; //if the edge can't be passed by bike, there is no need for further categorization
            }
            else if(osmValue == "unclassified"){
                streetType = STREETTYPE_HIGHWAY_UNCLASSIFIED;

            }
            else if(osmValue == "pedestrian" || osmValue == "footway"){
                streetType = STREETTYPE_HIGHWAY_PEDESTRIAN;
                isFootway = true;

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
            else if(osmValue == "cycleway"){
                isCycleway = true;
                isTrack = true;

            }
            else{
                streetType = STREETTYPE_UNKNOWN;

            }

            if(osmValue == "steps"){
                hasStairs = true;
            }
            else if(osmValue == "traffic_signals"){
                hasTrafficLights = true;
            }
            else if(osmValue == "stop"){
                hasStopSign = true;
            }

        }
        else if(osmKey == "ford"){
            isWay = true;
            if(osmValue != "no"){
                streetType = STREETTYPE_HIGHWAY_FORD;
            }

        }
        else if(osmKey == "barrier"){
            if(osmValue == "cycle_barrier"){
                hasCycleBarrier = true;
            }

        }

        else if(osmKey == "access"){
            if(osmValue == "no"){
                access = ACCESS_NOT_USABLE_FOR_BIKES;
                definitelyDontUse = true;
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
            else if(osmValue == "tartan"){
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

    }
    if(it < properties.constEnd() && access == ACCESS_NOT_USABLE_FOR_BIKES){ // Bikes can't pass. Now we check, if Pedestrians may.
        QVector<OSMProperty>::const_iterator it2;
        if(!isFootway){
            for (it2 = it; it2 != properties.constEnd(); it2++){
                QString osmKey = it->getKey();
                QString osmValue = it->getValue();
                if(osmKey == "foot"){
                    isWay = true;
                    if(osmValue == "yes"){
                        isExplicitlyFoot = true;
                        isFootway = true;
                    }
                    else if(osmValue == "designated"){
                        hasDesignation = true;
                        isExplicitlyFoot = true;
                        isFootway = true;
                    }
                    else if(osmValue == "no"){
                        isExplicitlyFoot = false;
                    }
                }
                else if(osmKey == "highway"){
                    if(osmValue == "pedestrian" || osmValue == "footway"){
                                    streetType = STREETTYPE_HIGHWAY_PEDESTRIAN;
                                    isFootway = true;

                    }
                }

            }
        }
    }
    //Keys/und/Values/betrachten//END/////////////////////


    //////////////////////////////////////////////////////
    //Flagauswertung//////////////////////////////////////
    if(isWay){
        if(access == ACCESS_UNKNOWN){
            access = ACCESS_YES;
        }

        if(isCycleway){
            cyclewayType = CYCLEWAYTYPE_TRACK;

        }

        if(isCycleway){
            cyclewayType = CYCLEWAYTYPE_UNKNOWN; //It's a cycleway. We just don't yet know which kind.
            if(isLane){
                if(isFootway || isSegregated){
                    if(isOpposite){cyclewayType = CYCLEWAYTYPE_LANE_SEGREGAETD_OP;}
                    else{cyclewayType = CYCLEWAYTYPE_LANE_SEGREGAETD;}
                }
                else if(isBusway){
                    if(isOpposite){cyclewayType = CYCLEWAYTYPE_LANE_SHARED_BUSWAY_OP;}
                    else{cyclewayType = CYCLEWAYTYPE_LANE_SHARED_BUSWAY;}
                }
                else{
                    if(isOpposite){cyclewayType = CYCLEWAYTYPE_LANE_OP;}
                    else{cyclewayType = CYCLEWAYTYPE_LANE;}
                }
            }
            else if(isTrack || (streetType == STREETTYPE_HIGHWAY_PATH && isExplicitlyBicycle)){
                if(isFootway || isSegregated){
                    cyclewayType = CYCLEWAYTYPE_TRACK_SEGREGATED;
                }
                else if(isBusway){
                    cyclewayType = CYCLEWAYTYPE_TRACK_SHARED_BUSWAY;
                }
                else{
                    cyclewayType = CYCLEWAYTYPE_TRACK;
                }
            }
        }

        if(isExplicitlyBicycle){
            access = ACCESS_YES;

            if(hasDesignation){
                access = ACCESS_DESIGNATED;
            }
            else if(isOfficial){
                access = ACCESS_COMPULSORY;
            }
        }
        else if(!isExplicitlyBicycle){
            access = ACCESS_NOT_USABLE_FOR_BIKES;
        }

        if(!isBicycleForward){
            accessForward = false;
        }
        if(isOnewayBicycle || (isOneway && !isOpposite) || !isBicycleBackward){
            accessBackward = false;
        }

        if(access == ACCESS_NOT_USABLE_FOR_BIKES && !definitelyDontUse && isFootway){
            access = ACCESS_FOOT_ONLY;
        }
    }
    else{
        definitelyDontUse = true;
        access = ACCESS_NOT_USABLE_FOR_BIKES;
    }
    //Flagauswertung//END/////////////////////////////////


    //////////////////////////////////////////////////////
    //Rückgabeparameter/setzen////////////////////////////
    routingEdge->setTrafficLights(hasTrafficLights);
    routingEdge->setTrafficCalmingBumps(hasTrafficCalmingBumps);
    routingEdge->setStopSign(hasStopSign);
    routingEdge->setStairs(hasStairs);
    routingEdge->setCycleBarrier(hasCycleBarrier);
    routingEdge->setStreetType(streetType);
    routingEdge->setCyclewayType(cyclewayType);
    routingEdge->setStreetSurfaceType(streetSurfaceType);
    routingEdge->setStreetSurfaceQuality(streetSurfaceQuality);

    if(!accessForward){
        if(isWay && !definitelyDontUse){
            access = ACCESS_FOOT_ONLY;
        }
        else{
            access = ACCESS_NOT_USABLE_FOR_BIKES;
        }
    }
    routingEdge->setAccess(access);

    propForward = routingEdge->getProperties();

    if(!accessBackward){
        if(isWay && !definitelyDontUse){
            access = ACCESS_FOOT_ONLY;
        }
        else{
            access = ACCESS_NOT_USABLE_FOR_BIKES;
        }
        routingEdge->setAccess(access);
        propBackward = routingEdge->getProperties();
    }
    else{
        propBackward = propForward;
    }
    //Rückgabeparameter/setzen//END///////////////////////

    return;
}

/**
 * @todo Es landen noch zu viele Knoten in der DB - wie kann das sein? Vermutung:
 *      Es landen einfach /alle/ Knoten in der DB am Ende. Wie kann man das verhindern, 
 *      ohne viel Speicher zu verwenden und sich zu merken, was man schon alles in die
 *      DB legte, bzw. ohne nochmal durch die ganze DB durchzuwandern?
 */
void DataPreprocessing::createRoutingGraph()
{
    std::cerr << "creating routing graph:" << std::endl;
    
    boost::uint64_t edgeID = 0;
    boost::uint64_t firstVal = 0;
    boost::uint64_t lastVal = 0;
    boost::uint32_t maxCount = 100;
    boost::uint64_t maxValue = 18446744073709551615ull;

    _finalDBConnection->beginTransaction();
    _tmpDBConnection.beginTransaction();

    std::cerr << "creating junctions and edges..." << std::endl;

    while(firstVal <= maxValue)
    {
        //gehe in 10K schritten durch die nodes
        lastVal = firstVal + (maxValue>>10);
        if (lastVal<(maxValue>>10))  //Wenn es einen Ueberlauf gab...
            lastVal = maxValue;
        std::cerr << "index: " <<firstVal << std::endl;
        QVector<boost::shared_ptr<OSMNode> > osmNodes = _tmpDBConnection.getOSMNodesByID(firstVal, lastVal, maxCount);

        for(int i = 0; i < osmNodes.size(); i++)
        {           
            QVector< boost::shared_ptr< OSMEdge > > osmEdgesIncome = _tmpDBConnection.getOSMEdgesByEndNodeIDWithoutProperties(osmNodes[i]->getID());
            QVector< boost::shared_ptr< OSMEdge > > osmEdgesOutgoing = _tmpDBConnection.getOSMEdgesByStartNodeIDWithoutProperties(osmNodes[i]->getID());
            QVector<int> sectors;

            if((osmEdgesIncome.size() > 2) && (osmEdgesOutgoing.size() > 2)) //Kreuzung
            {
                RoutingNode rNode(osmNodes[i]->getID(), *osmNodes[i]);
                _finalDBConnection->saveNode(rNode);

                for(int j = 0; j < osmEdgesOutgoing.size(); j++)
                {
                    boost::uint64_t oldStartNode = osmEdgesOutgoing[j]->getStartNodeID();
                    sectors << setNodeBorderingLongID(osmEdgesOutgoing[j], rNode);
                    //std::cerr << *osmEdgesOutgoing[j] << "." <<  std::endl;
                    _tmpDBConnection.updateOSMEdgeStartNode(*osmEdgesOutgoing[j], oldStartNode);

                }
                for(int j = 0; j < osmEdgesIncome.size(); j++)
                {
                    boost::uint64_t oldEndNode = osmEdgesIncome[j]->getEndNodeID();
                    sectors << setNodeBorderingLongID(osmEdgesIncome[j], rNode);
                    //std::cerr << *osmEdgesIncome[j] << "." << std::endl;
                    _tmpDBConnection.updateOSMEdgeEndNode(*osmEdgesIncome[j], oldEndNode);
                }                

                //handle turnRestrictions
                //for(int j = 0; j < osmEdgesIncome.size(); j++)
                //{
                    //erstelle neue innere Kanten zu allen ausgehenden Kanten
                    //Dabei: Fuege Turntypes zu den Kanten und lege neue kanten als RoutingEdge in finalDB ab
                    //TODO: testen. ist dafuer da, die inneren kanten einer kreuzung zu erstellen.                    
                    /*if (osmNodes[i]->getID()==RoutingNode::convertIDToShortFormat(4611686025386215424))
                    {
                        std::cerr << "sectors: " << sectors.size() << std::endl;
                        for (QVector<int>::iterator it = sectors.begin(); it != sectors.end(); it++)
                            std::cerr << *it << ",";
                    }*/
                    RoutingEdge rEdge;
                    for(QVector<int>::iterator startNodeSector = sectors.begin(); startNodeSector != sectors.end(); startNodeSector++)
                    {
                        if (*startNodeSector % 2 == 1)
                            continue;
                        for(QVector<int>::iterator endNodeSector = sectors.begin(); endNodeSector != sectors.end(); endNodeSector++)
                        {
                            if (*endNodeSector % 2 == 0)
                                continue;
                            rEdge.setID(edgeID++);
                            /*if (osmNodes[i]->getID()==RoutingNode::convertIDToShortFormat(4611686025386215424))
                                std::cerr << *startNodeSector << " to " << *endNodeSector << std::endl;*/
                            rEdge.setStartNodeID(RoutingNode::convertIDToLongFormat(osmNodes[i]->getID()) + *endNodeSector);
                            rEdge.setEndNodeID(RoutingNode::convertIDToLongFormat(osmNodes[i]->getID()) + *startNodeSector);
                            //rEdge.setTurnType(TURNTYPE_STRAIGHTCROSS);
                            rEdge.setTurnType(getTurnTypeBySectorNumbers(*startNodeSector, *endNodeSector));
                            rEdge.setStreetType(STREETTYPE_UNKNOWN);
                            //TODO: Evtl noch andere Eigenschaften setzen? Ampel etc?
                            /*if (osmNodes[i]->getID()==RoutingNode::convertIDToShortFormat(4611686025386215424))
                                std::cerr << rEdge << std::endl;*/
                            _finalDBConnection->saveEdge(rEdge);
                        }
                    }
                //}
            }
            else if((osmEdgesIncome.size() >= 1) && (osmEdgesOutgoing.size() >= 1) ) // Keine Kreuzung
            {
                for( int j = 0; j < osmEdgesIncome.size(); j++)
                {
                    boost::uint64_t oldEndNode = osmEdgesIncome[j]->getEndNodeID();
                    osmEdgesIncome[j]->setEndNodeID(RoutingNode::convertIDToLongFormat(osmEdgesIncome[j]->getEndNodeID()));
                    //std::cerr << *osmEdgesIncome[j] << std::endl;
                    _tmpDBConnection.updateOSMEdgeEndNode(*osmEdgesIncome[j], oldEndNode);
                }

                for( int j = 0; j < osmEdgesOutgoing.size(); j++)
                {
                    boost::uint64_t oldStartNode = osmEdgesOutgoing[j]->getStartNodeID();
                    osmEdgesOutgoing[j]->setStartNodeID(RoutingNode::convertIDToLongFormat(osmEdgesOutgoing[j]->getStartNodeID()));
                    //std::cerr << *osmEdgesOutgoing[j] << std::endl;
                    _tmpDBConnection.updateOSMEdgeStartNode(*osmEdgesOutgoing[j], oldStartNode);
                }
                RoutingNode rNode(osmNodes[i]->getID(), *osmNodes[i]);
                _finalDBConnection->saveNode(rNode);
            }
        }
        if (osmNodes.size()>0)
        {
            firstVal = osmNodes[osmNodes.size()-1]->getID() + 1;
        }
        else
            firstVal = lastVal + 1;

        if(firstVal == 0)
            break;

        _finalDBConnection->endTransaction();
        _finalDBConnection->beginTransaction();
        _tmpDBConnection.endTransaction();
        _tmpDBConnection.beginTransaction();
    }

    _tmpDBConnection.endTransaction();
    _finalDBConnection->endTransaction();
    _finalDBConnection->beginTransaction();

    std::cerr << "saving edges to final database..." << std::endl;

    while(firstVal <= maxValue)
    {
        //gehe in 10K schritten durch die nodes
        lastVal = firstVal + (maxValue>>10);
        if (lastVal<(maxValue>>10))  //Wenn es einen Ueberlauf gab...
            lastVal = maxValue;

        QVector< boost::uint64_t > wayIDs = _tmpDBConnection.getWayIDsInRange(firstVal, lastVal, maxCount);

        for(int i = 0 ; i < wayIDs.size(); i++)
        {
            boost::uint64_t propForward = 0;
            boost::uint64_t propBackward = 0;
            bool edgeForward = true;
            bool edgeBackward = true;
            //std::cerr << "iterating over wayIDs" << std::endl;
            QVector< boost::shared_ptr< OSMEdge > > tmpOsmEdges = _tmpDBConnection.getOSMEdgesByWayIDWithoutProperties(wayIDs[i]);

            //da alle edges in der way die gleichen eigenschaften haben, eine kante rausholen und propFor- und propBackward mittels
            //categorize vor-, bzw rueckwaertseigenschafen berechnen lassen
            categorize(_tmpDBConnection.getOSMPropertyListByWayID(wayIDs[i]), propForward, propBackward);

            routingEdge = boost::shared_ptr<RoutingEdge>(new RoutingEdge(0, 0, 0));
            routingEdge->setProperties(propForward);

            if(routingEdge->getAccess() == ACCESS_NOT_USABLE_FOR_BIKES)
            {
                edgeForward = false;
            }
            else
            {
                edgeForward = true;
            }

            routingEdge->setProperties(propBackward);
            if(routingEdge->getAccess() == ACCESS_NOT_USABLE_FOR_BIKES)
            {
                edgeBackward = false;
            }
            else
            {
                edgeBackward = true;
            }

            for(int j = 0; j < tmpOsmEdges.size(); j++)
            {
                //OSMEdge osmEdge = *tmpOsmEdges[j];
                //QVector<OSMProperty> osmProp = osmEdge.getProperties();
                if(edgeForward)
                {
                    RoutingEdge rEdge(edgeID++, tmpOsmEdges[j]->getStartNodeID(), tmpOsmEdges[j]->getEndNodeID(), propForward);
                    _finalDBConnection->saveEdge(rEdge);
                }
                if(edgeBackward)
                {
                    RoutingEdge rEdge(edgeID++, tmpOsmEdges[j]->getStartNodeID(), tmpOsmEdges[j]->getEndNodeID(), propBackward);
                    _finalDBConnection->saveEdge(rEdge);
                }
            }
        }

        if(wayIDs.size() > 0)
        {
            firstVal = wayIDs[wayIDs.size() - 1] + 1;
        }
        else
            firstVal = lastVal + 1;
        if(firstVal == 0)
            break;

        _finalDBConnection->endTransaction();
        _finalDBConnection->beginTransaction();
    }

    _finalDBConnection->endTransaction();

    //QVector<OSMProperty>::const_iterator it;
    //for (it = properties.constBegin(); it != properties.constEnd(); it++)
    //{
        //QString osmKey = it->getKey();
        //QString osmValue = it->getValue();

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

int DataPreprocessing::setNodeBorderingLongID(boost::shared_ptr<OSMEdge> edge, const RoutingNode& junction)
{
    static QCache<boost::uint64_t, OSMNode > cache(1000);

    bool junctionAtStartNode = false;
    boost::uint64_t shortJunctionNodeID = (junction.isIDInLongFormat() ? RoutingNode::convertIDToShortFormat(junction.getID()) : junction.getID());
    boost::uint64_t shortSecondNodeID = 0;

    if (RoutingNode::convertIDToShortFormat(edge->getStartNodeID())==shortJunctionNodeID)
    {
        //Kreuzung am Startknoten
        junctionAtStartNode = true;
        shortSecondNodeID = RoutingNode::convertIDToShortFormat(edge->getEndNodeID());
    }
    else if (RoutingNode::convertIDToShortFormat(edge->getEndNodeID())==shortJunctionNodeID)
    {
        //Kreuzung am Endknoten
        junctionAtStartNode = false;
        shortSecondNodeID = RoutingNode::convertIDToShortFormat(edge->getStartNodeID());
    }
    else
    {
        //weird. irgendwas ist falsch.
        std::cerr << "weird things happened while doing junction calculations." << std::endl;
    }

    OSMNode* secondNode;
    if (cache.contains(shortSecondNodeID))
        secondNode = cache[shortSecondNodeID];
    else
    {
        secondNode = new OSMNode(*(_tmpDBConnection.getOSMNodeByID(shortSecondNodeID)));
        cache.insert(shortSecondNodeID, secondNode);
    }

    int sector = getSector(junction.calcCourseAngle(*secondNode));
    if(junctionAtStartNode)
    {
        edge->setStartNodeID(RoutingNode::convertIDToLongFormat(edge->getStartNodeID()) + sector + 0);
        return sector;
    }
    else
    {
        edge->setEndNodeID(RoutingNode::convertIDToLongFormat(edge->getEndNodeID()) + sector + 1);
        return sector+1;
    }
}

int DataPreprocessing::getTurnTypeBySectorNumbers(int startSector, int endSector)
{
    //(((startSector - endSector) + 720 + 45) % 360) / 90

    // 1: Bilde Winkeldifferenz zwischen start- und endSector
    // 2: Drehe den Winkel so, dass Geradeaus von -45 bis +45 Grad liegt (Statt 0 bis 90 Grad)
    // 3: Ordne den gedrehten Winkeln einen von 4 Sektoren zu (0-3)
    // 4: Addiere 1 hinzu, damit das auf die Turntypes der routingEdge passt
    //TODO: Das /2 muss da rein, weil sonst die Winkel zu groß sind (zwischen 0 uhnd 256), wir erwarten aber Sektornummern zwischen 0 und 128
    return ((((startSector - endSector)/2 + 256 + 16) % 128) / 32) + 1;
}

int DataPreprocessing::getSector(double angle)
{
    if(angle > 360){
        int factor = int(angle / 360);
        angle = angle - (factor * 360);
    }
    return (int(angle * (128.0/360.0))*2); // 0.355555555... = 128/360
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
