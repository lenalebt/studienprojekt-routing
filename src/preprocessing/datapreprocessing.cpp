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
        
        routingNnode = boost::shared_ptr<RoutingNode>(new RoutingNode(*_osmNode.GetID(), _osmNode.GetLat(), _osmNode.GetLon()))
        
        //~ node = boost::shared_ptr<OSMNode>(new OSMNode(id, GPSPosition(lon, lat), QVector<OSMProperty>()));
        
        //~ _finalDBConnection.saveNode(*_osmNode);
}

void DataPreprocessing::saveEdgeToTmpDatabase()
{
    //edges sollten aus der way geliefert werden
    
    QVector<OSMEdge> edgeList;

        //~ QVector<OSMEdge> edgeList;
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
        //return EXIT_SUCCESS;
        return EXIT_FAILURE;
    }
}
