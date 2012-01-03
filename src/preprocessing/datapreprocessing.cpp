#include "datapreprocessing.hpp" 

//TODO: 1.Phase: OSMParser-Objekt fuellt Queues
//               Dann Queues auslesen und in tmp DB speichern
//      2.Phase: Kategorisieren


bool DataPreprocessing::enQueue()
{
    //TODO: Parser fuellt Queue
   

    _osmParser.startDocument();
    //OSMParser::operator <<
    return true;
}

bool DataPreprocessing::deQueue()
{
    //TODO: Elemente aus Queue entfernen und entsprechend dem Element in temp DB speichern
    /*     
        while(!queue.IsEmpty)
        {
            * queue.Dequeue()
        } 
     */ 
    
    return true;
}

bool DataPreprocessing::saveNodeToTmpDatabase(const OSMNode& node)
{
    if(_tmpDBConnection.isDBOpen())
    {
        _tmpDBConnection.saveOSMNode(node);
    }
    else
    {
        _tmpDBConnection.open("test");
        _tmpDBConnection.saveOSMNode(node);
    }

    return true;
}

bool DataPreprocessing::saveEdgeToTmpDatabase(const OSMEdge& edge)
{
    if(_tmpDBConnection.isDBOpen())
    {
        _tmpDBConnection.saveOSMEdge(edge);
    }
    else
    {
        _tmpDBConnection.open("test");
        _tmpDBConnection.saveOSMEdge(edge);
    }
    
    return true;
}

bool DataPreprocessing::saveTurnRestrictionToTmpDatabase(const OSMTurnRestriction& turnRestriction)
{
    if(_tmpDBConnection.isDBOpen())
    {
        _tmpDBConnection.saveOSMTurnRestriction(turnRestriction);
    }
    else
    {
        _tmpDBConnection.open("test");
        _tmpDBConnection.saveOSMTurnRestriction(turnRestriction);
    }

    return true;
}

namespace biker_tests
{
    int testDataPreprocessing()
    {
        //return EXIT_SUCCESS;
        return EXIT_FAILURE;
    }
}
