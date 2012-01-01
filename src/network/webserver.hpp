#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include "tests.hpp"
#include <boost/cstdint.hpp>

/**
 * @brief Diese Klasse stellt einen nebenläufig arbeitenden
 *      Http-Server dar, der jeden Request als eigenen Thread
 *      abarbeitet.
 * 
 * 
 * 
 * @author Lena Brüder
 * @date 2012-01-01
 * @copyright GNU GPL v3
 * @ingroup network
 */
class HttpServer : public QTcpServer, public QThread
{
private:
    bool _running;
    boost::uint16_t _port;
public:
    HttpServer(boost::uint16_t port);
    void run();
};

/**
 * @brief Diese Klasse stellt einen Http-Request-Prozessor dar,
 *      der als eigener Thread gestartet wird.
 * 
 * 
 * 
 * @author Lena Brüder
 * @date 2012-01-01
 * @copyright GNU GPL v3
 * @ingroup network
 */
class HttpRequestProcessor : public QThread
{
private:
    int _socketDescriptor;
    HttpRequestProcessor(int socketDescriptor);
public:
    void run();
    friend class HttpServer;
};

namespace biker_tests
{
    int testWebServer();
}

#endif //WEBSERVER_HPP
