#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <QTcpServer>
#include <QThread>
#include "tests.hpp"
#include <boost/cstdint.hpp>

class HttpServer : public QTcpServer, public QThread
{
private:
    bool _running;
    boost::uint16_t _port;
public:
    HttpServer(boost::uint16_t port);
    void run();
};

class HttpRequestPreprocessor
{
private:
    
public:
    
};

class HttpGetRequest
{
private:
    
public:
    
};

namespace biker_tests
{
    int testWebServer();
}

#endif //WEBSERVER_HPP
