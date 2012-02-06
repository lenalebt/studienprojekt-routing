#include "simpledatapreprocessing.hpp"
#include <QSet>


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

bool SimpleDataPreprocessing::isStreet(const OSMWay& way)
{
    QVector<OSMProperty> props = way.getProperties();
    for (QVector<OSMProperty>::const_iterator it = props.constBegin(); it != props.constEnd(); it++)
    {
        if (it->getKey() == "highway")
            return true;
    }
    return false;
}

bool SimpleDataPreprocessing::preprocess()
{
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
    
    
    
    std::cerr << "parsing ways..." << std::endl;
    _finalDBConnection->beginTransaction();
    boost::uint64_t edgeID=0;
    QSet<boost::uint64_t> nodeIDSet;
    int wayCount=0;
    while(_wayQueue.dequeue(_osmWay))
    {
        //edges aus way extrahieren
        if (isStreet(*_osmWay))
        {
            QVector<OSMEdge> edgeList = _osmWay->getEdgeList();
            for(int i = 0; i < edgeList.size(); i++)
            {
                RoutingEdge routingEdge(edgeID++, RoutingNode::convertIDToLongFormat(edgeList[i].getStartNode()), RoutingNode::convertIDToLongFormat(edgeList[i].getEndNode()));
                _finalDBConnection->saveEdge(routingEdge);
            }
            //So werden nur die Knoten in die DB gelegt, die auch von Edges benutzt werden.
            QVector<boost::uint64_t> memberList = _osmWay->getMemberList();
            for(int i = 0; i < memberList.size(); i++)
            {
                _osmNode = _tmpDBConnection.getOSMNodeByID(memberList[i]);
                //Das mit dem nodeIDSet mache ich, weil man der DB nicht sagen kann dass sie doppeltes Einfügen ignorieren soll.
                if (!nodeIDSet.contains(_osmNode->getID()))
                {
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
    
    std::cerr << "creating indexes..." << std::endl;
    _finalDBConnection->createIndexes();
    return true;
}

bool SimpleDataPreprocessing::preprocess(QString fileToParse, QString dbFilename)
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
        CHECK(dataPreprocessing.preprocess("data/rub.osm", "rub.db"));
        return EXIT_SUCCESS;
        //return EXIT_FAILURE;
    }
}
