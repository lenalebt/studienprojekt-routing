#ifdef SPATIALITE_FOUND

#include "spatialitedatabase.hpp"
#include <QStringList>
#include <QFile>
#include <sqlite_functions.hpp>

#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>

SpatialiteDatabaseConnection::SpatialiteDatabaseConnection() :
    _dbOpen(false), _db(NULL), _saveNodeStatement(NULL), _getNodeStatement(NULL),
    _getNodeByIDStatement(NULL),
    _saveEdgeStatement(NULL), _getEdgeStatementID(NULL), _getEdgeStatementStartNode(NULL),
    _getEdgeStatementEndNode(NULL), 
    _saveEdgeStreetnameStatement(NULL),
    _getEdgeStreetnameStatement(NULL),
    _deleteEdgeStatement(NULL)
{
    
}

SpatialiteDatabaseConnection::~SpatialiteDatabaseConnection()
{
    //Prepared Statements löschen
	if(_saveNodeStatement != NULL)
		sqlite3_finalize(_saveNodeStatement);
    if(_getNodeStatement != NULL)
		sqlite3_finalize(_getNodeStatement);
    if(_getNodeByIDStatement != NULL)
		sqlite3_finalize(_getNodeByIDStatement);
    if(_saveEdgeStatement != NULL)
		sqlite3_finalize(_saveEdgeStatement);
    if(_getEdgeStatementID != NULL)
		sqlite3_finalize(_getEdgeStatementID);
    if(_getEdgeStatementStartNode != NULL)
		sqlite3_finalize(_getEdgeStatementStartNode);
    if(_getEdgeStatementEndNode != NULL)
		sqlite3_finalize(_getEdgeStatementEndNode);
    if(_saveEdgeStreetnameStatement != NULL)
		sqlite3_finalize(_saveEdgeStreetnameStatement);
    if(_getEdgeStreetnameStatement != NULL)
		sqlite3_finalize(_getEdgeStreetnameStatement);
    if(_deleteEdgeStatement != NULL)
		sqlite3_finalize(_deleteEdgeStatement);
    
    if (_dbOpen)
        this->close();
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
    statements << "PRAGMA page_size = 4096;";
    statements << "PRAGMA max_page_count = 2147483646;";
    statements << "PRAGMA cache_size=10000;";
    statements << "PRAGMA synchronous=OFF;";
    statements << "PRAGMA journal_mode=MEMORY;";
    statements << "PRAGMA temp_store = MEMORY;";
	statements << "CREATE TABLE IF NOT EXISTS EDGES(ID INTEGER PRIMARY KEY, STARTNODE INTEGER NOT NULL, ENDNODE INTEGER NOT NULL, PROPERTIES INTEGER NOT NULL);";
	statements << "CREATE VIRTUAL TABLE NODES USING rtree(ID, MIN_LAT, MAX_LAT, MIN_LON, MAX_LON);";
	//TODO: Müssen noch Indicies erstellt werden? Laut Doku sollte es so schon schnell sein.
    statements << "CREATE TABLE EDGES_STREETNAME(ID INTEGER PRIMARY KEY, STREETNAME VARCHAR);";
    
    //Alle Statements der Liste ausführen in einer Transaktion
    //retVal = this->beginTransaction();
	QStringList::const_iterator it;
	for (it = statements.constBegin(); it != statements.constEnd(); it++)
	{
		retVal &= execCreateTableStatement(it->toStdString());
	}
    //retVal &= this->endTransaction();
	
	return retVal;
}

bool SpatialiteDatabaseConnection::createIndexes()
{
	bool retVal = true;
	
    //Liste von auszuführenden Statements erstellen
	QStringList statements;
	statements << "CREATE INDEX IF NOT EXISTS EDGES_STARTNODE_INDEX ON EDGES(STARTNODE);";
	statements << "CREATE INDEX IF NOT EXISTS EDGES_ENDNODE_INDEX ON EDGES(ENDNODE);";
    
    //Alle Statements der Liste ausführen in einer Transaktion
    retVal = this->beginTransaction();
	QStringList::const_iterator it;
	for (it = statements.constBegin(); it != statements.constEnd(); it++)
	{
		retVal &= execCreateTableStatement(it->toStdString());
	}
    retVal &= this->endTransaction();
	
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

boost::shared_ptr<RoutingNode> SpatialiteDatabaseConnection::getNodeByID(boost::uint64_t id)
{
    boost::shared_ptr<RoutingNode> retVal;
    
	int rc;
	if(_getNodeByIDStatement == NULL)
	{		
		rc = sqlite3_prepare_v2(_db, "SELECT ID, MIN_LAT, MIN_LON FROM NODES WHERE ID=?;",
			-1, &_getNodeByIDStatement, NULL);
		if (rc != SQLITE_OK)
		{	
			std::cerr << "Failed to create getNodeByIDStatement." << " Resultcode: " << rc << std::endl;
			return boost::shared_ptr<RoutingNode>();
		}
	}
	
	// Parameter an das Statement binden
    RoutingNode node;
    node.setID(id);
    if (node.isIDInLongFormat())
    {
        sqlite3_bind_int64(_getNodeByIDStatement, 1, RoutingNode::convertIDToShortFormat(id));
    }
    else
    {
        sqlite3_bind_int64(_getNodeByIDStatement, 1, id);
    }
	
	// Statement ausfuehren, in einer Schleife immer neue Zeilen holen
	while ((rc = sqlite3_step(_getNodeByIDStatement)) != SQLITE_DONE)
    {
        //Es können verschiedene Fehler aufgetreten sein.
        if (!sqlite_functions::handleSQLiteResultcode(rc))
            break;
        
        //Erstelle einen neuen Knoten auf dem Heap.
        //Verwirrend: Hier ist der erste Parameter mit Index 0 und nicht 1 (!!).
        RoutingNode* newNode = new RoutingNode(sqlite3_column_int64(_getNodeByIDStatement, 0),
                        sqlite3_column_double(_getNodeByIDStatement, 1),
                        sqlite3_column_double(_getNodeByIDStatement, 2));
        //Gib ihn an einen boost::shared_ptr weiter. newNode jetzt nicht mehr verwenden oder delete drauf anwenden!
        boost::shared_ptr<RoutingNode> ptr(newNode);
        //den boost::shared_ptr zur Liste hinzufügen
        retVal = ptr;
    }
	
    if (rc != SQLITE_DONE)
	{	
		std::cerr << "Failed to execute getNodeByIDStatement." << " Resultcode: " << rc << std::endl;
		return boost::shared_ptr<RoutingNode>();
	}
	
	
	rc = sqlite3_reset(_getNodeByIDStatement);
	if(rc != SQLITE_OK)
	{
		std::cerr << "Failed to reset getNodeByIDStatement." << " Resultcode: " << rc << std::endl;
	}
	
    return retVal;
}

QVector<boost::shared_ptr<RoutingNode> >
SpatialiteDatabaseConnection::getNodes(const GPSPosition &searchMidpoint, double radius)
{
    //Berechne einfach die beiden Punkte, die in den Ecken des umgebenden
    //Rechtecks sein müssten, plus ein bisschen.
    return this->getNodes(searchMidpoint.calcPositionInDistance(180+45, radius*1.45),
        searchMidpoint.calcPositionInDistance(45, radius*1.45));
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
			std::cerr << "Failed to create getNodeStatement." << " Resultcode: " << rc << std::endl;
			return QVector<boost::shared_ptr<RoutingNode> >();
		}
	}
	
	// Parameter an das Statement binden
	sqlite3_bind_double(_getNodeStatement, 1, minCorner.getLat());
	sqlite3_bind_double(_getNodeStatement, 2, maxCorner.getLat());
	sqlite3_bind_double(_getNodeStatement, 3, minCorner.getLon());
	sqlite3_bind_double(_getNodeStatement, 4, maxCorner.getLon());
	
    /*std::cerr << "bound parameters:" << "minCorner.getLat()=" << minCorner.getLat() <<
        ", maxCorner.getLat()=" << maxCorner.getLat() <<
        ", minCorner.getLon()=" << minCorner.getLon() <<
        ", maxCorner.getLon()=" << maxCorner.getLon() << std::endl;*/
    
	// Statement ausfuehren, in einer Schleife immer neue Zeilen holen
	while ((rc = sqlite3_step(_getNodeStatement)) != SQLITE_DONE)
    {
        //Es können verschiedene Fehler aufgetreten sein.
        if (!sqlite_functions::handleSQLiteResultcode(rc))
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
		std::cerr << "Failed to execute getNodeStatement." << " Resultcode: " << rc << std::endl;
		return QVector<boost::shared_ptr<RoutingNode> >();
	}
	
	
	rc = sqlite3_reset(_getNodeStatement);
	if(rc != SQLITE_OK)
	{
		std::cerr << "Failed to reset getNodeStatement." << " Resultcode: " << rc << std::endl;
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
            std::cerr << "Failed to create saveNodeStatement." << " Resultcode: " << rc << std::endl;
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
        std::cerr << "Failed to execute saveNodeStatement." << " Resultcode: " << rc << std::endl;
        return false;
    }


    rc = sqlite3_reset(_saveNodeStatement);
    if(rc != SQLITE_OK)
    {
        std::cerr << "Failed to reset saveNodeStatement." << " Resultcode: " << rc << std::endl;
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
			std::cerr << "Failed to create getEdgeStatement." << " Resultcode: " << rc << std::endl;
			return QVector<boost::shared_ptr<RoutingEdge> >();
		}
	}
	
	// Parameter an das Statement binden
	sqlite3_bind_int64(_getEdgeStatementStartNode, 1, startNodeID);
	
	// Statement ausfuehren, in einer Schleife immer neue Zeilen holen
	while ((rc = sqlite3_step(_getEdgeStatementStartNode)) != SQLITE_DONE)
    {
        //Es können verschiedene Fehler aufgetreten sein.
        if (!sqlite_functions::handleSQLiteResultcode(rc))
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
		std::cerr << "Failed to execute getEdgeStatement." << " Resultcode: " << rc << std::endl;
		return QVector<boost::shared_ptr<RoutingEdge> >();
	}
	
	rc = sqlite3_reset(_getEdgeStatementStartNode);
	if(rc != SQLITE_OK)
	{
		std::cerr << "Failed to reset getEdgeStatement." << " Resultcode: " << rc << std::endl;
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
			std::cerr << "Failed to create getEdgeStatement." << " Resultcode: " << rc << std::endl;
			return QVector<boost::shared_ptr<RoutingEdge> >();
		}
	}
	
	// Parameter an das Statement binden
	sqlite3_bind_int64(_getEdgeStatementEndNode, 1, endNodeID);
	
	// Statement ausfuehren, in einer Schleife immer neue Zeilen holen
	while ((rc = sqlite3_step(_getEdgeStatementEndNode)) != SQLITE_DONE)
    {
        //Es können verschiedene Fehler aufgetreten sein.
        if (!sqlite_functions::handleSQLiteResultcode(rc))
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
		std::cerr << "Failed to execute getEdgeStatement." << " Resultcode: " << rc << std::endl;
		return QVector<boost::shared_ptr<RoutingEdge> >();
	}
	
	rc = sqlite3_reset(_getEdgeStatementEndNode);
	if(rc != SQLITE_OK)
	{
		std::cerr << "Failed to reset getEdgeStatement." << " Resultcode: " << rc << std::endl;
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
			std::cerr << "Failed to create getEdgeStatement." << " Resultcode: " << rc << std::endl;
			return boost::shared_ptr<RoutingEdge>();
		}
	}
	
	// Parameter an das Statement binden
	sqlite3_bind_int64(_getEdgeStatementID, 1, edgeID);
	
	// Statement ausfuehren, in einer Schleife immer neue Zeilen holen
	while ((rc = sqlite3_step(_getEdgeStatementID)) != SQLITE_DONE)
    {
        //Es können verschiedene Fehler aufgetreten sein.
        if (!sqlite_functions::handleSQLiteResultcode(rc))
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
		std::cerr << "Failed to execute getEdgeStatement." << " Resultcode: " << rc << std::endl;
		return boost::shared_ptr<RoutingEdge>();
	}
	
	rc = sqlite3_reset(_getEdgeStatementID);
	if(rc != SQLITE_OK)
	{
		std::cerr << "Failed to reset getEdgeStatement." << " Resultcode: " << rc << std::endl;
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
            std::cerr << "Failed to create saveEdgeStatement." << " Resultcode: " << rc << std::endl;
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
        std::cerr << "Failed to execute saveEdgeStatement." << " Resultcode: " << rc << std::endl;
        return false;
    }

    rc = sqlite3_reset(_saveEdgeStatement);
    if(rc != SQLITE_OK)
    {
        std::cerr << "Failed to reset saveEdgeStatement." << " Resultcode: " << rc << std::endl;
    }
    return true;
}


bool SpatialiteDatabaseConnection::saveEdge(const RoutingEdge &edge, const QString& name)
{
    if (!saveEdge(edge))
        return false;
    
    int rc;
    if(_saveEdgeStreetnameStatement == NULL)
    {
        rc = sqlite3_prepare_v2(_db, "INSERT OR IGNORE INTO EDGES_STREETNAME VALUES (@ID, @STREETNAME);", -1, &_saveEdgeStreetnameStatement, NULL);
        if (rc != SQLITE_OK)
        {	
            std::cerr << "Failed to create saveEdgeStreetnameStatement." << " Resultcode: " << rc << std::endl;
            return false;
        }
    }

    // Parameter an das Statement binden
    sqlite3_bind_int64(_saveEdgeStreetnameStatement, 1, edge.getID());
    sqlite3_bind_text(_saveEdgeStreetnameStatement, 2, name.toLatin1(), -1, SQLITE_TRANSIENT);
    
    // Statement ausfuehren
    rc = sqlite3_step(_saveEdgeStreetnameStatement);
    if (rc != SQLITE_DONE)
    {	
        std::cerr << "Failed to execute saveEdgeStreetnameStatement." << " Resultcode: " << rc << std::endl;
        return false;
    }

    rc = sqlite3_reset(_saveEdgeStreetnameStatement);
    if(rc != SQLITE_OK)
    {
        std::cerr << "Failed to reset saveEdgeStreetnameStatement." << " Resultcode: " << rc << std::endl;
    }
    return true;
}

bool SpatialiteDatabaseConnection::deleteEdge(boost::uint64_t startNodeID, boost::uint64_t endNodeID)
{
    int rc;
    if(_deleteEdgeStatement == NULL)
    {
        rc = sqlite3_prepare_v2(_db, "DELETE FROM EDGES WHERE STARTNODE=@STARTNODE AND ENDNODE=@ENDNODE;", -1, &_deleteEdgeStatement, NULL);
        if (rc != SQLITE_OK)
        {	
            std::cerr << "Failed to create deleteEdgeStatement." << " Resultcode: " << rc << std::endl;
            return false;
        }
    }
    
    // Parameter an das Statement binden
    sqlite3_bind_int64(_deleteEdgeStatement, 1, startNodeID);
    sqlite3_bind_int64(_deleteEdgeStatement, 2, endNodeID);
    
    // Statement ausfuehren
    rc = sqlite3_step(_deleteEdgeStatement);
    if (rc != SQLITE_DONE)
    {	
        std::cerr << "Failed to execute deleteEdgeStatement." << " Resultcode: " << rc << std::endl;
        return false;
    }

    rc = sqlite3_reset(_deleteEdgeStatement);
    if(rc != SQLITE_OK)
    {
        std::cerr << "Failed to reset deleteEdgeStatement." << " Resultcode: " << rc << std::endl;
    }
    return true;
}

QString SpatialiteDatabaseConnection::getStreetName(const RoutingEdge &edge)
{
    QString streetname = "";
      
    int rc;
    if(_getEdgeStreetnameStatement == NULL)
    {		
        rc = sqlite3_prepare_v2(_db, "SELECT STREETNAME FROM EDGES_STREETNAME WHERE ID=?;",
            -1, &_getEdgeStreetnameStatement, NULL);
        if (rc != SQLITE_OK)
        {	
            std::cerr << "Failed to create getEdgeStreetnameStatement." << " Resultcode: " << rc << std::endl;
            return "";
        }
    }

    // Parameter an das Statement binden
    sqlite3_bind_int64(_getEdgeStreetnameStatement, 1, edge.getID());

    // Statement ausfuehren, in einer Schleife immer neue Zeilen holen
    while ((rc = sqlite3_step(_getEdgeStreetnameStatement)) != SQLITE_DONE)
    {
        //Es können verschiedene Fehler aufgetreten sein.
        if (!sqlite_functions::handleSQLiteResultcode(rc))
            break;
        
        //Verwirrend: Hier ist der erste Parameter mit Index 0 und nicht 1 (!!).
        streetname = QString(reinterpret_cast<const char*>(sqlite3_column_text(_getEdgeStreetnameStatement, 0)));
    }

    if (rc != SQLITE_DONE)
    {	
        std::cerr << "Failed to execute getEdgeStreetnameStatement." << " Resultcode: " << rc << std::endl;
        return "";
    }

    rc = sqlite3_reset(_getEdgeStreetnameStatement);
    if(rc != SQLITE_OK)
    {
        std::cerr << "Failed to reset getEdgeStreetnameStatement." << " Resultcode: " << rc << std::endl;
    }

    return streetname;
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
        QFile file("spatialitetest.db");
        
        std::cerr << "Removing database test file \"spatialitetest.db\"..." << std::endl;
        if (file.exists())
            file.remove();
        
        std::cerr << "Opening \"spatialitetest.db\"..." << std::endl;
        connection.open("spatialitetest.db");
        CHECK(connection.isDBOpen());
        
        std::cerr << "Closing database..." << std::endl;
        connection.close();
        CHECK(!connection.isDBOpen());
        
        std::cerr << "Reopening \"spatialitetest.db\"..." << std::endl;
        connection.open("spatialitetest.db");
        CHECK(connection.isDBOpen());
        
        RoutingNode node(25, 51.0, 7.0);
        std::cerr << "Save Node..." << std::endl;
        CHECK(connection.saveNode(node));
        node = RoutingNode(26, 51.5, 7.5);
        CHECK(connection.saveNode(node));
        CHECK(*connection.getNodeByID(26) == node);
        CHECK(*connection.getNodeByID(RoutingNode::convertIDToLongFormat(26)) == node);
        
        RoutingEdge edge(45, 25, 26);
        std::cerr << "Save Edge..." << std::endl;
        edge.setCycleBarrier(true);
        edge.setCyclewayType(5);
        CHECK(connection.saveEdge(edge, "Teststraße"));
        CHECK_EQ_TYPE(connection.getStreetName(edge), "Teststraße", QString);
        edge = RoutingEdge(46, 26, 25);
        CHECK(connection.saveEdge(edge));
        
        GPSPosition min(50.0, 6.0);
        GPSPosition max(52.0, 8.0);
        QVector<boost::shared_ptr<RoutingNode> > list = connection.getNodes(min, max);
        CHECK(!list.isEmpty());
        CHECK(list.size() == 2);
        
        std::cerr << "Node 0 from DB: " << *(list[0]) << std::endl;
        std::cerr << "Node 1 from DB: " << *(list[1]) << std::endl;
        CHECK((*list[0] == node) || (*list[1] == node));
        
        boost::minstd_rand generator(42u);
        boost::uniform_real<> uni_dist(50, 52);
        boost::variate_generator<boost::minstd_rand&, boost::uniform_real<> > uni(generator, uni_dist);
        
        std::cerr << "Inserting 10000 Nodes within one transaction..." << std::endl;
        bool successInsertManyNodes = true;
        CHECK(connection.beginTransaction());
        for (int i=0; i<10000; i++)
        {
            node = RoutingNode(i + 100, uni(), uni() - (51.0 - 7.0));
            successInsertManyNodes = successInsertManyNodes && connection.saveNode(node);
        }
        CHECK(successInsertManyNodes);
        CHECK(connection.endTransaction());
        std::cerr << "Hier erwartet: Resultcode 19 (-> Constraint failed)" << std::endl;
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
        
        
        std::cerr << "Inserting 10000 Edges within one transaction..." << std::endl;
        bool successInsertManyEdges = true;
        CHECK(connection.beginTransaction());
        for (int i=0; i<10000; i++)
        {
            edge = RoutingEdge(i + 100, i+99, i+100);
            successInsertManyEdges = successInsertManyEdges && connection.saveEdge(edge);
        }
        CHECK(successInsertManyEdges);
        CHECK(connection.endTransaction());
        std::cerr << "Hier erwartet: Resultcode 19 (-> Constraint failed)" << std::endl;
        CHECK(!connection.saveEdge(edge));
        
        
        edgeList = connection.getEdgesByStartNodeID(99);
        CHECK(!edgeList.isEmpty());
        edgeList = connection.getEdgesByStartNodeID(100);
        CHECK(!edgeList.isEmpty());
        
        CHECK(connection.beginTransaction());
        CHECK(connection.deleteEdge(99, 100));
        CHECK(connection.deleteEdge(100, 101));
        CHECK(connection.endTransaction());
        
        edgeList = connection.getEdgesByStartNodeID(99);
        CHECK(edgeList.isEmpty());
        edgeList = connection.getEdgesByStartNodeID(100);
        CHECK(edgeList.isEmpty());
        
        return EXIT_SUCCESS;
    }
}

#endif //SPATIALITE_FOUND
