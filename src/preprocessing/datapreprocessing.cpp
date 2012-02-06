#include "datapreprocessing.hpp" 


DataPreprocessing::DataPreprocessing(boost::shared_ptr<DatabaseConnection> finaldb)
    : _nodeQueue(1000), _wayQueue(1000), _turnRestrictionQueue(1000),
    _osmParser(),
    _pbfParser(),
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
    
    std::cerr << "temp file created." << std::endl;

    //Prueft, ob .osm oder .pbf am Ende vorhanden
    if(fileToParse.endsWith(".osm"))
    {
        _osmParser.reset(new OSMParser(&_nodeQueue, &_wayQueue, &_turnRestrictionQueue));
        QFuture<bool> future = QtConcurrent::run(_osmParser.get(), &OSMParser::parse, fileToParse);
            
        saveNodeToTmpDatabase();
        saveEdgeToTmpDatabase();        
        saveTurnRestrictionToTmpDatabase();
                
        future.waitForFinished();
        return true;
    }
    else if (fileToParse.endsWith(".pbf"))
    {
        _pbfParser.reset(new PBFParser(&_nodeQueue, &_wayQueue, &_turnRestrictionQueue));
        QFuture<bool> future = QtConcurrent::run(_pbfParser.get(), &PBFParser::parse, fileToParse);

        saveNodeToTmpDatabase();
        saveEdgeToTmpDatabase();
        saveTurnRestrictionToTmpDatabase();

        future.waitForFinished();

        return true;
    }
    else
    {
        return false;
    }
}

void DataPreprocessing::saveNodeToTmpDatabase()
{
    std::cerr << "Parsing Nodes..." << std::endl;
    _finalDBConnection->beginTransaction();
    _tmpDBConnection.beginTransaction();
    while(_nodeQueue.dequeue(_osmNode))
    {
        if(_tmpDBConnection.saveOSMNode(*_osmNode) == true)
        {
            std::cerr << "node saved to tmpdb" << std::endl;
        }
        else
        {
            std::cerr << "node NOT saved to tmpdb" << std::endl;
        }
                
        routingNode = boost::shared_ptr<RoutingNode>(new RoutingNode(_osmNode->getID(), _osmNode->getLat(), _osmNode->getLon()));
        //saveNodeToDatabase(*routingNode);
    }
    _finalDBConnection->endTransaction();
    _tmpDBConnection.endTransaction();
}

void DataPreprocessing::saveEdgeToTmpDatabase()
{
    std::cerr << "Parsing Ways..." << std::endl;
    _finalDBConnection->beginTransaction();
    _tmpDBConnection.beginTransaction();
    boost::uint64_t edgeID=0;
    //TODO: nochmal ueberlegen, ob if-Abfrage nicht sinnvoller als while-loop
    while(_wayQueue.dequeue(_osmWay))
    {
        //edges aus way extrahieren
        QVector<OSMEdge> edgeList = _osmWay->getEdgeList();
        for(int i = 0; i < edgeList.size(); i++)
        {
            if(_tmpDBConnection.saveOSMEdge(edgeList[i]) == true)
            {
                std::cerr << "edge saved" << std::endl;
            }
            else
            {
                std::cerr << "edge NOT saved" << std::endl;
            }
            routingEdge = boost::shared_ptr<RoutingEdge>(new RoutingEdge(edgeList[i].getID(), edgeList[i].getStartNode(), edgeList[i].getEndNode()));
            
            //TODO: Bevor in finale Datenbank gespeichert wird, Hier die Kategorisierung starten
            //categorizeEdge(*routingEdge);
            
            //speichert routingEdge in die finale Datenbank
            //_finalDBConnection->saveEdge(*routingEdge);
        }
    }
    _finalDBConnection->endTransaction();
    _tmpDBConnection.endTransaction();
}

void DataPreprocessing::saveTurnRestrictionToTmpDatabase()
{
    while(_turnRestrictionQueue.dequeue(_osmTurnRestriction))
    {
        //~ _tmpDBConnection.saveTurnRestrictionToTmpDatabase(*_turnRestrictionQueue);
    }
}

void DataPreprocessing::saveNodeToDatabase(const RoutingNode &node)
{
    if(_finalDBConnection->saveNode(node) == true)
    {
        std::cerr << "node saved to finalDB" << std::endl;
    }
    else
    {
        std::cerr << "node NOT saved finalDB" << std::endl;
    }
}

void DataPreprocessing::saveEdgeToDatabase(const RoutingEdge &edge)
{
    if(_finalDBConnection->saveEdge(edge))
    {
        std::cerr << "edge saved to finalDB" << std::endl;
    }
    else
    {
        std::cerr << "edge NOT saved to finalDB" << std::endl;
    }
}

//TODO kategorisierungsfunktion implementieren
//void DataPreprocessing::categorizeEdge(const RoutingEdge &edge)
//{
//    if(edge.hasStairs)
//    {
//    }
//    //...
    
//    //edge.getSurfaceQuality aufrufen und damit die Qualitaet festlegen
    
//    //noch zu klaeren, wie es im Detail läuft (wird ein laengerer if-else-zweig)
//}


//TODO kategorisierungsfunktionen implementieren
//boost::shared_ptr<RoutingEdge> DataPreprocessing::categorizeEdge(const OSMEdge &osmEdge) //sollte ich das hier als boost::shared_ptr<OSMEdge> bekommen?
//{
//    bool hasTrafficLights;
//    bool hasTrafficCalmingBumps;
//    bool hasStopSign;
//    bool hasStairs;
//    bool hasCycleBarrier;
//    boost::uint8_t streetType;
//    boost::uint8_t cyclewayType;
//    boost::uint8_t streetSurfaceType;
//    boost::uint8_t streetSurfaceQuality;
//    boost::uint8_t turnType;

//    routingEdge = boost::shared_ptr<RoutingEdge>(new RoutingEdge(osmEdge.getID(), osmEdge.getStartNode(), osmEdge.getEndNode()));

//    // Hier würden jetzt mit viel if und else, durch betrachtung der OSMPropertys der OSMEdge, die jeweiligen Werte der routingEdge gesetzt.
//    routingEdge->setTrafficLights(hasTrafficLights);
//    routingEdge->setTrafficCalmingBumps(hasTrafficCalmingBumps);
//    routingEdge->setStopSign(hasStopSign);
//    routingEdge->setStairs(hasStairs);
//    routingEdge->setCycleBarrier(hasCycleBarrier);
//    routingEdge->setStreetType(streetType);
//    routingEdge->setCyclewayType(cyclewayType);
//    routingEdge->setStreetSurfaceType(streetSurfaceType);
//    routingEdge->setStreetSurfaceQuality(streetSurfaceQuality);
//    routingEdge->setTurnType(turnType);

//    return routingEdge;
//}

//int DataPreprocessing::getStreetType();
//int DataPreprocessing::getStreetSurfaceQuality();
//int DataPreprocessing::getStreetSurfaceType();

namespace biker_tests
{    
    int testDataPreprocessing()
    {
        QFile file("rub.db");
        
        std::cerr << "Removing database test file \"rub.db\"..." << std::endl;
        if (file.exists())
            file.remove();
        
        boost::shared_ptr<SpatialiteDatabaseConnection> finalDB(new SpatialiteDatabaseConnection());
        DataPreprocessing dataPreprocessing(finalDB);
        dataPreprocessing.startparser("data/rub.osm", "rub.db");
        return EXIT_SUCCESS;
        //return EXIT_FAILURE;
    }
}
