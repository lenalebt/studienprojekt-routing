#include "database.hpp"

boost::shared_ptr<DatabaseConnection> DatabaseConnection::_globalInstance = boost::shared_ptr<DatabaseConnection>();
