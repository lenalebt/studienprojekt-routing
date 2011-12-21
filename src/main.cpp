/**
 * @file
 * @copyright Lizenz: GNU GPL v3
 */ 

#include "main.hpp"
#include <iostream>
#include <boost/program_options.hpp>
#include "tests.hpp"
#include <QThreadPool>

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
    unsigned int threadPoolSize=10;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("test", po::value<std::string>(&testName)->implicit_value("all"), "run program tests")
        ("threadpoolsize", po::value<unsigned int>(&threadPoolSize)->default_value(10u), "set maximum thread pool size")
        ;
    
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    
    
    //Es soll die Hilfe angezeigt werden: Tun wir das doch!
    if (vm.count("help")) {
        cout << desc << "\n";
        return EXIT_FAILURE;
    }
    
    //Threadpool-Größe festlegen. Minimum nötig: 5.
    if (threadPoolSize < 5)
    {
        std::cout << "We need a minimum threadpoolsize of 5. Setting to 5." << std::endl;
        threadPoolSize = 5;
    }
    std::cout << "Using up to " << threadPoolSize << " threads." << std::endl;
    QThreadPool::globalInstance()->setMaxThreadCount(threadPoolSize);
    
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
    cout << "Biker Version " << QUOTEME(VERSION) << endl;
    int retVal=0;
    
    //parse commandline options
    retVal = parseProgramOptions(argc, argv);
    
    return retVal;
}
