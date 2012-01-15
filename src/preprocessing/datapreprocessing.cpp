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

void DataPreprocessing::saveNodeToDatabase(const RoutingNode &node)
{
    _finalDBConnection.saveNode(node);
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
    
    //~ OSMEdge newEdge(id, properties);    
    //~ 
    //~ if (!memberIDList.isEmpty()){
        //~ QVectorIterator<boost::uint64_t> i(memberIDList);
        //~ i.toFront();  // Iterator springt vor den ersten Eintrag in memberIDList
        //~ i.next();     // Iterator geht einen Schirtt weiter (gibt auch Wert zurück, der hier nicht verwendet wird)
        //~ while (i.hasNext()){
            //~ newEdge.setNodes(i.peekPrevious(), i.next());
            //~ edgeList << newEdge;
        //~ } 
    //~ }
    //~ 
    //~ return edgeList; 
    
}

void DataPreprocessing::saveTurnRestrictionToTmpDatabase()
{
    while(_turnRestrictionQueue.dequeue(_osmTurnRestriction))
    {
        //~ _tmpDBConnection.saveTurnRestrictionToTmpDatabase(*_turnRestrictionQueue);
    }
}



namespace biker_tests
{
    int testDataPreprocessing()
    {
        return EXIT_SUCCESS;
        //return EXIT_FAILURE;
    }
}
