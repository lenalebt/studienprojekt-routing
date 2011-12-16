#include "temporarydatabase.hpp"

#include <QFile>
#include <QStringList>

TemporaryOSMDatabaseConnection::TemporaryOSMDatabaseConnection() :
    _dbOpen(false), _db(NULL), _getLastInsertRowIDStatement(NULL),
    _saveOSMPropertyStatement(NULL), _getOSMPropertyStatement(NULL)
{
    
}

void TemporaryOSMDatabaseConnection::close()
{
    sqlite3_close(_db);
    _dbOpen = false;
}


/**
 * @todo Anpassen an die temporäre DB, ist noch fast 1:1 kopiert aus
 *      SpatialiteDatabaseConnection. Besonders: Erweiterungen (Spatialite)
 *      nötig? Wahrscheinlich nicht.
 */
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

/**
 * @todo Anpassen an die temporäre DB, ist noch fast 1:1 kopiert aus
 * SpatialiteDatabaseConnection
 */
bool TemporaryOSMDatabaseConnection::createTables()
{
	bool retVal = true;
	
    //Liste von auszuführenden Statements erstellen
	QStringList statements;
	statements << "CREATE TABLE IF NOT EXISTS PROPERTIES(PROPERTYID INTEGER PRIMARY KEY, KEY VARCHAR, VALUE VARCHAR);";
    
    statements << "CREATE TABLE IF NOT EXISTS NODES(ID INTEGER NOT NULL, LAT DOUBLE NOT NULL, LON DOUBLE NOT NULL);";
    statements << "CREATE TABLE IF NOT EXISTS NODEPROPERTYID(NODEID INTEGER, PROPERTYID INTEGER, PRIMARY KEY(NODEID, PROPERTYID));";
    
    statements << "CREATE TABLE IF NOT EXISTS EDGES(ID INTEGER PRIMARY KEY, STARTNODE INTEGER NOT NULL, ENDNODE INTEGER NOT NULL, WAYID INTEGER NOT NULL);";
    statements << "CREATE TABLE IF NOT EXISTS WAYPROPERTYID(WAYID INTEGER, PROPERTYID INTEGER, PRIMARY KEY(WAYID, PROPERTYID));";
    
    statements << "CREATE TABLE IF NOT EXISTS TURNRESTRICTIONS(VIAID INTEGER NOT NULL, TOID INTEGER NOT NULL, FROMID INTEGER NOT NULL, NOLEFT BOOLEAN, NORIGHT BOOLEAN, NOSTRAIGHT BOOLEAN, NOUTURN BOOLEAN);";
    
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
        CHECK(connection.endTransaction());
        
        property.setKey("key");
        CHECK_EQ(*connection.getOSMPropertyByID(1), property);
        property.setKey("key2");
        CHECK_EQ(*connection.getOSMPropertyByID(2), property);
        property.setKey("key3");
        CHECK_EQ(*connection.getOSMPropertyByID(3), property);
        property.setKey("key");
        CHECK(!(*connection.getOSMPropertyByID(3) == property));
        
        std::cout << "Closing database..." << std::endl;
        connection.close();
        CHECK(!connection.isDBOpen());
        
        return EXIT_SUCCESS;
    }
}
