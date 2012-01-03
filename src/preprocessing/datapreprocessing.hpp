#ifndef DATAPREPROCESSING_HPP
#define DATAPREPROCESSING_HPP

#include "database.hpp"
#include "temporarydatabase.hpp"
#include "blockingqueue.hpp"
#include "routingedge.hpp"
#include "osmparser.hpp"
#include "tests.hpp"

namespace biker_tests
{
    int testDataPreprocessing();
}


/**
 * @brief Diese Klasse kuemmert sich um jegliche Form der Datenvorverarbeitung
 * 
 * @ingroup preprocessing
 * @author Sebastian Koehlert
 * @date 2011-12-21
 * @copyright GNU GPL v3
 * @todo
 */

class DataPreprocessing
{
	private:

	public:
    OSMParser _osmParser;
    
    DataPreprocessing();
    ~DataPreprocessing();
    
    bool deQueue();
    bool enQueue();
    bool saveNodeToTmpDatabase(const OSMNode& node);
    bool saveEdgeToTmpDatabase(const OSMEdge& edge);
    bool saveTurnRestrictionToTmpDatabase(const OSMTurnRestriction& turnRestriction);
	
	bool feedParser();	
};
#endif //DATAPREPROCESSING_HPP
