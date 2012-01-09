#ifndef DATAPREPROCESSING_HPP
#define DATAPREPROCESSING_HPP

#include "database.hpp"
#include "temporarydatabase.hpp"
#include "blockingqueue.hpp"
#include "routingedge.hpp"
#include "routingnode.hpp"
#include "osmparser.hpp"
#include "osmway.hpp"
#include "osmnode.hpp"
#include "osmedge.hpp"
#include "osmturnrestriction.hpp"
#include "tests.hpp"
#include <QtConcurrentRun>

#include <QDebug>
#include <QThread>
#include <QString>
#include <qtconcurrentrun.h>
#include <QApplication>

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
    DataPreprocessing();
    ~DataPreprocessing();
    
    TemporaryOSMDatabaseConnection _tmpDBConnection;
    
    BlockingQueue<OSMNode> _nodeQueue;
    BlockingQueue<OSMEdge> _edgeQueue;
    BlockingQueue<OSMTurnRestriction> _turnRestrictionQueue;
    BlockingQueue<OSMWay> _wayQueue;
    
    OSMParser parser(*_wayQueue);
    
    void Startparser(QString filename);    
    bool deQueue();
    bool enQueue();
    bool saveNodeToTmpDatabase(const OSMNode& node);
    bool saveEdgeToTmpDatabase(const OSMEdge& edge);
    bool saveTurnRestrictionToTmpDatabase(const OSMTurnRestriction& turnRestriction);
};
#endif //DATAPREPROCESSING_HPP
