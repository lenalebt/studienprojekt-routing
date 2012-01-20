#include "datapreprocessing.hpp" 


DataPreprocessing::DataPreprocessing()
    : _nodeQueue(1000), _wayQueue(1000), _turnRestrictionQueue(1000),
    parser(&_nodeQueue, &_wayQueue, &_turnRestrictionQueue)
{
    
}

//TODO: 1.Phase: OSMParser-Objekt fuellt Queues
//               Dann Queues auslesen und in tmp DB speichern
//      2.Phase: Kategorisieren
//
void DataPreprocessing::startparser(QString filename)
{
    parser.parse(filename);
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



namespace biker_tests
{    
    int testDataPreprocessing()
    {
        
        DataPreprocessing::DataPreprocessing()
        : _nodeQueue(1000), _wayQueue(1000), _turnRestrictionQueue(1000),
        parser(&_nodeQueue, &_wayQueue, &_turnRestrictionQueue)
        {
            
        }   
            
        boost::shared_ptr<OSMNode> _osmNode;
        boost::shared_ptr<OSMWay> _osmWay;
        boost::shared_ptr<OSMTurnRestriction> _osmTurnRestriction;
        
        boost::shared_ptr<RoutingNode> routingNode;
        boost::shared_ptr<RoutingEdge> routingEdge;

        TemporaryOSMDatabaseConnection _tmpDBConnection;
        SpatialiteDatabaseConnection _finalDBConnection;
        
        //~ BlockingQueue<boost::shared_ptr<OSMNode> > _nodeQueue;
        //~ BlockingQueue<boost::shared_ptr<OSMWay> > _wayQueue;
        //~ BlockingQueue<boost::shared_ptr<OSMTurnRestriction> > _turnRestrictionQueue;

        //~ BlockingQueue<boost::shared_ptr<OSMNode> > _nodeQueue;
        //~ BlockingQueue<boost::shared_ptr<OSMWay> > _wayQueue;
        //~ BlockingQueue<boost::shared_ptr<OSMTurnRestriction> > _turnRestrictionQueue;
                    
        //~ OSMParser parser(&_nodeQueue, &_wayQueue, &_turnRestrictionQueue);
                
        return EXIT_SUCCESS;
        //return EXIT_FAILURE;
    }
}
