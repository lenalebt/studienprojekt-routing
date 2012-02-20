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
#include "databaseramcache.hpp"
#include <sstream>

namespace po = boost::program_options;
using namespace std;

class ProgramOptions
{
public:
    //Webserver-spezifischer kram
    std::string webserver_public_html_folder;
    unsigned int webserver_port;
    unsigned int webserver_threadpool_size;
    bool webserver_startWebserver;
    
    //Threads
    unsigned int threads_threadpool_size;
    
    //Tests
    std::string tests_testName;
    bool tests_starttest;
    
    //Datenvorverarbeitung
    std::string osmFilename;
    bool parseOsmFile;
    bool simpleParseOsmFile;
    
    //Datenbank
    std::string dbFilename;
    std::string dbBackend;
    
    //Routenberechnung
    bool doRouting;
    GPSPosition routingStartPoint;
    GPSPosition routingEndPoint;
    std::string routingStartPointString;
    std::string routingEndPointString;
    bool routeOutputAsGPX;
    
    ProgramOptions() :
        webserver_public_html_folder(""),
        webserver_port(8080),
        webserver_threadpool_size(20),
        webserver_startWebserver(false),
        
        threads_threadpool_size(20),
        
        tests_testName("all"),
        tests_starttest(false),
        
        osmFilename(""),
        parseOsmFile(false),
        simpleParseOsmFile(false),
        
        dbFilename(""),
        dbBackend("spatialite"),
        
        doRouting(false),
        routingStartPoint(),
        routingEndPoint(),
        routingStartPointString(""),
        routingEndPointString(""),
        routeOutputAsGPX(true)
    {
        
    }
};

/**
 * @brief Parst die Kommandozeilenparameter.
 * @param argc Anzahl Aufrufargumente
 * @param argv Werte der Aufrufparameter
 * @return
 * @todo Diese Funktion ist noch komplett unfertig.
 * @bug Wenn man einen Parameter angibt, der nicht aufgeführt ist,
 *      stürzt das Programm ab (->Exception).
 */
int parseProgramOptions(int argc, char* argv[], ProgramOptions* programOptions)
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("test,t", po::value<std::string>(&(programOptions->tests_testName))->implicit_value("all"), "run program tests")
        ("threadpoolsize", po::value<unsigned int>(&(programOptions->threads_threadpool_size))->default_value(20u), "set maximum thread pool size for standard purposes")
        ("start-webserver", "start webserver with given or standard settings")
        ("webserver-public-html-folder,d", po::value<std::string>(&(programOptions->webserver_public_html_folder))->default_value("./gui/"), "set public html folder of webserver")
        ("webserver-port,p", po::value<unsigned int>(&(programOptions->webserver_port))->default_value(8080), "set port of webserver")
        ("webserver-threadpoolsize", po::value<unsigned int>(&(programOptions->webserver_threadpool_size))->default_value(20u), "set maximum thread pool size of webserver")
        ("parse", po::value<std::string>(&(programOptions->osmFilename))->implicit_value("input.osm"), "set filename to parse for parser")
        ("simple-parse", po::value<std::string>(&(programOptions->osmFilename))->implicit_value("input.osm"), "set filename to parse for simple parser")
        ("dbfile", po::value<std::string>(&(programOptions->dbFilename))->default_value("database.db"), "set database filename for database operations")
        ("dbbackend", po::value<std::string>(&(programOptions->dbBackend))->implicit_value("spatialite"), "set database backend. possible values: spatialite.")
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
    
    
    if (vm.count("start-webserver"))
    {
        programOptions->webserver_startWebserver = true;
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
        //Dateinamen wurden schon gesetzt vom Framework
    }
    if (vm.count("simple-parse"))
    {
        programOptions->simpleParseOsmFile = true;
        //Dateinamen wurden schon gesetzt vom Framework
    }
    
    if (vm.count("dbbackend"))
    {
        if (programOptions->dbBackend != "spatialite")
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
    int retVal=0;
    
    //wird benötigt für EventLoops etc. Diese werden nur in eigenen Threads
    //verwendet (z.B. Webserver), und nicht in der Hauptanwendung!
    //Daher ist ein Aufruf von exec() hier falsch. Er würde die gesamte
    //Anwendung blockieren.
    QCoreApplication app(argc, argv);
    
    ProgramOptions programOptions;
    //parse commandline options
    retVal = parseProgramOptions(argc, argv, &programOptions);
    
    if (retVal == EXIT_FAILURE)
        return EXIT_FAILURE;
    
    if (programOptions.tests_starttest)
        return biker_tests::testProgram(programOptions.tests_testName);
    
    boost::shared_ptr<HttpServerThread<BikerHttpRequestProcessor> > server;
    if (programOptions.webserver_startWebserver)
    {
        BikerHttpRequestProcessor::publicHtmlDirectory = programOptions.webserver_public_html_folder.c_str();
        server.reset(new HttpServerThread<BikerHttpRequestProcessor>(programOptions.webserver_port, programOptions.webserver_threadpool_size));
        server->startServer();
    }
    
    /* TODO: Für den Parser:
     * programOptions->parseOsmFile abfragen. Hier steht, ob geparst werden soll.
     * 
     * Die Dateinamen findet man in programOptions->osmFilename und
     * programoptions->dbFilename.
     * 
     * Evtl muss man sich noch überlegen ob man das "start-webserver"
     * übergehen will, wenn er parsen soll - oder er startet dafür einen
     * Thread. Lass uns da nochmal reden.
     * 
     * Hier ist erstmal eine Beispielimplementierung.
     */
    if (programOptions.parseOsmFile)
    {
        //TODO: Andere Backends zulassen
        boost::shared_ptr<SpatialiteDatabaseConnection> ptr(new SpatialiteDatabaseConnection());
        DataPreprocessing preprocessor(ptr);
        return (preprocessor.startparser(programOptions.osmFilename.c_str(), programOptions.dbFilename.c_str()) ? EXIT_SUCCESS : EXIT_FAILURE);
    }
    else if (programOptions.simpleParseOsmFile)
    {
        //TODO: Andere Backends zulassen
        boost::shared_ptr<SpatialiteDatabaseConnection> ptr(new SpatialiteDatabaseConnection());
        SimpleDataPreprocessing preprocessor(ptr);
        return (preprocessor.preprocess(programOptions.osmFilename.c_str(), programOptions.dbFilename.c_str()) ? EXIT_SUCCESS : EXIT_FAILURE);
    }
    
    boost::shared_ptr<DatabaseConnection> db;
    if (programOptions.dbBackend == "spatialite")
        db.reset(new SpatialiteDatabaseConnection());
    if (db)
    {
        QFile file(programOptions.dbFilename.c_str());
        if (!file.exists())
        {
            std::cerr << "did not find database file \"" << programOptions.dbFilename << "\". exiting." << std::endl;
            return 1;
        }
        db->open(programOptions.dbFilename.c_str());
        if (!db->isDBOpen())
        {
            std::cerr << "error while opening database file \"" << programOptions.dbFilename << "\". exiting." << std::endl;
            return 1;
        }
        boost::shared_ptr<DatabaseConnection> dbcache(new DatabaseRAMCache(db));
        DatabaseConnection::setGlobalInstance(dbcache);
    }
    else
    {
        std::cerr << "was not able to construct global database object. exiting." << std::endl;
    }
    
    if (programOptions.doRouting)
    {
        boost::shared_ptr<Router> router;
        boost::shared_ptr<RoutingMetric> metric;
        boost::shared_ptr<AltitudeProvider> altitudeProvider;
        
        altitudeProvider.reset(new SRTMProvider());
        metric.reset(new EuclidianRoutingMetric(altitudeProvider));
        
        router.reset(new DijkstraRouter(db, metric));
        
        GPSRoute route = router->calculateShortestRoute(programOptions.routingStartPoint, programOptions.routingEndPoint);
        QString routeString;
        if (programOptions.routeOutputAsGPX)
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
