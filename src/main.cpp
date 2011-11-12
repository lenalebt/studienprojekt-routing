/**
 * @file
 * @copyright Lizenz: GNU GPL v3
 */ 

#include "main.hpp"
#include <iostream>
#include <boost/program_options.hpp>
#include "tests.hpp"

/**
 * @brief Zur Erzeugung eines Strings aus eines Defines.
 */
#define QUOTEME_(x) #x
/**
 * @brief Zur Erzeugung eines Strings aus eines Defines.
 */
#define QUOTEME(x) QUOTEME_(x)

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
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("test", po::value<std::string>(&testName)->implicit_value("all"), "run program tests")
        ;
    
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    
    
    //Es soll die Hilfe angezeigt werden: Tun wir das doch!
    if (vm.count("help")) {
        cout << desc << "\n";
        return EXIT_FAILURE;
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
    cout << "Biker Version " << QUOTEME(VERSION) << endl;
    //first parse commandline options
    return parseProgramOptions(argc, argv);
}
