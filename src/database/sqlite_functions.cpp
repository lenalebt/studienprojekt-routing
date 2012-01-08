#include "sqlite_functions.hpp"
#include <iostream>

namespace sqlite_functions
{
    bool handleSQLiteResultcode(int rc)
    {
        switch (rc)
        {
            case SQLITE_ROW:
                return true;
            case SQLITE_ERROR:
                std::cerr << "SQL error or missing database." << " Resultcode: " << rc << std::endl;
                return false;
            case SQLITE_BUSY:
                std::cerr << "The database file is locked." << " Resultcode: " << rc << std::endl;
                return false;
            case SQLITE_LOCKED:
                std::cerr << "A table in the database is locked." << " Resultcode: " << rc << std::endl;
                return false;
            case SQLITE_CONSTRAINT:
                std::cerr << "A constraint violation has occured (DB)." << " Resultcode: " << rc << std::endl;
                return false;
            case SQLITE_MISMATCH:
                std::cerr << "Datatype mismatch in DB." << " Resultcode: " << rc << std::endl;
                return false;
            default:
                std::cerr << "Unknown error (DB). Resultcode:" << rc << std::endl;
                return false;
        }
        return false;
    }
}
