#include "tests.hpp"
#include <iostream>

#include "routingnode.hpp"
#include "routingedge.hpp"

//für EXIT_SUCCESS und EXIT_FAILURE
#include <boost/program_options.hpp>

/**
 * @file
 * @ingroup tests
 * @todo i18n/l10n
 * @attention Diese Testfunktionen können das Programm komplett in einen anderen
 *      Zustand überführen als es vor dem Aufruf der Funktion hatte
 */

using namespace std;

int testProgram()
{
    //TODO: i18n!
    cout << "starting program tests..." << endl << flush;
    
    cout << fixed << 12345678901234567890.0 << endl;
    cout << fixed << 1.0/0.0 << endl;
    
    //Anpassen, falls Fehler auftraten!
    return EXIT_SUCCESS;
}
