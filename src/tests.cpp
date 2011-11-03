#include "tests.hpp"
#include <iostream>

//für EXIT_SUCCESS und EXIT_FAILURE
#include <boost/program_options.hpp>

/**
 * @file
 * @ingroup tests
 * @todo i18n/l10n
 */

using namespace std;

int testProgram()
{
    //TODO: i18n!
    cout << "Führe Programmtest durch..." << endl;
    //Anpassen, falls Fehler auftraten!
    return EXIT_SUCCESS;
}

#include "routingnode.hpp"
#include "routingedge.hpp"
