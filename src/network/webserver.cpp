#include "webserver.hpp"

void HttpServer::run()
{
    bool timedOut = false;
    bool newConnectionAvailable = false;
    while (_running)
    {
        
        newConnectionAvailable = this->waitForNewConnection(100, &timedOut);
        if (newConnectionAvailable)
        {
            /* TODO:
             * -Verbindung aufbauen. Achtunf, bei anderen Threads: socketDescriptor
             *      weitergeben, und nicht hier schon Socket erstellen (leider)!
             * -...
             */
        }
    }
}

HttpServer::HttpServer(boost::uint16_t port) :
    _running(false), _port(port)
{
    this->start();
}

namespace biker_tests
{
    int testWebServer()
    {
        return EXIT_FAILURE;
    }
}
