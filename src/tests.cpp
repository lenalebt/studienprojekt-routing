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
    cout << "Führe Programmtest durch..." << endl;
    //Anpassen, falls Fehler auftraten!
    return EXIT_SUCCESS;
}
