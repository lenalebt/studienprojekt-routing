#include "spatialitedatabase.hpp"
#include <QStringList>
#include <QFile>

SpatialiteDatabaseConnection::SpatialiteDatabaseConnection() :
    _dbOpen(false), _db(NULL), _saveNodeStatement(NULL)
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
	sqlite3_bind_double(_getNodeStatement, 1, node.getLat());
	sqlite3_bind_double(_getNodeStatement, 2, node.getLat());
	sqlite3_bind_double(_getNodeStatement, 3, node.getLon());
	sqlite3_bind_double(_getNodeStatement, 4, node.getLon());
	
	// Statement ausfuehren
	rc = sqlite3_step(_getNodeStatement);
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
	
    return QVector<boost::shared_ptr<RoutingNode> >();
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
    return QVector<boost::shared_ptr<RoutingEdge> >();
}


QVector<boost::shared_ptr<RoutingEdge> >
SpatialiteDatabaseConnection::getEdgesByEndNodeID(boost::uint64_t endNodeID)
{
    return QVector<boost::shared_ptr<RoutingEdge> >();
}


boost::shared_ptr<RoutingEdge>
SpatialiteDatabaseConnection::getEdgesByEdgeID(boost::uint64_t edgeID)
{
    return boost::shared_ptr<RoutingEdge>(new RoutingEdge());
}


bool SpatialiteDatabaseConnection::saveEdge(const RoutingEdge &edge)
{
    //TODO
    return false;
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
        std::cout << "Save Node \"test.db\"..." << std::endl;
        CHECK(connection.saveNode(node));
        
        return EXIT_SUCCESS;
    }
}
