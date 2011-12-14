#include "temporarydatabase.hpp"

#include <QFile>
#include <QStringList>

TemporaryOSMDatabaseConnection::TemporaryOSMDatabaseConnection() :
    _dbOpen(false), _db(NULL)
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

namespace biker_tests
{
    /**
     * @todo Test erweitern
     */
    int testTemporaryOSMDatabaseConnection()
    {
        TemporaryOSMDatabaseConnection connection;
        std::cout << "Opening \"testosm.db\"..." << std::endl;
        connection.open("testosm.db");
        CHECK(connection.isDBOpen());
        
        std::cout << "Closing database..." << std::endl;
        connection.close();
        CHECK(!connection.isDBOpen());
        
        std::cout << "Reopening \"test.db\"..." << std::endl;
        connection.open("testosm.db");
        CHECK(connection.isDBOpen());
        
        return EXIT_SUCCESS;
    }
}
