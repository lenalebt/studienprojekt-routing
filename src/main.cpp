/**
 * @copyright Lizenz: GNU GPL v3
 */ 

#include "main.hpp"
#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;
using namespace std;

/**
 * @brief Parst die Kommandozeilenparameter.
 * @param argc Anzahl Aufrufargumente
 * @param argv Werte der Aufrufparameter
 * @return
 * @todo Diese Funktion ist noch komplett unfertig.
 */
int parseProgramOptions(int argc, char* argv[])
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ;
    
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    

    if (vm.count("help")) {
        cout << desc << "\n";
        return EXIT_FAILURE;
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
    //first parse commandline options
    return parseProgramOptions(argc, argv);
}
