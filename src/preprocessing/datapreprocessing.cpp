#include "datapreprocessing.hpp" 


DataPreprocessing::DataPreprocessing()
    : _nodeQueue(1000), _wayQueue(1000), _turnRestrictionQueue(1000),
    osmParser(&_nodeQueue, &_wayQueue, &_turnRestrictionQueue),
    pbfParser(&_nodeQueue, &_wayQueue, &_turnRestrictionQueue)
{
    
}

DataPreprocessing::~DataPreprocessing()
{
    
}

//TODO: 1.Phase: OSMParser-Objekt fuellt Queues
//               Dann Queues auslesen und in tmp DB speichern
//      2.Phase: Kategorisieren
//
bool DataPreprocessing::startparser(QString fileToParse, QString dbFilename)
{
    _finalDBConnection.open(dbFilename);

    //Prueft, ob .osm oder .pbf am Ende vorhanden
    if(fileToParse.endsWith(".osm"))
    {
        QFuture<bool> future = QtConcurrent::run(&osmParser, &OSMParser::parse, fileToParse);
        return true;
    }
    else if (fileToParse.endsWith(".pbf"))
    {
        QFuture<bool> future = QtConcurrent::run(&pbfParser, &PBFParser::parse, fileToParse);
        return true;
    }
    else
    {
        return false;
    }
}

void DataPreprocessing::saveNodeToTmpDatabase()
{    
    while(_nodeQueue.dequeue(_osmNode))
    {
        _tmpDBConnection.saveOSMNode(*_osmNode);        
        routingNode = boost::shared_ptr<RoutingNode>(new RoutingNode(_osmNode->getID(), _osmNode->getLat(), _osmNode->getLon()));        
        //~ _finalDBConnection.saveNode(*routingNnode);
        saveNodeToDatabase(*routingNode);
    }
}

void DataPreprocessing::saveEdgeToTmpDatabase()
{
    while(_wayQueue.dequeue(_osmWay))
    {
        //edges aus way extrahieren
        QVector<OSMEdge> edgeList = _osmWay->getEdgeList();
        for(int i = 0; i < edgeList.size(); i++)
        {
            _tmpDBConnection.saveOSMEdge(edgeList[i]);
            routingEdge = boost::shared_ptr<RoutingEdge>(new RoutingEdge(edgeList[i].getID(), edgeList[i].getStartNode(), edgeList[i].getEndNode()));
            _finalDBConnection.saveEdge(*routingEdge);
        }
    }
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
    _finalDBConnection.saveNode(node);
}

void DataPreprocessing::saveEdgeToDatabase(const RoutingEdge &edge)
{
    _finalDBConnection.saveEdge(edge);
}

//TODO kategorisierungsfunktionen implementieren

namespace biker_tests
{    
    int testDataPreprocessing()
    {        
        return EXIT_SUCCESS;
        //return EXIT_FAILURE;
    }
}