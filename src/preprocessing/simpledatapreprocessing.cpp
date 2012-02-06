#include "simpledatapreprocessing.hpp" 


SimpleDataPreprocessing::SimpleDataPreprocessing(boost::shared_ptr<DatabaseConnection> finaldb)
    : _nodeQueue(1000), _wayQueue(1000), _turnRestrictionQueue(1000),
    _osmParser(),
    _pbfParser(),
      _finalDBConnection(finaldb)
{
    
}

SimpleDataPreprocessing::~SimpleDataPreprocessing()
{
    
}

bool SimpleDataPreprocessing::preprocess()
{
    std::cerr << "Parsing Nodes..." << std::endl;
    _finalDBConnection->beginTransaction();
    int nodeCount=0;
    while(_nodeQueue.dequeue(_osmNode))
    {
        routingNode = boost::shared_ptr<RoutingNode>(new RoutingNode(_osmNode->getID(), _osmNode->getLat(), _osmNode->getLon()));
        _finalDBConnection->saveNode(*routingNode);
        
        if (++nodeCount == 100000)
        {
            nodeCount = 0;
            _finalDBConnection->endTransaction();
            _finalDBConnection->beginTransaction();
        }
    }
    _finalDBConnection->endTransaction();
    
    
    
    std::cerr << "Parsing Ways..." << std::endl;
    _finalDBConnection->beginTransaction();
    boost::uint64_t edgeID=0;
    int wayCount=0;
    while(_wayQueue.dequeue(_osmWay))
    {
        //edges aus way extrahieren
        QVector<OSMEdge> edgeList = _osmWay->getEdgeList();
        for(int i = 0; i < edgeList.size(); i++)
        {
            routingEdge = boost::shared_ptr<RoutingEdge>(new RoutingEdge(edgeID++, RoutingNode::convertIDToLongFormat(edgeList[i].getStartNode()), RoutingNode::convertIDToLongFormat(edgeList[i].getEndNode())));
            _finalDBConnection->saveEdge(*routingEdge);
        }
        if (++wayCount == 100000)
        {
            wayCount = 0;
            _finalDBConnection->endTransaction();
            _finalDBConnection->beginTransaction();
        }
    }
    _finalDBConnection->endTransaction();
    
    
    _finalDBConnection->createIndexes();
    return true;
}

bool SimpleDataPreprocessing::preprocess(QString fileToParse, QString dbFilename)
{
    _finalDBConnection->open(dbFilename);
    
    //Prueft, ob .osm oder .pbf am Ende vorhanden
    if(fileToParse.endsWith(".osm"))
    {
        _osmParser.reset(new OSMParser(&_nodeQueue, &_wayQueue, &_turnRestrictionQueue));
        QFuture<bool> future = QtConcurrent::run(_osmParser.get(), &OSMParser::parse, fileToParse);
        
        bool preprocessRetval = preprocess();
        future.waitForFinished();
        return (preprocessRetval && future.result());
    }
    else if (fileToParse.endsWith(".pbf"))
    {
        _pbfParser.reset(new PBFParser(&_nodeQueue, &_wayQueue, &_turnRestrictionQueue));
        QFuture<bool> future = QtConcurrent::run(_pbfParser.get(), &PBFParser::parse, fileToParse);
        
        bool preprocessRetval = preprocess();
        future.waitForFinished();
        return (preprocessRetval && future.result());
    }
    else
    {
        return false;
    }
}

//TODO kategorisierungsfunktionen implementieren

namespace biker_tests
{    
    int testSimpleDataPreprocessing()
    {
        QFile file("rub.db");
        
        std::cerr << "Removing database test file \"rub.db\"..." << std::endl;
        if (file.exists())
            file.remove();
        
        boost::shared_ptr<SpatialiteDatabaseConnection> finalDB(new SpatialiteDatabaseConnection());
        SimpleDataPreprocessing dataPreprocessing(finalDB);
        dataPreprocessing.preprocess("data/rub.osm", "rub.db");
        return EXIT_SUCCESS;
        //return EXIT_FAILURE;
    }
}
