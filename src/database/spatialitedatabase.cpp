#include "spatialitedatabase.hpp"

SpatialiteDatabaseConnection::SpatialiteDatabaseConnection() :
    _dbOpen(false)
{
    
}

void SpatialiteDatabaseConnection::close()
{
    sqlite3_close(db);
    _dbOpen = false;
}

/**
 * @todo Laden der Spatialite-Erweiterungen
 */
void SpatialiteDatabaseConnection::open(QString dbConnectionString)
{
    int rc; //return-Wert speichern
    rc = sqlite3_open(dbConnectionString.toStdString().c_str(), &db);
    
    if (rc != SQLITE_OK)
    {   //Es gab einen Fehler beim Öffnen der Datenbank.
        _dbOpen = false;
        sqlite3_close(db);
        std::cerr << "Failed to open database file \"" << dbConnectionString.toStdString()
            << std::endl;
        return;
    }
    
    //Zeiger auf die Fehlernachricht von SQLite. Speicher wird von Sqlite
    //selbst geholt und verwaltet, nur wieder freigeben ist nötig.
    char* errorMessage;
    
    //Bekommt den Dateinamen von Spatialite direkt von CMake :).
    std::string spatialiteFilename;
    spatialiteFilename = QUOTEME(SPATIALITE_LIB);
    
    //Erlaube das Laden von Erweiterungen
    rc = sqlite3_enable_load_extension(db, 1);
    if (rc != SQLITE_OK)
    {
        _dbOpen = false;
        sqlite3_close(db);
        std::cerr << "Failed to enable loading of sqlite3 extensions." << std::endl;
        sqlite3_free(errorMessage);
        return;
    }
    
    //Lade die Erweiterung
    rc = sqlite3_load_extension(db, spatialiteFilename.c_str(), 0, &errorMessage);
    
    if (rc != SQLITE_OK)
    {
        _dbOpen = false;
        sqlite3_close(db);
        std::cerr << "Failed to load spatialite. Filename: \"" << spatialiteFilename
            << ", Error message: \"" << errorMessage << "\"" << std::endl;
        sqlite3_free(errorMessage);
        return;
    }
    
    //Verbiete das laden von Erweiterungen wieder (Sicherheitsfeature?)
    rc = sqlite3_enable_load_extension(db, 0);
    if (rc != SQLITE_OK)
    {
        _dbOpen = false;
        sqlite3_close(db);
        std::cerr << "Failed to disable loading of sqlite3 extensions." << std::endl;
        sqlite3_free(errorMessage);
        return;
    }
    
    _dbOpen = true;
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
SpatialiteDatabaseConnection::getNodes(const GPSPosition &ulCorner, const GPSPosition &brCorner)
{
    return QVector<boost::shared_ptr<RoutingNode> >();
}


void SpatialiteDatabaseConnection::saveNode(const RoutingNode &node)
{
    
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


void SpatialiteDatabaseConnection::saveEdge(const RoutingEdge &edge)
{
    
}


void SpatialiteDatabaseConnection::saveEdge(const RoutingEdge &edge, QString name)
{
    
}


QString SpatialiteDatabaseConnection::getStreetName(const RoutingEdge &edge)
{
    return "";
}

namespace biker_tests
{
    int testSpatialiteDatabaseConnection()
    {
        SpatialiteDatabaseConnection connection;
        
        std::cout << "Opening \"test.db\"..." << std::endl;
        connection.open("test.db");
        CHECK(connection.isDBOpen());
        
        std::cout << "Closing database..." << std::endl;
        connection.close();
        CHECK(!connection.isDBOpen());
        
        return EXIT_SUCCESS;
    }
}

//sqlite3_stmt* statement = NULL;
//rc = sqlite3_prepare_v2(db, ".load libspatialite.so;", -1, &statement, NULL);
