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
    
    ProgramOptions() :
        webserver_public_html_folder(""),
        webserver_port(8080),
        webserver_threadpool_size(5),
        webserver_startWebserver(false),
        
        threads_threadpool_size(5),
        
        tests_testName("all"),
        tests_starttest(false)
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
        ("threadpoolsize", po::value<unsigned int>(&(programOptions->threads_threadpool_size))->default_value(10u), "set maximum thread pool size")
        ("start-webserver", "start webserver with given or standard settings")
        ("webserver-public-html-folder,d", po::value<std::string>(&(programOptions->webserver_public_html_folder))->default_value(""), "set public html folder of webserver")
        ("webserver-port,p", po::value<unsigned int>(&(programOptions->webserver_port))->default_value(8080), "set port of webserver")
        ("webserver-threadpoolsize", po::value<unsigned int>(&(programOptions->webserver_threadpool_size))->default_value(5), "set maximum thread pool size of webserver")
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
    std::cerr << "Using up to " << programOptions->threads_threadpool_size << " threads." << std::endl;
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
    }
    
    return EXIT_SUCCESS;
}

/**
 * @brief Main-Funktion
 * @param argc Anzahl Aufrufargumente
 * @param argv Werte der Aufrufparameter
 * @return ob das Programm erfolgreich beendet wurde
 * @todo Noch kein effektiver Inhalt.
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
    
    if (programOptions.tests_starttest)
        return biker_tests::testProgram(programOptions.tests_testName);
    
    //TODO: Pointer darf nicht null sein... Warum?
    boost::shared_ptr<HttpServerThread<BikerHttpRequestProcessor> > server;
    if (programOptions.webserver_startWebserver)
    {
        BikerHttpRequestProcessor::publicHtmlDirectory = programOptions.webserver_public_html_folder.c_str();
        server.reset(new HttpServerThread<BikerHttpRequestProcessor>(programOptions.webserver_port, programOptions.webserver_threadpool_size));
        server->startServer();
    }
    
    //Warte, bis der Server beendet wird, so einer gestartet/initialisiert wurde...
    if (server)
        server->wait();
    
    return retVal;
}
