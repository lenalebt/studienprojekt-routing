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

/**
 * @brief Parst die Kommandozeilenparameter.
 * @param argc Anzahl Aufrufargumente
 * @param argv Werte der Aufrufparameter
 * @return
 * @todo Diese Funktion ist noch komplett unfertig.
 * @bug Wenn man einen Parameter angibt, der nicht aufgeführt ist,
 *      stürzt das Programm ab (->Exception).
 */
int parseProgramOptions(int argc, char* argv[])
{
    std::string testName("");
    std::string webserver_public_html_folder("");
    unsigned int webserver_port=8080;
    unsigned int threadPoolSize=10;
    
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("test", po::value<std::string>(&testName)->implicit_value("all"), "run program tests")
        ("threadpoolsize", po::value<unsigned int>(&threadPoolSize)->default_value(10u), "set maximum thread pool size")
        ("start-webserver", "start webserver with given or standard settings")
        ("webserver_public_html", po::value<std::string>(&webserver_public_html_folder)->default_value(""), "set public html folder of webserver")
        ("webserver_port", po::value<unsigned int>(&webserver_port)->default_value(8080), "set port of webserver")
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
    if (threadPoolSize < 5)
    {
        std::cerr << "We need a minimum threadpoolsize of 5. Setting to 5." << std::endl;
        threadPoolSize = 5;
    }
    std::cerr << "Using up to " << threadPoolSize << " threads." << std::endl;
    QThreadPool::globalInstance()->setMaxThreadCount(threadPoolSize);
    
    if (vm.count("start-webserver"))
    {
        if (webserver_public_html_folder != "")
        {
            std::cerr << "Starting Webserver:" << std::endl;
            std::cerr << "Webserver port is " << webserver_port << std::endl;
            std::cerr << "Webserver public_html folder is \"" << webserver_public_html_folder << "\"" << std::endl;
            BikerHttpRequestProcessor::publicHtmlDirectory = webserver_public_html_folder.c_str();
            std::cerr << "TODO: Webserver wirklich starten..." << std::endl;
        }
        else
        {
            std::cerr << "Webserver public_html folder may not be empty!" << std::endl;
            std::cerr << "Webserver not starting..." << std::endl;
        }
    }
    
    //Tests ausführen, wenn auf der Kommandozeile so gewollt
    if (vm.count("test")) {
        return biker_tests::testProgram(testName);
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
    //Daher ist ein AUfruf von exec() hier falsch. Er würde die gesamte
    //Anwendung blockieren.
    QCoreApplication app(argc, argv);
    
    //parse commandline options
    retVal = parseProgramOptions(argc, argv);
    
    return retVal;
}
