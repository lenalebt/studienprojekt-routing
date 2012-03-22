/**
 * @file
 * @copyright Lizenz: GNU GPL v3
 */ 

#include "main.hpp"
#include <iostream>
#include <boost/program_options.hpp>
#include "tests.hpp"
#include <QThreadPool>
#include <QCoreApplication>
#include "webserver.hpp"
#include "datapreprocessing.hpp"
#include "simpledatapreprocessing.hpp"
#include "gpsposition.hpp"
#include "gpsroute.hpp"
#include "router.hpp"
#include "dijkstra.hpp"
#include "altitudeprovider.hpp"
#include "srtmprovider.hpp"
#include "spatialitedatabase.hpp"
#include "sqlitedatabase.hpp"
#include "databaseramcache.hpp"
#include <sstream>
#include "programoptions.hpp"

namespace po = boost::program_options;
using namespace std;

/**
 * @brief Parst die Kommandozeilenparameter.
 * @param argc Anzahl Aufrufargumente
 * @param argv Werte der Aufrufparameter
 * @param programOptions Ein shared_ptr auf ein Einstellungsobjekt, in dem die EInstellungsne gespeichert werden
 * @return
 */
int parseProgramOptions(int argc, char* argv[], boost::shared_ptr<ProgramOptions> programOptions)
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("test,t", po::value<std::string>(&(programOptions->tests_testName))->implicit_value("all"), "run program tests")
        ("threadpoolsize", po::value<unsigned int>(&(programOptions->threads_threadpool_size))->default_value(20u), "set maximum thread pool size for standard purposes")
        ("no-start-webserver", "do not start webserver")
        ("webserver-public-html-folder,d", po::value<std::string>(&(programOptions->webserver_public_html_folder))->default_value("./gui/"), "set public html folder of webserver")
        ("webserver-api-key,k", po::value<std::string>(&(programOptions->webserver_apikey))->default_value(""), "set admissable API key (kind of a \"server password\")")
        ("webserver-port,p", po::value<unsigned int>(&(programOptions->webserver_port))->default_value(8080), "set port of webserver")
        ("webserver-threadpoolsize", po::value<unsigned int>(&(programOptions->webserver_threadpool_size))->default_value(20u), "set maximum thread pool size of webserver")
        ("parse", po::value<std::string>(&(programOptions->osmFilename))->implicit_value("input.osm"), "set filename to parse for parser")
        ("simple-parse", po::value<std::string>(&(programOptions->osmFilename))->implicit_value("input.osm"), "set filename to parse for simple parser")
        ("dbfile", po::value<std::string>(&(programOptions->dbFilename))->default_value("database.db"), "set database filename for database operations")
        #ifdef SPATIALITE_FOUND
            ("dbbackend", po::value<std::string>(&(programOptions->dbBackend))->default_value("spatialite"), "set database backend. possible values: spatialite, sqlite.")
        #else
            ("dbbackend", po::value<std::string>(&(programOptions->dbBackend))->default_value("sqlite"), "set database backend. possible values: sqlite.")
        #endif
        ("dbcachesize", po::value<unsigned int>(&(programOptions->dbCacheSize))->default_value(500000u), "set database cache size (in bytes per query statement, ~6x)")
        ("route", po::value<std::string>(&(programOptions->routingStartPointString))->implicit_value("(0/0)"), "set routing startpoint.")
        ("to", po::value<std::string>(&(programOptions->routingEndPointString))->implicit_value("(0/0)"), "set routing endpoint.")
        ("json-output", "create routes as JSON instead of GPX.")
        ;
    
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    
    
    //Es soll die Hilfe angezeigt werden: Tun wir das doch!
    if (vm.count("help")) {
        std::cerr << desc << "\n";
        return EXIT_FAILURE;
    }
    
    //Threadpool-Größe festlegen. Minimum nötig: 5.
    if (programOptions->threads_threadpool_size < 5)
    {
        std::cerr << "We need a minimum threadpoolsize of 5. Setting to 5." << std::endl;
        programOptions->threads_threadpool_size = 5;
    }
    std::cerr << "Using up to " << programOptions->threads_threadpool_size << " threads for standard purposes." << std::endl;
    QThreadPool::globalInstance()->setMaxThreadCount(programOptions->threads_threadpool_size);
    
    
    if (vm.count("no-start-webserver"))
    {
        programOptions->webserver_startWebserver = false;
    }
    else
    {
        //Threadpool-Größe festlegen. Minimum nötig: 5.
        if (programOptions->webserver_threadpool_size < 5)
        {
            std::cerr << "We need a minimum threadpoolsize of 5 for webserver. Setting to 5." << std::endl;
            programOptions->webserver_threadpool_size = 5;
        }
        std::cerr << "Using up to " << programOptions->webserver_threadpool_size << " threads for webserver." << std::endl;
    }
    
    //Tests ausführen, wenn auf der Kommandozeile so gewollt
    if (vm.count("test"))
    {
        programOptions->tests_starttest = true;
        //Testname wurde schon gesetzt vom Framework
    }
    
    if (vm.count("parse"))
    {
        programOptions->parseOsmFile = true;
        programOptions->webserver_startWebserver = false;
        //Dateinamen wurden schon gesetzt vom Framework
    }
    if (vm.count("simple-parse"))
    {
        programOptions->simpleParseOsmFile = true;
        programOptions->webserver_startWebserver = false;
        //Dateinamen wurden schon gesetzt vom Framework
    }
    
    if (vm.count("dbbackend"))
    {
        if (
            #ifdef SPATIALITE_FOUND
                (programOptions->dbBackend != "spatialite") && 
            #endif
            (programOptions->dbBackend != "sqlite")
            )
        {
            std::cerr << "did not find database backend \"" << programOptions->dbBackend
                    << "\". see help for possible values." << std::endl;
        }
    }
    
    if (vm.count("route"))
    {
        programOptions->doRouting = true;
        std::stringstream strstream (std::stringstream::in | std::stringstream::out);
        strstream << programOptions->routingStartPointString << programOptions->routingEndPointString;
        strstream >> (programOptions->routingStartPoint);
        strstream >> (programOptions->routingEndPoint);
    }
    
    if (vm.count("json-output"))
        programOptions->routeOutputAsGPX = false;
    else
        programOptions->routeOutputAsGPX = true;
    
    return EXIT_SUCCESS;
}

/**
 * @brief Main-Funktion
 * @param argc Anzahl Aufrufargumente
 * @param argv Werte der Aufrufparameter
 * @return ob das Programm erfolgreich beendet wurde
 * @bug Wenn eine Datei als --dbfile übergeben wird, die schon existiert, wird sie noch nicht gelöscht, sondern Daten angefügt.
 */
int main ( int argc, char* argv[] )
{
    cerr << "Biker Version " << QUOTEME(VERSION) << endl;
    #ifdef SPATIALITE_FOUND
        cerr << "compiled with spatialite support" << endl;
    #else
        cerr << "compiled without spatialite support" << endl;
    #endif
    #ifdef ZZIP_FOUND
        cerr << "compiled with zzip support" << endl;
    #else
        cerr << "compiled without zzip support" << endl;
    #endif
    #ifdef PROTOBUF_FOUND
        cerr << "compiled with protobuf support" << endl;
    #else
        cerr << "compiled without protobuf support" << endl;
    #endif
    int retVal=0;
    
    //wird benötigt für EventLoops etc. Diese werden nur in eigenen Threads
    //verwendet (z.B. Webserver), und nicht in der Hauptanwendung!
    //Daher ist ein Aufruf von exec() hier falsch. Er würde die gesamte
    //Anwendung blockieren.
    QCoreApplication app(argc, argv);
    
    boost::shared_ptr<ProgramOptions> programOptions = ProgramOptions::getInstance();
    //parse commandline options
    try
    {
        retVal = parseProgramOptions(argc, argv, programOptions);
    }
    catch (boost::program_options::unknown_option e)
    {
        std::cerr << e.what() << ". See help for allowed options." << std::endl;
        return 1;
    }
    
    if (retVal == EXIT_FAILURE)
        return EXIT_FAILURE;
    
    if (programOptions->tests_starttest)
        return biker_tests::testProgram(programOptions->tests_testName);
    
    boost::shared_ptr<HttpServerThread<BikerHttpRequestProcessor> > server;
    if (programOptions->webserver_startWebserver)
    {
        server.reset(new HttpServerThread<BikerHttpRequestProcessor>(programOptions->webserver_port, programOptions->webserver_threadpool_size));
        server->startServer();
    }
    
    
    boost::shared_ptr<DatabaseConnection> db;
    #ifdef SPATIALITE_FOUND
        if (programOptions->dbBackend == "spatialite")
            db.reset(new SpatialiteDatabaseConnection());
        else 
    #endif
    if (programOptions->dbBackend == "sqlite")
        db.reset(new SQLiteDatabaseConnection());
    if (db)
    {
        QFile file(programOptions->dbFilename.c_str());
        if (!(programOptions->parseOsmFile || programOptions->simpleParseOsmFile))
        {
            if (!file.exists())
            {
                std::cerr << "did not find database file \"" << programOptions->dbFilename << "\". exiting." << std::endl;
                return 1;
            }
            db->open(programOptions->dbFilename.c_str());
            if (!db->isDBOpen())
            {
                std::cerr << "error while opening database file \"" << programOptions->dbFilename << "\". exiting." << std::endl;
                return 1;
            }
            db->close();
        }
    }
    else
    {
        std::cerr << "was not able to construct database object. exiting." << std::endl;
        return 1;
    }
    
    if (programOptions->parseOsmFile)
    {
        std::cerr << "starting data preprocessing..." << std::endl;
        DataPreprocessing preprocessor(db);
        return (preprocessor.startparser(programOptions->osmFilename.c_str(), programOptions->dbFilename.c_str()) ? EXIT_SUCCESS : EXIT_FAILURE);
    }
    else if (programOptions->simpleParseOsmFile)
    {
        std::cerr << "starting simple data preprocessing..." << std::endl;
        SimpleDataPreprocessing preprocessor(db);
        return (preprocessor.preprocess(programOptions->osmFilename.c_str(), programOptions->dbFilename.c_str()) ? EXIT_SUCCESS : EXIT_FAILURE);
    }
    
    if (programOptions->doRouting)
    {
        boost::shared_ptr<Router> router;
        boost::shared_ptr<RoutingMetric> metric;
        boost::shared_ptr<AltitudeProvider> altitudeProvider;
        
        #ifdef ZZIP_FOUND
            altitudeProvider.reset(new SRTMProvider());
        #else
            altitudeProvider.reset(new ZeroAltitudeProvider());
        #endif
        metric.reset(new EuclidianRoutingMetric(altitudeProvider));
        
        router.reset(new DijkstraRouter(db, metric));
        
        GPSRoute route = router->calculateShortestRoute(programOptions->routingStartPoint, programOptions->routingEndPoint);
        QString routeString;
        if (programOptions->routeOutputAsGPX)
            routeString = route.exportGPXString();
        else
            routeString = route.exportJSONString();
        std::cout << routeString;
    }
    
    //Warte, bis der Server beendet wird, so einer gestartet/initialisiert wurde...
    if (server)
        server->wait();
    
    return retVal;
}
