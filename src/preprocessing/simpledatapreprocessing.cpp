#include "simpledatapreprocessing.hpp"
#include <QSet>


SimpleDataPreprocessing::SimpleDataPreprocessing(boost::shared_ptr<DatabaseConnection> finaldb)
    : _nodeQueue(10000), _wayQueue(10000), _turnRestrictionQueue(10000),
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
    //Wenn ein "highway"-key gefunden wird, ist es eine Straße - sonst nicht.
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
