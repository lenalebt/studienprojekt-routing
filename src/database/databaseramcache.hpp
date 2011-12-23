#ifndef DATABASERAMCACHE_HPP
#define DATABASERAMCACHE_HPP

#include "database.hpp"
#include "tests.hpp"

/**
 * @brief Implementiert einen RAM-Cache für Datenbankelemente.
 * 
 * Wenn ein Element bereits im Speicher ist, wird es von dort geholt,
 * statt es aus der Datenbank zu holen. In allen anderen Fällen wird es
 * aus der Datenbank geladen.
 * 
 * @author Lena Brüder
 * @date 2011-12-23
 * @copyright GNU GPL v3
 * @ingroup database
 */
class DatabaseRAMCache : public DatabaseConnection
{
    
};

namespace biker_tests
{
    int testDatabaseRAMCache();
}
#endif //DATABASERAMCACHE_HPP
