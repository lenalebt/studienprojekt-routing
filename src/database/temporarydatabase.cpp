#include "temporarydatabase.hpp"

#include <QFile>
#include <QStringList>

TemporaryOSMDatabaseConnection::TemporaryOSMDatabaseConnection() :
    _dbOpen(false), _db(NULL), _getLastInsertRowIDStatement(NULL),
    _saveOSMPropertyStatement(NULL), _getOSMPropertyStatement(NULL),
    _saveOSMNodeStatement(NULL), _getOSMNodeByIDStatement(NULL),
    _saveOSMNodePropertyStatement(NULL), _getOSMNodePropertyStatement(NULL),
    _saveOSMEdgeStatement(NULL), _getOSMEdgeStatement(NULL),
    _saveOSMEdgePropertyStatement(NULL), _getOSMEdgePropertyStatement(NULL),
    _saveOSMTurnRestrictionStatement(NULL), _getOSMTurnRestrictionByViaIDStatement(NULL)
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
    if(_getOSMNodeByIDStatement != NULL)
		sqlite3_finalize(_getOSMNodeByIDStatement);
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
    if(_saveOSMTurnRestrictionStatement != NULL)
		sqlite3_finalize(_saveOSMTurnRestrictionStatement);
    if(_getOSMTurnRestrictionByViaIDStatement != NULL)
		sqlite3_finalize(_getOSMTurnRestrictionByViaIDStatement);
    
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
    
    statements << "CREATE TABLE IF NOT EXISTS TURNRESTRICTIONS(FROMID INTEGER NOT NULL, VIAID INTEGER NOT NULL, TOID INTEGER NOT NULL, LEFT BOOLEAN, RIGHT BOOLEAN, STRAIGHT BOOLEAN, UTURN BOOLEAN, PRIMARY KEY(FROMID, VIAID, TOID));";
    
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
            std::cerr << "Failed to create saveOSMPropertyStatement." << " Resultcode: " << rc << std::endl;
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
        std::cerr << "Failed to execute saveOSMPropertyStatement." << " Resultcode: " << rc << std::endl;
        return 0;
    }


    rc = sqlite3_reset(_saveOSMPropertyStatement);
    if(rc != SQLITE_OK)
    {
        std::cerr << "Failed to reset saveOSMPropertyStatement." << " Resultcode: " << rc << std::endl;
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
            std::cerr << "Failed to create getOSMPropertyStatement." << " Resultcode: " << rc << std::endl;
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
        std::cerr << "Failed to execute getOSMPropertyStatement." << " Resultcode: " << rc << std::endl;
        return boost::shared_ptr<OSMProperty>();
    }

    rc = sqlite3_reset(_getOSMPropertyStatement);
    if(rc != SQLITE_OK)
    {
        std::cerr << "Failed to reset getOSMPropertyStatement." << " Resultcode: " << rc << std::endl;
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
            std::cerr << "Failed to create getLastInsertRowIDStatement." << " Resultcode: " << rc << std::endl;
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
        std::cerr << "Failed to execute getLastInsertRowIDStatement." << " Resultcode: " << rc << std::endl;
        return 0;
    }

    rc = sqlite3_reset(_getLastInsertRowIDStatement);
    if(rc != SQLITE_OK)
    {
        std::cerr << "Failed to reset getLastInsertRowIDStatement." << " Resultcode: " << rc << std::endl;
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
            std::cerr << "Failed to create saveOSMNodeStatement." << " Resultcode: " << rc << std::endl;
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
        std::cerr << "Failed to execute saveOSMNodeStatement." << " Resultcode: " << rc << std::endl;
        return false;
    }

    rc = sqlite3_reset(_saveOSMNodeStatement);
    if(rc != SQLITE_OK)
    {
        std::cerr << "Failed to reset saveOSMNodeStatement." << " Resultcode: " << rc << std::endl;
    }
    
    //Properties speichern. Erstmal Statement zum Verbinden von Node und Property anlegen...
    if(_saveOSMNodePropertyStatement == NULL)
    {
        rc = sqlite3_prepare_v2(_db, "INSERT INTO NODEPROPERTYID VALUES (@NODEID, @PROPERTYID);", -1, &_saveOSMNodePropertyStatement, NULL);
        if (rc != SQLITE_OK)
        {	
            std::cerr << "Failed to create saveOSMNodePropertyStatement." << " Resultcode: " << rc << std::endl;
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
            std::cerr << "Failed to execute saveOSMNodePropertyStatement." << " Resultcode: " << rc << std::endl;
            return false;
        }
        
        //Statement resetten
        rc = sqlite3_reset(_saveOSMNodePropertyStatement);
        if(rc != SQLITE_OK)
        {
            std::cerr << "Failed to reset saveOSMNodePropertyStatement." << " Resultcode: " << rc << std::endl;
        }
    }
    return true;
}

boost::shared_ptr<OSMNode> TemporaryOSMDatabaseConnection::getOSMNodeByID(boost::uint64_t nodeID)
{
    boost::shared_ptr<OSMNode> node;
      
    int rc;
    if(_getOSMNodeByIDStatement == NULL)
    {		
        rc = sqlite3_prepare_v2(_db, "SELECT LAT, LON FROM NODES WHERE ID=?;",
            -1, &_getOSMNodeByIDStatement, NULL);
        if (rc != SQLITE_OK)
        {	
            std::cerr << "Failed to create getOSMNodeByIDStatement." << " Resultcode: " << rc << std::endl;
            return boost::shared_ptr<OSMNode>();
        }
    }

    // Parameter an das Statement binden
    sqlite3_bind_int64(_getOSMNodeByIDStatement, 1, nodeID);

    // Statement ausfuehren, in einer Schleife immer neue Zeilen holen
    while ((rc = sqlite3_step(_getOSMNodeByIDStatement)) != SQLITE_DONE)
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
                        sqlite3_column_double(_getOSMNodeByIDStatement, 0),
                        sqlite3_column_double(_getOSMNodeByIDStatement, 1))
                        );
        //Gib ihn an einen boost::shared_ptr weiter. newNode jetzt nicht mehr verwenden oder delete drauf anwenden!
        node.reset(newNode);
    }

    if (rc != SQLITE_DONE)
    {
        std::cerr << "Failed to execute getOSMNodeByIDStatement." << " Resultcode: " << rc << std::endl;
        return boost::shared_ptr<OSMNode>();
    }

    rc = sqlite3_reset(_getOSMNodeByIDStatement);
    if(rc != SQLITE_OK)
    {
        std::cerr << "Failed to reset getOSMNodeByIDStatement." << " Resultcode: " << rc << std::endl;
    }
    
    //Bis hier sind die Grundeigenschaften des Knotens geladen. Es fehlen die Attribute.
    //Properties laden
    if(_getOSMNodePropertyStatement == NULL)
    {		
        rc = sqlite3_prepare_v2(_db, "SELECT PROPERTYID FROM NODEPROPERTYID WHERE NODEID=?;",
            -1, &_getOSMNodePropertyStatement, NULL);
        if (rc != SQLITE_OK)
        {	
            std::cerr << "Failed to create getOSMNodePropertyStatement." << " Resultcode: " << rc << std::endl;
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
        std::cerr << "Failed to execute getOSMNodePropertyStatement." << " Resultcode: " << rc << std::endl;
        return boost::shared_ptr<OSMNode>();
    }

    rc = sqlite3_reset(_getOSMNodePropertyStatement);
    if(rc != SQLITE_OK)
    {
        std::cerr << "Failed to reset getOSMNodePropertyStatement." << " Resultcode: " << rc << std::endl;
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


bool TemporaryOSMDatabaseConnection::saveOSMTurnRestriction(const OSMTurnRestriction& turnRestriction)
{
    int rc;
    if(_saveOSMTurnRestrictionStatement == NULL)
    {
        rc = sqlite3_prepare_v2(_db, "INSERT INTO TURNRESTRICTIONS VALUES (@FROMID, @VIAID, @TOID, @LEFT, @RIGHT, @STRAIGHT, @UTURN);", -1, &_saveOSMTurnRestrictionStatement, NULL);
        if (rc != SQLITE_OK)
        {	
            std::cerr << "Failed to create saveOSMTurnRestrictionStatement." << " Resultcode: " << rc << std::endl;
            return false;
        }
    }

    // Parameter an das Statement binden. Bei NULL beim Primary Key wird automatisch inkrementiert
    sqlite3_bind_int64(_saveOSMTurnRestrictionStatement, 1, turnRestriction.getFromId());
    sqlite3_bind_int64(_saveOSMTurnRestrictionStatement, 2, turnRestriction.getViaId());
    sqlite3_bind_int64(_saveOSMTurnRestrictionStatement, 3, turnRestriction.getToId());
    sqlite3_bind_int64(_saveOSMTurnRestrictionStatement, 4, turnRestriction.getLeft());
    sqlite3_bind_int64(_saveOSMTurnRestrictionStatement, 5, turnRestriction.getRight());
    sqlite3_bind_int64(_saveOSMTurnRestrictionStatement, 6, turnRestriction.getStraight());
    sqlite3_bind_int64(_saveOSMTurnRestrictionStatement, 7, turnRestriction.getUTurn());
    
    // Statement ausfuehren
    rc = sqlite3_step(_saveOSMTurnRestrictionStatement);
    if (rc != SQLITE_DONE)
    {	
        std::cerr << "Failed to execute saveOSMTurnRestrictionStatement." << " Resultcode: " << rc << std::endl;
        return false;
    }


    rc = sqlite3_reset(_saveOSMTurnRestrictionStatement);
    if(rc != SQLITE_OK)
    {
        std::cerr << "Failed to reset saveOSMTurnRestrictionStatement." << " Resultcode: " << rc << std::endl;
    }
    
    return true;
}

QVector<boost::shared_ptr<OSMTurnRestriction> > TemporaryOSMDatabaseConnection::getOSMTurnRestrictionByViaID(boost::uint64_t viaID)
{
    QVector<boost::shared_ptr<OSMTurnRestriction> > restrictions;
      
    int rc;
    if(_getOSMTurnRestrictionByViaIDStatement == NULL)
    {		
        rc = sqlite3_prepare_v2(_db, "SELECT FROMID, TOID, LEFT, RIGHT, STRAIGHT, UTURN FROM TURNRESTRICTIONS WHERE VIAID=?;",
            -1, &_getOSMTurnRestrictionByViaIDStatement, NULL);
        if (rc != SQLITE_OK)
        {	
            std::cerr << "Failed to create getOSMTurnRestrictionByViaIDStatement." << " Resultcode: " << rc << std::endl;
            return QVector<boost::shared_ptr<OSMTurnRestriction> >();
        }
    }

    // Parameter an das Statement binden
    sqlite3_bind_int64(_getOSMTurnRestrictionByViaIDStatement, 1, viaID);

    // Statement ausfuehren, in einer Schleife immer neue Zeilen holen
    while ((rc = sqlite3_step(_getOSMTurnRestrictionByViaIDStatement)) != SQLITE_DONE)
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
        OSMTurnRestriction* newRestriction = new OSMTurnRestriction(
                        sqlite3_column_int64(_getOSMTurnRestrictionByViaIDStatement, 0),
                        viaID,
                        sqlite3_column_int64(_getOSMTurnRestrictionByViaIDStatement, 1),
                        sqlite3_column_int64(_getOSMTurnRestrictionByViaIDStatement, 2),
                        sqlite3_column_int64(_getOSMTurnRestrictionByViaIDStatement, 4),
                        sqlite3_column_int64(_getOSMTurnRestrictionByViaIDStatement, 3),
                        sqlite3_column_int64(_getOSMTurnRestrictionByViaIDStatement, 5)
                        );
        //Gib ihn an einen boost::shared_ptr weiter. newNode jetzt nicht mehr verwenden oder delete drauf anwenden!
        restrictions << boost::shared_ptr<OSMTurnRestriction>(newRestriction);
    }

    if (rc != SQLITE_DONE)
    {
        std::cerr << "Failed to execute getOSMTurnRestrictionByViaIDStatement." << " Resultcode: " << rc << std::endl;
        return QVector<boost::shared_ptr<OSMTurnRestriction> >();
    }

    rc = sqlite3_reset(_getOSMTurnRestrictionByViaIDStatement);
    if(rc != SQLITE_OK)
    {
        std::cerr << "Failed to reset getOSMTurnRestrictionByViaIDStatement." << " Resultcode: " << rc << std::endl;
    }
    
    return restrictions;
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
        
        
        
        std::cout << "Checking OSMEdge..." << std::endl;
        //TODO
        
        std::cout << "Checking OSMTurnRestriction..." << std::endl;
        CHECK(connection.beginTransaction());
        OSMTurnRestriction turnRestriction( 0,  1,  2, true, false, true, false );
        CHECK(connection.saveOSMTurnRestriction(turnRestriction));
        OSMTurnRestriction r1( 1,  2,  3, false, false, true, false );
        CHECK(connection.saveOSMTurnRestriction(r1));
        OSMTurnRestriction r2( 4,  5,  6, false, false, true, false );
        CHECK(connection.saveOSMTurnRestriction(r2));
        OSMTurnRestriction r3( 1,  2,  4, false, true, true, false );
        CHECK(connection.saveOSMTurnRestriction(r3));
        OSMTurnRestriction r4( 4,  6,  5, false, false, true, true );
        CHECK(connection.saveOSMTurnRestriction(r4));
        OSMTurnRestriction r5( 3,  2,  3, true, false, false, false );
        CHECK(connection.saveOSMTurnRestriction(r5));
        OSMTurnRestriction r6( 3,  2,  3, true, false, false, false );
        CHECK(!connection.saveOSMTurnRestriction(r6));
        CHECK(connection.endTransaction());
        CHECK_EQ(turnRestriction, *(connection.getOSMTurnRestrictionByViaID(1)[0]));
        //TODO: Laden und mehrere Sachen ablegen
        
        std::cout << "Closing database..." << std::endl;
        connection.close();
        CHECK(!connection.isDBOpen());
        
        return EXIT_SUCCESS;
    }
}
