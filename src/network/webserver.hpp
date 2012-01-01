#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QRunnable>
#include <QThreadPool>
#include "tests.hpp"
#include <boost/cstdint.hpp>

/**
 * @brief Diese Klasse stellt einen nebenläufig arbeitenden
 *      Http-Server dar, der jeden Request als eigenen Thread
 *      abarbeitet.
 * 
 * 
 * @tparam HttpRequestProcessorType Gibt die Klasse an, die
 *      aufgerufen wird wenn ein Request hereinkommt.
 * @author Lena Brüder
 * @date 2012-01-01
 * @copyright GNU GPL v3
 * @ingroup network
 */
template <typename HttpRequestProcessorType>
class HttpServer : public QTcpServer
{
private:
    boost::uint16_t _port;
    boost::uint16_t _threadPoolSize;
    QThreadPool _threadPool;
protected:
    /**
     * @brief Wird aufgerufen, wenn eine neue Verbindung hergestellt wird.
     */
    void incomingConnection (int socketDescriptor);
public:
    /**
     * @brief Startet einen neuen Webserver am angegebenen Port.
     * 
     * Es werden maximal so viele Verbindungen gleichzeitig akzeptiert,
     * wie über <code>threadPoolSize</code> angegeben.
     * 
     * @remarks Ports kleiner als 1024 sind unter Unix für Prozesse
     *      mit root-Rechten vorbehalten!
     * @param port Der Port, an dem der Webserver lauschen soll.
     * @param threadPoolSize Die maximale Anzahl gleichzeitig
     *      für den Webserver laufender Threads.
     */
    HttpServer(boost::uint16_t port=8080, boost::uint16_t threadPoolSize = 5);
    ~HttpServer();
};

template <typename HttpRequestProcessorType>
class HttpServerThread : public QThread
{
private:
    bool _running;
    boost::uint16_t _port;
    boost::uint16_t _threadPoolSize;
    HttpServer<HttpRequestProcessorType>* _server;
public:
    void run();
    void stopServer();
    HttpServerThread(boost::uint16_t port=8080, boost::uint16_t threadPoolSize = 5);
    ~HttpServerThread();
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
class HttpRequestProcessor : public QRunnable
{
protected:
    int _socketDescriptor;
    QTcpSocket* _socket;
public:
    HttpRequestProcessor(int socketDescriptor);
    void run();
    
    /**
     * @brief Diese Funktion wird aufgerufen, um einen neuen Request zu
     *      bearbeiten.
     * 
     * Dabei sind ein paar Vorverarbeitungsschritte schon gemacht worden:
     *  - Alle Header wurden bearbeitet.
     *  - Weiteres Lesen vom Socket liefert die Daten, die nach dem Header kommen
     * 
     * @todo Implementieren!
     */
    virtual void processRequest()=0;
//    template <typename T> friend class HttpServer<T>;
};

class BikerHttpRequestProcessor : public HttpRequestProcessor
{
public:
    BikerHttpRequestProcessor(int socketDescriptor) :
        HttpRequestProcessor(socketDescriptor) {}
    void processRequest();
};

namespace biker_tests
{
    int testWebServer();
}

#endif //WEBSERVER_HPP
