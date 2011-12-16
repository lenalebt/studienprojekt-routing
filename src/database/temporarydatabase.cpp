#include "temporarydatabase.hpp"

#include <QFile>
#include <QStringList>

TemporaryOSMDatabaseConnection::TemporaryOSMDatabaseConnection() :
    _dbOpen(false), _db(NULL), _getLastInsertRowIDStatement(NULL),
    _saveOSMPropertyStatement(NULL), _getOSMPropertyStatement(NULL),
    _saveOSMNodeStatement(NULL), _getOSMNodeStatement(NULL),
    _saveOSMNodePropertyStatement(NULL), _getOSMNodePropertyStatement(NULL),
    _saveOSMEdgeStatement(NULL), _getOSMEdgeStatement(NULL),
    _saveOSMEdgePropertyStatement(NULL), _getOSMEdgePropertyStatement(NULL),
    _saveOSMRelationStatement(NULL), _getOSMRelationStatement(NULL)
{
    
}

TemporaryOSMDatabaseConnection::~TemporaryOSMDatabaseConnection()
{
    //Prepared Statements löschen
    if(_getLastInsertRowIDStatement != NULL)
		sqlite3_finalize(_getLastInsertRowIDStatement);
    if(_saveOSMPropertyStatement != NULL)
		sqlite3_finalize(_saveOSMPropertyStatement);
    if(_getOSMPropertyStatement != NULL)
		sqlite3_finalize(_getOSMPropertyStatement);
    if(_saveOSMNodeStatement != NULL)
		sqlite3_finalize(_saveOSMNodeStatement);
    if(_getOSMNodeStatement != NULL)
		sqlite3_finalize(_getOSMNodeStatement);
    if(_saveOSMNodePropertyStatement != NULL)
		sqlite3_finalize(_saveOSMNodePropertyStatement);
    if(_getOSMNodePropertyStatement != NULL)
		sqlite3_finalize(_getOSMNodePropertyStatement);
    if(_saveOSMEdgeStatement != NULL)
		sqlite3_finalize(_saveOSMEdgeStatement);
    if(_getOSMEdgeStatement != NULL)
		sqlite3_finalize(_getOSMEdgeStatement);
    if(_saveOSMEdgePropertyStatement != NULL)
		sqlite3_finalize(_saveOSMEdgePropertyStatement);
    if(_getOSMEdgePropertyStatement != NULL)
		sqlite3_finalize(_getOSMEdgePropertyStatement);
    if(_saveOSMRelationStatement != NULL)
		sqlite3_finalize(_saveOSMRelationStatement);
    if(_getOSMRelationStatement != NULL)
		sqlite3_finalize(_getOSMRelationStatement);
    
    if (_dbOpen)
        this->close();
}

void TemporaryOSMDatabaseConnection::close()
{
    sqlite3_close(_db);
    _dbOpen = false;
}


void TemporaryOSMDatabaseConnection::open(QString dbConnectionString)
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

bool TemporaryOSMDatabaseConnection::isDBOpen()
{
    return _dbOpen;
}

bool TemporaryOSMDatabaseConnection::createTables()
{
	bool retVal = true;
	
    //Liste von auszuführenden Statements erstellen
	QStringList statements;
	statements << "CREATE TABLE IF NOT EXISTS PROPERTIES(PROPERTYID INTEGER PRIMARY KEY, KEY VARCHAR, VALUE VARCHAR);";
    
    statements << "CREATE TABLE IF NOT EXISTS NODES(ID INTEGER PRIMARY KEY, LAT DOUBLE NOT NULL, LON DOUBLE NOT NULL);";
    statements << "CREATE TABLE IF NOT EXISTS NODEPROPERTYID(NODEID INTEGER, PROPERTYID INTEGER, PRIMARY KEY(NODEID, PROPERTYID));";
    
    statements << "CREATE TABLE IF NOT EXISTS EDGES(ID INTEGER PRIMARY KEY, STARTNODE INTEGER NOT NULL, ENDNODE INTEGER NOT NULL, WAYID INTEGER NOT NULL);";
    statements << "CREATE TABLE IF NOT EXISTS WAYPROPERTYID(WAYID INTEGER, PROPERTYID INTEGER, PRIMARY KEY(WAYID, PROPERTYID));";
    
    statements << "CREATE TABLE IF NOT EXISTS TURNRESTRICTIONS(VIAID INTEGER NOT NULL, TOID INTEGER NOT NULL, FROMID INTEGER NOT NULL, NOLEFT BOOLEAN, NORIGHT BOOLEAN, NOSTRAIGHT BOOLEAN, NOUTURN BOOLEAN, PRIMARY KEY(VIAID, TOID, FROMID));";
    
    //Alle Statements der Liste ausführen
	QStringList::const_iterator it;
	for (it = statements.constBegin(); it != statements.constEnd(); it++)
	{
		retVal &= execCreateTableStatement(it->toStdString());
	}
	
	return retVal;
}

bool TemporaryOSMDatabaseConnection::execCreateTableStatement(std::string paramCreateTableStatement)
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

bool TemporaryOSMDatabaseConnection::beginTransaction()
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
bool TemporaryOSMDatabaseConnection::endTransaction()
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

boost::uint64_t TemporaryOSMDatabaseConnection::saveOSMProperty(const OSMProperty& property)
{
    int rc;
    if(_saveOSMPropertyStatement == NULL)
    {
        rc = sqlite3_prepare_v2(_db, "INSERT INTO PROPERTIES VALUES (@ID, @KEY, @VALUE);", -1, &_saveOSMPropertyStatement, NULL);
        if (rc != SQLITE_OK)
        {	
            std::cerr << "Failed to create saveOSMPropertyStatement." << " Resultcode: " << rc;
            return 0;
        }
    }

    // Parameter an das Statement binden. Bei NULL beim Primary Key wird automatisch inkrementiert
    sqlite3_bind_null(_saveOSMPropertyStatement, 1);
    sqlite3_bind_text(_saveOSMPropertyStatement, 2, property.getKey().toLatin1(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(_saveOSMPropertyStatement, 3, property.getValue().toLatin1(), -1, SQLITE_TRANSIENT);
    
    // Statement ausfuehren
    rc = sqlite3_step(_saveOSMPropertyStatement);
    if (rc != SQLITE_DONE)
    {	
        std::cerr << "Failed to execute saveOSMPropertyStatement." << " Resultcode: " << rc;
        return 0;
    }


    rc = sqlite3_reset(_saveOSMPropertyStatement);
    if(rc != SQLITE_OK)
    {
        std::cerr << "Failed to reset saveOSMPropertyStatement." << " Resultcode: " << rc;
    }
    
    return getLastInsertRowID();
}

boost::shared_ptr<OSMProperty> TemporaryOSMDatabaseConnection::getOSMPropertyByID(boost::uint64_t propertyID)
{
    boost::shared_ptr<OSMProperty> property;
      
    int rc;
    if(_getOSMPropertyStatement == NULL)
    {		
        rc = sqlite3_prepare_v2(_db, "SELECT KEY, VALUE FROM PROPERTIES WHERE PROPERTYID=?;",
            -1, &_getOSMPropertyStatement, NULL);
        if (rc != SQLITE_OK)
        {	
            std::cerr << "Failed to create getOSMPropertyStatement." << " Resultcode: " << rc;
            return boost::shared_ptr<OSMProperty>();
        }
    }

    // Parameter an das Statement binden
    sqlite3_bind_int64(_getOSMPropertyStatement, 1, propertyID);

    // Statement ausfuehren, in einer Schleife immer neue Zeilen holen
    while ((rc = sqlite3_step(_getOSMPropertyStatement)) != SQLITE_DONE)
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
        
        
        //Verwirrend: Hier ist der erste Parameter mit Index 0 und nicht 1 (!!).
        OSMProperty* newproperty = new OSMProperty(
                        QString(reinterpret_cast<const char*>(sqlite3_column_text(_getOSMPropertyStatement, 0))),
                        QString(reinterpret_cast<const char*>(sqlite3_column_text(_getOSMPropertyStatement, 1)))
                        );
        //Gib ihn an einen boost::shared_ptr weiter. newNode jetzt nicht mehr verwenden oder delete drauf anwenden!
        property.reset(newproperty);
    }

    if (rc != SQLITE_DONE)
    {	
        std::cerr << "Failed to execute getOSMPropertyStatement." << " Resultcode: " << rc;
        return boost::shared_ptr<OSMProperty>();
    }

    rc = sqlite3_reset(_getOSMPropertyStatement);
    if(rc != SQLITE_OK)
    {
        std::cerr << "Failed to reset getOSMPropertyStatement." << " Resultcode: " << rc;
    }

    return property;
}


boost::uint64_t TemporaryOSMDatabaseConnection::getLastInsertRowID()
{
    boost::uint64_t retVal=0;
      
    int rc;
    if(_getLastInsertRowIDStatement == NULL)
    {		
        rc = sqlite3_prepare_v2(_db, "SELECT last_insert_rowid();",
            -1, &_getLastInsertRowIDStatement, NULL);
        if (rc != SQLITE_OK)
        {	
            std::cerr << "Failed to create getLastInsertRowIDStatement." << " Resultcode: " << rc;
            return 0;
        }
    }

    // Statement ausfuehren, in einer Schleife immer neue Zeilen holen
    while ((rc = sqlite3_step(_getLastInsertRowIDStatement)) != SQLITE_DONE)
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
        
        
        //Verwirrend: Hier ist der erste Parameter mit Index 0 und nicht 1 (!!).
        retVal = sqlite3_column_int64(_getLastInsertRowIDStatement, 0);
    }

    if (rc != SQLITE_DONE)
    {	
        std::cerr << "Failed to execute getLastInsertRowIDStatement." << " Resultcode: " << rc;
        return 0;
    }

    rc = sqlite3_reset(_getLastInsertRowIDStatement);
    if(rc != SQLITE_OK)
    {
        std::cerr << "Failed to reset getLastInsertRowIDStatement." << " Resultcode: " << rc;
    }

    return retVal;
}

bool TemporaryOSMDatabaseConnection::saveOSMNode(const OSMNode& node)
{
    QVector<OSMProperty> properties = node.getProperties();
    
    int rc;
    if(_saveOSMNodeStatement == NULL)
    {
        rc = sqlite3_prepare_v2(_db, "INSERT INTO NODES VALUES (@ID, @LAT, @LON);", -1, &_saveOSMNodeStatement, NULL);
        if (rc != SQLITE_OK)
        {	
            std::cerr << "Failed to create saveOSMNodeStatement." << " Resultcode: " << rc;
            return false;
        }
    }

    // Parameter an das Statement binden. Bei NULL beim Primary Key wird automatisch inkrementiert
    sqlite3_bind_int64(_saveOSMNodeStatement, 1, node.getID());
    sqlite3_bind_double(_saveOSMNodeStatement, 2, node.getLat());
    sqlite3_bind_double(_saveOSMNodeStatement, 3, node.getLon());
    
    // Statement ausfuehren
    rc = sqlite3_step(_saveOSMNodeStatement);
    if (rc != SQLITE_DONE)
    {	
        std::cerr << "Failed to execute saveOSMNodeStatement." << " Resultcode: " << rc;
        return false;
    }

    rc = sqlite3_reset(_saveOSMNodeStatement);
    if(rc != SQLITE_OK)
    {
        std::cerr << "Failed to reset saveOSMNodeStatement." << " Resultcode: " << rc;
    }
    
    //Properties speichern. Erstmal Statement zum Verbinden von Node und Property anlegen...
    if(_saveOSMNodePropertyStatement == NULL)
    {
        rc = sqlite3_prepare_v2(_db, "INSERT INTO NODEPROPERTYID VALUES (@NODEID, @PROPERTYID);", -1, &_saveOSMNodePropertyStatement, NULL);
        if (rc != SQLITE_OK)
        {	
            std::cerr << "Failed to create saveOSMNodePropertyStatement." << " Resultcode: " << rc;
            return false;
        }
    }
    //Dieses wird nämlich später benötigt und es wäre blöd, das in der Schleife abzufragen.
    
    for (QVector<OSMProperty>::const_iterator it = properties.constBegin(); it != properties.constEnd(); it++)
    {
        //erstmal Property speichern.
        boost::uint64_t propertyID = saveOSMProperty(*it);
        
        //Parameter binden
        sqlite3_bind_int64(_saveOSMNodePropertyStatement, 1, node.getID());
        sqlite3_bind_int64(_saveOSMNodePropertyStatement, 2, propertyID);
        
        //Statement ausfuehren
        rc = sqlite3_step(_saveOSMNodePropertyStatement);
        if (rc != SQLITE_DONE)
        {	
            std::cerr << "Failed to execute saveOSMNodePropertyStatement." << " Resultcode: " << rc;
            return false;
        }
        
        //Statement resetten
        rc = sqlite3_reset(_saveOSMNodePropertyStatement);
        if(rc != SQLITE_OK)
        {
            std::cerr << "Failed to reset saveOSMNodePropertyStatement." << " Resultcode: " << rc;
        }
    }
    return true;
}

boost::shared_ptr<OSMNode> TemporaryOSMDatabaseConnection::getOSMNodeByID(boost::uint64_t nodeID)
{
    boost::shared_ptr<OSMNode> node;
      
    int rc;
    if(_getOSMNodeStatement == NULL)
    {		
        rc = sqlite3_prepare_v2(_db, "SELECT LAT, LON FROM NODES WHERE ID=?;",
            -1, &_getOSMNodeStatement, NULL);
        if (rc != SQLITE_OK)
        {	
            std::cerr << "Failed to create getOSMNodeStatement." << " Resultcode: " << rc;
            return boost::shared_ptr<OSMNode>();
        }
    }

    // Parameter an das Statement binden
    sqlite3_bind_int64(_getOSMNodeStatement, 1, nodeID);

    // Statement ausfuehren, in einer Schleife immer neue Zeilen holen
    while ((rc = sqlite3_step(_getOSMNodeStatement)) != SQLITE_DONE)
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
        
        
        //Verwirrend: Hier ist der erste Parameter mit Index 0 und nicht 1 (!!).
        OSMNode* newNode = new OSMNode(nodeID, GPSPosition(
                        sqlite3_column_double(_getOSMNodeStatement, 0),
                        sqlite3_column_double(_getOSMNodeStatement, 1))
                        );
        //Gib ihn an einen boost::shared_ptr weiter. newNode jetzt nicht mehr verwenden oder delete drauf anwenden!
        node.reset(newNode);
    }

    if (rc != SQLITE_DONE)
    {
        std::cerr << "Failed to execute getOSMNodeStatement." << " Resultcode: " << rc;
        return boost::shared_ptr<OSMNode>();
    }

    rc = sqlite3_reset(_getOSMNodeStatement);
    if(rc != SQLITE_OK)
    {
        std::cerr << "Failed to reset getOSMNodeStatement." << " Resultcode: " << rc;
    }
    
    //Bis hier sind die Grundeigenschaften des Knotens geladen. Es fehlen die Attribute.
    //Properties laden
    if(_getOSMNodePropertyStatement == NULL)
    {		
        rc = sqlite3_prepare_v2(_db, "SELECT PROPERTYID FROM NODEPROPERTYID WHERE NODEID=?;",
            -1, &_getOSMNodePropertyStatement, NULL);
        if (rc != SQLITE_OK)
        {	
            std::cerr << "Failed to create getOSMNodePropertyStatement." << " Resultcode: " << rc;
            return boost::shared_ptr<OSMNode>();
        }
    }

    // Parameter an das Statement binden
    sqlite3_bind_int64(_getOSMNodePropertyStatement, 1, nodeID);
    
    QVector<boost::uint64_t> propertyIDs;

    // Statement ausfuehren, in einer Schleife immer neue Zeilen holen
    while ((rc = sqlite3_step(_getOSMNodePropertyStatement)) != SQLITE_DONE)
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
        
        
        //Verwirrend: Hier ist der erste Parameter mit Index 0 und nicht 1 (!!).
        boost::uint64_t propertyID = sqlite3_column_int64(_getOSMNodePropertyStatement, 0);
        propertyIDs << propertyID;
    }

    if (rc != SQLITE_DONE)
    {
        std::cerr << "Failed to execute getOSMNodePropertyStatement." << " Resultcode: " << rc;
        return boost::shared_ptr<OSMNode>();
    }

    rc = sqlite3_reset(_getOSMNodePropertyStatement);
    if(rc != SQLITE_OK)
    {
        std::cerr << "Failed to reset getOSMNodePropertyStatement." << " Resultcode: " << rc;
    }
    //Bis hier: Liste mit Eigenschaften-IDs laden.
    
    //Eigenschaften selbst laden
    for (QVector<boost::uint64_t>::const_iterator it = propertyIDs.constBegin(); it != propertyIDs.constEnd(); it++)
    {
        node->addProperty(*getOSMPropertyByID(*it));
    }
    
    return node;
}


bool TemporaryOSMDatabaseConnection::saveOSMEdge(const OSMEdge& edge)
{
    return false;
}


bool TemporaryOSMDatabaseConnection::saveOSMTurnRestriction(const OSMRelation& relation)
{
    int rc;
    if(_saveOSMRelationStatement == NULL)
    {
        rc = sqlite3_prepare_v2(_db, "INSERT INTO TURNRESTRICITIONS VALUES (@VIAID, @TOID, @FROMID, @NOLEFT, @NORIGHT, @NOSTRAIGHT, @NOUTURN);", -1, &_saveOSMRelationStatement, NULL);
        if (rc != SQLITE_OK)
        {	
            std::cerr << "Failed to create saveOSMRelationStatement." << " Resultcode: " << rc;
            return false;
        }
    }

    // Parameter an das Statement binden. Bei NULL beim Primary Key wird automatisch inkrementiert
    sqlite3_bind_int64(_saveOSMRelationStatement, 1, relation.getViaId());
    sqlite3_bind_int64(_saveOSMRelationStatement, 2, relation.getToId());
    sqlite3_bind_int64(_saveOSMRelationStatement, 3, relation.getFromId());
    sqlite3_bind_int64(_saveOSMRelationStatement, 4, relation.getLeft());
    sqlite3_bind_int64(_saveOSMRelationStatement, 5, relation.getRight());
    sqlite3_bind_int64(_saveOSMRelationStatement, 6, relation.getStraight());
    sqlite3_bind_int64(_saveOSMRelationStatement, 7, relation.getUTurn());
    
    // Statement ausfuehren
    rc = sqlite3_step(_saveOSMRelationStatement);
    if (rc != SQLITE_DONE)
    {	
        std::cerr << "Failed to execute saveOSMRelationStatement." << " Resultcode: " << rc;
        return false;
    }


    rc = sqlite3_reset(_saveOSMRelationStatement);
    if(rc != SQLITE_OK)
    {
        std::cerr << "Failed to reset saveOSMRelationStatement." << " Resultcode: " << rc;
    }
    
    return true;
}
















namespace biker_tests
{
    /**
     * @todo Test erweitern
     */
    int testTemporaryOSMDatabaseConnection()
    {
        TemporaryOSMDatabaseConnection connection;
        
        QFile file("testosm.db");
        
        std::cout << "Removing database test file \"testosm.db\"..." << std::endl;
        if (file.exists())
            file.remove();
        
        std::cout << "Opening \"testosm.db\"..." << std::endl;
        connection.open("testosm.db");
        CHECK(connection.isDBOpen());
        
        std::cout << "Closing database..." << std::endl;
        connection.close();
        CHECK(!connection.isDBOpen());
        
        std::cout << "Reopening \"test.db\"..." << std::endl;
        connection.open("testosm.db");
        CHECK(connection.isDBOpen());
        
        std::cout << "Checking OSMProperty..." << std::endl;
        CHECK(connection.beginTransaction());
        OSMProperty property("key", "value");
        CHECK_EQ_TYPE(connection.saveOSMProperty(property), 1, boost::uint64_t);
        property.setKey("key2");
        CHECK_EQ_TYPE(connection.saveOSMProperty(property), 2, boost::uint64_t);
        property.setKey("key3");
        CHECK_EQ_TYPE(connection.saveOSMProperty(property), 3, boost::uint64_t);
        CHECK_EQ_TYPE(connection.saveOSMProperty(property), 4, boost::uint64_t);
        CHECK(connection.endTransaction());
        
        property.setKey("key");
        CHECK_EQ(*connection.getOSMPropertyByID(1), property);
        property.setKey("key2");
        CHECK_EQ(*connection.getOSMPropertyByID(2), property);
        property.setKey("key3");
        CHECK_EQ(*connection.getOSMPropertyByID(3), property);
        property.setKey("key");
        CHECK(!(*connection.getOSMPropertyByID(3) == property));
        
        std::cout << "Checking OSMNode..." << std::endl;
        OSMProperty property1("key1", "value1");
        OSMProperty property2("key2", "value2");
        OSMProperty property3("key3", "value3");
        OSMNode node(10, GPSPosition(51.0, 7.0));
        node.addProperty(property1);
        node.addProperty(property2);
        node.addProperty(property3);
        CHECK(connection.saveOSMNode(node));
        
        CHECK_EQ(*connection.getOSMNodeByID(10), node);
        
        std::cout << "Closing database..." << std::endl;
        connection.close();
        CHECK(!connection.isDBOpen());
        
        return EXIT_SUCCESS;
    }
}
