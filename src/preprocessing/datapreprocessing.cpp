#include "datapreprocessing.hpp" 
#include <QSet>


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
        _finalDBConnection->close();
        _tmpDBConnection.close();
        tmpFile.remove();
        return true;
    }
    else
    {
        return false;
    }
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
                //TODO: kategorisieren
                categorizeEdge(edgeList[i]);
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
    
    //Die Queues müssen alle geleert werden, sonst kann das Programm nicht beendet werden!
    while (_turnRestrictionQueue.dequeue(_osmTurnRestriction))
    {
        
    }
    
    //Am Schluss noch Indexe erstellen
    std::cerr << "creating indexes..." << std::endl;
    _finalDBConnection->createIndexes();
    return true;
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
boost::shared_ptr<RoutingEdge> DataPreprocessing::categorizeEdge(const OSMEdge &osmEdge) //sollte ich das hier als boost::shared_ptr<OSMEdge> bekommen?
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

    // Hier würden jetzt mit viel if und else, durch betrachtung der OSMPropertys der OSMEdge, die jeweiligen Werte der routingEdge gesetzt.
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

        if(_value == "impassable")
        {
            streetSurfaceQuality = STREETSURFACEQUALITY_IMPASSABLE;
            break;
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
            else if (_value == "cobblestone")
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
            else if (_value == "gravel")
            {
                streetSurfaceType = STREETSURFACETYPE_GRAVEL;
            }
            else if (_value == "ground" || _value == "dirt" || _value == "mud" || _value == "earth" || _value == "clay" || _value == "sand")
            {
                streetSurfaceType = STREETSURFACETYPE_GROUND;
            }
            else if (_value ==)
            {
            }
        }
    }
    
    routingEdge->setTrafficLights(hasTrafficLights);
    routingEdge->setTrafficCalmingBumps(hasTrafficCalmingBumps);
    routingEdge->setStopSign(hasStopSign);
    routingEdge->setStairs(hasStairs);
    routingEdge->setCycleBarrier(hasCycleBarrier);
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
