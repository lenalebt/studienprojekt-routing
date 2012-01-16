#ifndef SQLITE_FUNCTIONS
#define SQLITE_FUNCTIONS

#include <sqlite3.h>

/**
 * @brief Hier werden Funktionen gesammelt, die irgendwas mit der
 * SQLite-Datenbank zu tun haben und überall gebraucht werden.
 * 
 * @ingroup database
 */
namespace sqlite_functions
{
    bool handleSQLiteResultcode(int rc);
}

#endif //SQLITE_FUNCTIONS
