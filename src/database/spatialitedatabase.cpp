#include "spatialitedatabase.hpp"
#include <QStringList>
#include <QFile>

#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>

SpatialiteDatabaseConnection::SpatialiteDatabaseConnection() :
    _dbOpen(false), _db(NULL), _saveNodeStatement(NULL), _getNodeStatement(NULL),
    _saveEdgeStatement(NULL), _getEdgeStatementID(NULL), _getEdgeStatementStartNode(NULL),
    _getEdgeStatementEndNode(NULL)
{
    
}

void SpatialiteDatabaseConnection::close()
{
    sqlite3_close(_db);
    _dbOpen = false;
}

void SpatialiteDatabaseConnection::open(QString dbConnectionString)
{
    int rc; //return-Wert speichern
    QFile file(dbConnectionString);
    bool dbExisted = file.exists();
    
    rc = sqlite3_open_v2(dbConnectionString.toStdString().c_str(), &_db, 
		SQLITE_OPEN_READWRITE |
		SQLITE_OPEN_CREATE |
		SQLITE_OPEN_FULLMUTEX,
		NULL);
    
    if (rc != SQLITE_OK)
    {   //Es gab einen Fehler beim Öffnen der Datenbank.
        _dbOpen = false;
        sqlite3_close(_db);
        std::cerr << "Failed to open database file \"" << dbConnectionString.toStdString()
            << "\"" << std::endl;
        return;
    }
    
    //Zeiger auf die Fehlernachricht von SQLite. Speicher wird von Sqlite
    //selbst geholt und verwaltet, nur wieder freigeben ist nötig.
    char* errorMessage;
    
    //Bekommt den Dateinamen von Spatialite direkt von CMake :).
    std::string spatialiteFilename;
    spatialiteFilename = QUOTEME(SPATIALITE_LIB);
    
    //Erlaube das Laden von Erweiterungen
    rc = sqlite3_enable_load_extension(_db, 1);
    if (rc != SQLITE_OK)
    {
        _dbOpen = false;
        sqlite3_close(_db);
        std::cerr << "Failed to enable loading of sqlite3 extensions." << std::endl;
        return;
    }
    
    //Lade die Erweiterung
    rc = sqlite3_load_extension(_db, spatialiteFilename.c_str(), 0, &errorMessage);
    
    if (rc != SQLITE_OK)
    {
        _dbOpen = false;
        sqlite3_close(_db);
        std::cerr << "Failed to load spatialite. Filename: \"" << spatialiteFilename
            << ", Error message: \"" << errorMessage << "\"" << std::endl;
        sqlite3_free(errorMessage);
        return;
    }
    
    //Verbiete das laden von Erweiterungen wieder (Sicherheitsfeature?)
    rc = sqlite3_enable_load_extension(_db, 0);
    if (rc != SQLITE_OK)
    {
        _dbOpen = false;
        sqlite3_close(_db);
        std::cerr << "Failed to disable loading of sqlite3 extensions." << std::endl;
        return;
    }
    
    //Erstelle Tabellen nur, wenn die Datei vorher nicht existierte.
    //Grund: IF NOT EXISTS gibt es nicht für virtuelle Tabellen.
    if (!dbExisted)
        _dbOpen = createTables();
    else
        _dbOpen = true;
}

bool SpatialiteDatabaseConnection::createTables()
{
	bool retVal = true;
	
    //Liste von auszuführenden Statements erstellen
	QStringList statements;
	statements << "CREATE TABLE IF NOT EXISTS EDGES(ID INTEGER PRIMARY KEY, STARTNODE INTEGER NOT NULL, ENDNODE INTEGER NOT NULL, PROPERTIES INTEGER NOT NULL);";
	statements << "CREATE INDEX IF NOT EXISTS EDGES_STARTNODE_INDEX ON EDGES(STARTNODE);";
	statements << "CREATE INDEX IF NOT EXISTS EDGES_ENDNODE_INDEX ON EDGES(ENDNODE);";
	statements << "CREATE VIRTUAL TABLE NODES USING rtree(ID, MIN_LAT, MAX_LAT, MIN_LON, MAX_LON);";
	//TODO: Müssen noch Indicies erstellt werden? Laut Doku sollte es so schon schnell sein.
    statements << "CREATE TABLE EDGES_STREETNAME(ID INTEGER PRIMARY KEY, STREETNAME VARCHAR);";
    
    //Alle Statements der Liste ausführen
	QStringList::const_iterator it;
	for (it = statements.constBegin(); it != statements.constEnd(); it++)
	{
		retVal &= execCreateTableStatement(it->toStdString());
	}
	
	return retVal;
}

bool SpatialiteDatabaseConnection::execCreateTableStatement(std::string paramCreateTableStatement)
{
	char* errorMessage;
    //Wenn die Callback-Funktion NULL ist (3.Parameter) wird sie nicht aufgerufen.
	int rc = sqlite3_exec(_db, paramCreateTableStatement.c_str(), NULL, 0, &errorMessage);
	
	if (rc != SQLITE_OK)
    {
        sqlite3_close(_db);
        std::cerr << "Failed to create table. Statement: \"" << paramCreateTableStatement
            << "\", Error message: \"" << errorMessage << "\"" << std::endl;
        sqlite3_free(errorMessage);
        return false;
    }
    else
		return true;
}

bool SpatialiteDatabaseConnection::isDBOpen()
{
    return _dbOpen;
}


QVector<boost::shared_ptr<RoutingNode> >
SpatialiteDatabaseConnection::getNodes(const GPSPosition &searchMidpoint, double radius)
{
    return QVector<boost::shared_ptr<RoutingNode> >();
}


QVector<boost::shared_ptr<RoutingNode> >
SpatialiteDatabaseConnection::getNodes(const GPSPosition &minCorner, const GPSPosition &maxCorner)
{
    QVector<boost::shared_ptr<RoutingNode> > retList;
    
	int rc;
	if(_getNodeStatement == NULL)
	{		
		rc = sqlite3_prepare_v2(_db, "SELECT ID, MIN_LAT, MIN_LON FROM NODES WHERE MIN_LAT>=? AND MAX_LAT<=? AND MIN_LON>=? AND MAX_LON<=?;",
			-1, &_getNodeStatement, NULL);
		if (rc != SQLITE_OK)
		{	
			std::cerr << "Failed to create getNodeStatement." << " Resultcode: " << rc;
			return QVector<boost::shared_ptr<RoutingNode> >();
		}
	}
	
	// Parameter an das Statement binden
	sqlite3_bind_double(_getNodeStatement, 1, minCorner.getLat());
	sqlite3_bind_double(_getNodeStatement, 2, maxCorner.getLat());
	sqlite3_bind_double(_getNodeStatement, 3, minCorner.getLon());
	sqlite3_bind_double(_getNodeStatement, 4, maxCorner.getLon());
	
	// Statement ausfuehren, in einer Schleife immer neue Zeilen holen
	while ((rc = sqlite3_step(_getNodeStatement)) != SQLITE_DONE)
    {
        bool breakLoop = false;
        //Es können verschiedene Fehler aufgetreten sein.
        switch (rc)
        {
            case SQLITE_ROW:
                //noch eine Zeile verfügbar: Gut. Weitermachen.
                break;
            case SQLITE_ERROR:
                breakLoop=true;
                std::cerr << "SQL error or missing database." << " Resultcode: " << rc << std::endl;
                break;
            case SQLITE_BUSY:
                breakLoop=true;
                std::cerr << "The database file is locked." << " Resultcode: " << rc << std::endl;
                break;
            case SQLITE_LOCKED:
                breakLoop=true;
                std::cerr << "A table in the database is locked" << " Resultcode: " << rc << std::endl;
                break;
            default:
                breakLoop = true;
                std::cerr << "Unknown error. Resultcode:" << rc << std::endl;
        }
        if (breakLoop)
            break;
        
        
        //Erstelle einen neuen Knoten auf dem Heap.
        //Verwirrend: Hier ist der erste Parameter mit Index 0 und nicht 1 (!!).
        RoutingNode* newNode = new RoutingNode(sqlite3_column_int64(_getNodeStatement, 0),
                        sqlite3_column_double(_getNodeStatement, 1),
                        sqlite3_column_double(_getNodeStatement, 2));
        //Gib ihn an einen boost::shared_ptr weiter. newNode jetzt nicht mehr verwenden oder delete drauf anwenden!
        boost::shared_ptr<RoutingNode> ptr(newNode);
        //den boost::shared_ptr zur Liste hinzufügen
        retList << ptr;
    }
	
    if (rc != SQLITE_DONE)
	{	
		std::cerr << "Failed to execute getNodeStatement." << " Resultcode: " << rc;
		return QVector<boost::shared_ptr<RoutingNode> >();
	}
	
	
	rc = sqlite3_reset(_getNodeStatement);
	if(rc != SQLITE_OK)
	{
		std::cerr << "Failed to reset getNodeStatement." << " Resultcode: " << rc;
	}
	
    return retList;
}


bool SpatialiteDatabaseConnection::saveNode(const RoutingNode &node)
{
    int rc;
    if(_saveNodeStatement == NULL)
    {
        rc = sqlite3_prepare_v2(_db, "INSERT INTO NODES VALUES (@ID, @MIN_LAT, @MAX_LAT, @MIN_LON, @MAX_LON);", -1, &_saveNodeStatement, NULL);
        if (rc != SQLITE_OK)
        {	
            std::cerr << "Failed to create saveNodeStatement." << " Resultcode: " << rc;
            return false;
        }
    }

    // Parameter an das Statement binden
    sqlite3_bind_int64(_saveNodeStatement, 1, node.getID());
    sqlite3_bind_double(_saveNodeStatement, 2, node.getLat());
    sqlite3_bind_double(_saveNodeStatement, 3, node.getLat());
    sqlite3_bind_double(_saveNodeStatement, 4, node.getLon());
    sqlite3_bind_double(_saveNodeStatement, 5, node.getLon());

    // Statement ausfuehren
    rc = sqlite3_step(_saveNodeStatement);
    if (rc != SQLITE_DONE)
    {	
        std::cerr << "Failed to execute saveNodeStatement." << " Resultcode: " << rc;
        return false;
    }


    rc = sqlite3_reset(_saveNodeStatement);
    if(rc != SQLITE_OK)
    {
        std::cerr << "Failed to reset saveNodeStatement." << " Resultcode: " << rc;
    }
    return true;
}


QVector<boost::shared_ptr<RoutingEdge> >
SpatialiteDatabaseConnection::getEdgesByStartNodeID(boost::uint64_t startNodeID)
{
    QVector<boost::shared_ptr<RoutingEdge> > edgeList;
      
	int rc;
	if(_getEdgeStatementStartNode == NULL)
	{		
		rc = sqlite3_prepare_v2(_db, "SELECT ID, STARTNODE, ENDNODE, PROPERTIES FROM EDGES WHERE STARTNODE=?;",
			-1, &_getEdgeStatementStartNode, NULL);
		if (rc != SQLITE_OK)
		{	
			std::cerr << "Failed to create getEdgeStatement." << " Resultcode: " << rc;
			return QVector<boost::shared_ptr<RoutingEdge> >();
		}
	}
	
	// Parameter an das Statement binden
	sqlite3_bind_int64(_getEdgeStatementStartNode, 1, startNodeID);
	
	// Statement ausfuehren, in einer Schleife immer neue Zeilen holen
	while ((rc = sqlite3_step(_getEdgeStatementStartNode)) != SQLITE_DONE)
    {
        bool breakLoop = false;
        //Es können verschiedene Fehler aufgetreten sein.
        switch (rc)
        {
            case SQLITE_ROW:
                //noch eine Zeile verfügbar: Gut. Weitermachen.
                break;
            case SQLITE_ERROR:
                breakLoop=true;
                std::cerr << "SQL error or missing database." << " Resultcode: " << rc << std::endl;
                break;
            case SQLITE_BUSY:
                breakLoop=true;
                std::cerr << "The database file is locked." << " Resultcode: " << rc << std::endl;
                break;
            case SQLITE_LOCKED:
                breakLoop=true;
                std::cerr << "A table in the database is locked" << " Resultcode: " << rc << std::endl;
                break;
            default:
                breakLoop = true;
                std::cerr << "Unknown error. Resultcode:" << rc << std::endl;
        }
        if (breakLoop)
            break;
        
        
        //Erstelle einen neuen Knoten auf dem Heap.
        //Verwirrend: Hier ist der erste Parameter mit Index 0 und nicht 1 (!!).
        RoutingEdge* newEdge = new RoutingEdge(sqlite3_column_int64(_getEdgeStatementStartNode, 0),
                        sqlite3_column_int64(_getEdgeStatementStartNode, 1),
                        sqlite3_column_int64(_getEdgeStatementStartNode, 2),
                        sqlite3_column_int64(_getEdgeStatementStartNode, 3)
                        );
        //Gib ihn an einen boost::shared_ptr weiter. newNode jetzt nicht mehr verwenden oder delete drauf anwenden!
        boost::shared_ptr<RoutingEdge> ptr(newEdge);
        //den boost::shared_ptr zur Liste hinzufügen
		edgeList << ptr;
    }
	
    if (rc != SQLITE_DONE)
	{	
		std::cerr << "Failed to execute getEdgeStatement." << " Resultcode: " << rc;
		return QVector<boost::shared_ptr<RoutingEdge> >();
	}
	
	rc = sqlite3_reset(_getEdgeStatementStartNode);
	if(rc != SQLITE_OK)
	{
		std::cerr << "Failed to reset getEdgeStatement." << " Resultcode: " << rc;
	}
	
	return edgeList;
}


QVector<boost::shared_ptr<RoutingEdge> >
SpatialiteDatabaseConnection::getEdgesByEndNodeID(boost::uint64_t endNodeID)
{
        QVector<boost::shared_ptr<RoutingEdge> > edgeList;
      
	int rc;
	if(_getEdgeStatementEndNode == NULL)
	{		
		rc = sqlite3_prepare_v2(_db, "SELECT ID, STARTNODE, ENDNODE, PROPERTIES FROM EDGES WHERE ENDNODE=?;",
			-1, &_getEdgeStatementEndNode, NULL);
		if (rc != SQLITE_OK)
		{	
			std::cerr << "Failed to create getEdgeStatement." << " Resultcode: " << rc;
			return QVector<boost::shared_ptr<RoutingEdge> >();
		}
	}
	
	// Parameter an das Statement binden
	sqlite3_bind_int64(_getEdgeStatementEndNode, 1, endNodeID);
	
	// Statement ausfuehren, in einer Schleife immer neue Zeilen holen
	while ((rc = sqlite3_step(_getEdgeStatementEndNode)) != SQLITE_DONE)
    {
        bool breakLoop = false;
        //Es können verschiedene Fehler aufgetreten sein.
        switch (rc)
        {
            case SQLITE_ROW:
                //noch eine Zeile verfügbar: Gut. Weitermachen.
                break;
            case SQLITE_ERROR:
                breakLoop=true;
                std::cerr << "SQL error or missing database." << " Resultcode: " << rc << std::endl;
                break;
            case SQLITE_BUSY:
                breakLoop=true;
                std::cerr << "The database file is locked." << " Resultcode: " << rc << std::endl;
                break;
            case SQLITE_LOCKED:
                breakLoop=true;
                std::cerr << "A table in the database is locked" << " Resultcode: " << rc << std::endl;
                break;
            default:
                breakLoop = true;
                std::cerr << "Unknown error. Resultcode:" << rc << std::endl;
        }
        if (breakLoop)
            break;
        
        
        //Erstelle einen neuen Knoten auf dem Heap.
        //Verwirrend: Hier ist der erste Parameter mit Index 0 und nicht 1 (!!).
        RoutingEdge* newEdge = new RoutingEdge(sqlite3_column_int64(_getEdgeStatementEndNode, 0),
                        sqlite3_column_int64(_getEdgeStatementEndNode, 1),
                        sqlite3_column_int64(_getEdgeStatementEndNode, 2),
                        sqlite3_column_int64(_getEdgeStatementEndNode, 3)
                        );
        //Gib ihn an einen boost::shared_ptr weiter. newNode jetzt nicht mehr verwenden oder delete drauf anwenden!
        boost::shared_ptr<RoutingEdge> ptr(newEdge);
        //den boost::shared_ptr zur Liste hinzufügen
        edgeList << ptr;
    }
	
    if (rc != SQLITE_DONE)
	{	
		std::cerr << "Failed to execute getEdgeStatement." << " Resultcode: " << rc;
		return QVector<boost::shared_ptr<RoutingEdge> >();
	}
	
	rc = sqlite3_reset(_getEdgeStatementEndNode);
	if(rc != SQLITE_OK)
	{
		std::cerr << "Failed to reset getEdgeStatement." << " Resultcode: " << rc;
	}
	
	return edgeList;
}


boost::shared_ptr<RoutingEdge>
SpatialiteDatabaseConnection::getEdgeByEdgeID(boost::uint64_t edgeID)
{
	boost::shared_ptr<RoutingEdge> edge;
      
	int rc;
	if(_getEdgeStatementID == NULL)
	{		
		rc = sqlite3_prepare_v2(_db, "SELECT ID, STARTNODE, ENDNODE, PROPERTIES FROM EDGES WHERE ID=?;",
			-1, &_getEdgeStatementID, NULL);
		if (rc != SQLITE_OK)
		{	
			std::cerr << "Failed to create getEdgeStatement." << " Resultcode: " << rc;
			return boost::shared_ptr<RoutingEdge>();
		}
	}
	
	// Parameter an das Statement binden
	sqlite3_bind_int64(_getEdgeStatementID, 1, edgeID);
	
	// Statement ausfuehren, in einer Schleife immer neue Zeilen holen
	while ((rc = sqlite3_step(_getEdgeStatementID)) != SQLITE_DONE)
    {
        bool breakLoop = false;
        //Es können verschiedene Fehler aufgetreten sein.
        switch (rc)
        {
            case SQLITE_ROW:
                //noch eine Zeile verfügbar: Gut. Weitermachen.
                break;
            case SQLITE_ERROR:
                breakLoop=true;
                std::cerr << "SQL error or missing database." << " Resultcode: " << rc << std::endl;
                break;
            case SQLITE_BUSY:
                breakLoop=true;
                std::cerr << "The database file is locked." << " Resultcode: " << rc << std::endl;
                break;
            case SQLITE_LOCKED:
                breakLoop=true;
                std::cerr << "A table in the database is locked" << " Resultcode: " << rc << std::endl;
                break;
            default:
                breakLoop = true;
                std::cerr << "Unknown error. Resultcode:" << rc << std::endl;
        }
        if (breakLoop)
            break;
        
        
        //Erstelle einen neuen Knoten auf dem Heap.
        //Verwirrend: Hier ist der erste Parameter mit Index 0 und nicht 1 (!!).
        RoutingEdge* newEdge = new RoutingEdge(sqlite3_column_int64(_getEdgeStatementID, 0),
                        sqlite3_column_int64(_getEdgeStatementID, 1),
                        sqlite3_column_int64(_getEdgeStatementID, 2),
                        sqlite3_column_int64(_getEdgeStatementID, 3)
                        );
        //Gib ihn an einen boost::shared_ptr weiter. newNode jetzt nicht mehr verwenden oder delete drauf anwenden!
        edge.reset(newEdge);
    }
	
    if (rc != SQLITE_DONE)
	{	
		std::cerr << "Failed to execute getEdgeStatement." << " Resultcode: " << rc;
		return boost::shared_ptr<RoutingEdge>();
	}
	
	rc = sqlite3_reset(_getEdgeStatementID);
	if(rc != SQLITE_OK)
	{
		std::cerr << "Failed to reset getEdgeStatement." << " Resultcode: " << rc;
	}
	
	return edge;
}


bool SpatialiteDatabaseConnection::saveEdge(const RoutingEdge &edge)
{
    int rc;
    if(_saveEdgeStatement == NULL)
    {
        rc = sqlite3_prepare_v2(_db, "INSERT INTO EDGES VALUES (@ID, @STARTNODE, @ENDNODE, @PROPERTIES);", -1, &_saveEdgeStatement, NULL);
        if (rc != SQLITE_OK)
        {	
            std::cerr << "Failed to create saveEdgeStatement." << " Resultcode: " << rc;
            return false;
        }
    }

    // Parameter an das Statement binden
    sqlite3_bind_int64(_saveEdgeStatement, 1, edge.getID());
    sqlite3_bind_int64(_saveEdgeStatement, 2, edge.getStartNodeID());
    sqlite3_bind_int64(_saveEdgeStatement, 3, edge.getEndNodeID());
    sqlite3_bind_int64(_saveEdgeStatement, 4, edge.getProperties());
    
    // Statement ausfuehren
    rc = sqlite3_step(_saveEdgeStatement);
    if (rc != SQLITE_DONE)
    {	
        std::cerr << "Failed to execute saveEdgeStatement." << " Resultcode: " << rc;
        return false;
    }


    rc = sqlite3_reset(_saveEdgeStatement);
    if(rc != SQLITE_OK)
    {
        std::cerr << "Failed to reset saveEdgeStatement." << " Resultcode: " << rc;
    }
    return true;
}


bool SpatialiteDatabaseConnection::saveEdge(const RoutingEdge &edge, QString name)
{
    return saveEdge(edge);
    //TODO: Straßenname auch speichern.
}


QString SpatialiteDatabaseConnection::getStreetName(const RoutingEdge &edge)
{
    return "";
}

SpatialiteDatabaseConnection::~SpatialiteDatabaseConnection()
{
	if(_saveNodeStatement != NULL)
		sqlite3_finalize(_saveNodeStatement);
}

bool SpatialiteDatabaseConnection::beginTransaction()
{
    char* errorMessage;
    //Wenn die Callback-Funktion NULL ist (3.Parameter) wird sie nicht aufgerufen.
	int rc = sqlite3_exec(_db, "BEGIN TRANSACTION;", NULL, 0, &errorMessage);
	
	if (rc != SQLITE_OK)
    {
        sqlite3_close(_db);
        std::cerr << "Failed to begin transaction."
            << " Error message: \"" << errorMessage << "\"" << std::endl;
        sqlite3_free(errorMessage);
        return false;
    }
    else
		return true;
}
bool SpatialiteDatabaseConnection::endTransaction()
{
    char* errorMessage;
    //Wenn die Callback-Funktion NULL ist (3.Parameter) wird sie nicht aufgerufen.
	int rc = sqlite3_exec(_db, "END TRANSACTION;", NULL, 0, &errorMessage);
	
	if (rc != SQLITE_OK)
    {
        sqlite3_close(_db);
        std::cerr << "Failed to begin transaction."
            << " Error message: \"" << errorMessage << "\"" << std::endl;
        sqlite3_free(errorMessage);
        return false;
    }
    else
		return true;
}

namespace biker_tests
{
    int testSpatialiteDatabaseConnection()
    {
        SpatialiteDatabaseConnection connection;
        QFile file("test.db");
        
        std::cout << "Removing database test file \"test.db\"..." << std::endl;
        if (file.exists())
            file.remove();
        
        std::cout << "Opening \"test.db\"..." << std::endl;
        connection.open("test.db");
        CHECK(connection.isDBOpen());
        
        std::cout << "Closing database..." << std::endl;
        connection.close();
        CHECK(!connection.isDBOpen());
        
        std::cout << "Reopening \"test.db\"..." << std::endl;
        connection.open("test.db");
        CHECK(connection.isDBOpen());
        
        RoutingNode node(25, 51.0, 7.0);
        std::cout << "Save Node..." << std::endl;
        CHECK(connection.saveNode(node));
        node = RoutingNode(26, 51.5, 7.5);
        CHECK(connection.saveNode(node));
        
        RoutingEdge edge(45, 25, 26);
        std::cout << "Save Edge..." << std::endl;
        edge.setCycleBarrier(true);
        edge.setCyclewayType(5);
        CHECK(connection.saveEdge(edge));
        edge = RoutingEdge(46, 26, 25);
        CHECK(connection.saveEdge(edge));
        
        GPSPosition min(50.0, 6.0);
        GPSPosition max(52.0, 8.0);
        QVector<boost::shared_ptr<RoutingNode> > list = connection.getNodes(min, max);
        CHECK(!list.isEmpty());
        CHECK(list.size() == 2);
        
        std::cout << "Node 0 from DB: " << *(list[0]) << std::endl;
        std::cout << "Node 1 from DB: " << *(list[1]) << std::endl;
        CHECK((*list[0] == node) || (*list[1] == node));
        
        boost::minstd_rand generator(42u);
        boost::uniform_real<> uni_dist(50, 52);
        boost::variate_generator<boost::minstd_rand&, boost::uniform_real<> > uni(generator, uni_dist);
        
        std::cout << "Inserting 10000 Nodes within one transaction..." << std::endl;
        bool successInsertManyNodes = true;
        CHECK(connection.beginTransaction());
        for (int i=0; i<10000; i++)
        {
            node = RoutingNode(i + 100, uni(), uni() - (51.0 - 7.0));
            successInsertManyNodes = successInsertManyNodes && connection.saveNode(node);
        }
        CHECK(successInsertManyNodes);
        CHECK(connection.endTransaction());
        CHECK(!connection.saveNode(node));
        
        boost::shared_ptr<RoutingEdge> dbEdge(connection.getEdgeByEdgeID(46));
        CHECK_EQ(edge, *dbEdge);
        
        QVector<boost::shared_ptr<RoutingEdge> > edgeList;
        edgeList = connection.getEdgesByStartNodeID(26);
        CHECK_EQ(edge, *edgeList[0]);
        edgeList = connection.getEdgesByStartNodeID(26);
        CHECK_EQ(edge, *edgeList[0]);
        
        edgeList = connection.getEdgesByEndNodeID(25);
        CHECK_EQ(edge, *edgeList[0]);
        edgeList = connection.getEdgesByEndNodeID(25);
        CHECK_EQ(edge, *edgeList[0]);
        
        
        std::cout << "Inserting 10000 Edges within one transaction..." << std::endl;
        bool successInsertManyEdges = true;
        CHECK(connection.beginTransaction());
        for (int i=0; i<10000; i++)
        {
            edge = RoutingEdge(i + 100, i+99, i+100);
            successInsertManyEdges = successInsertManyEdges && connection.saveEdge(edge);
        }
        CHECK(successInsertManyEdges);
        CHECK(connection.endTransaction());
        CHECK(!connection.saveEdge(edge));
        
        return EXIT_SUCCESS;
    }
}
