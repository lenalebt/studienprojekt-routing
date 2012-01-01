#include "webserver.hpp"
#include <iostream>

template <typename HttpRequestProcessorType>
void HttpServerThread<HttpRequestProcessorType>::run()
{
    bool timedOut = false;
    bool newConnectionAvailable = false;
    
    _server = new HttpServer<HttpRequestProcessorType>();
    
    if (_server->listen(QHostAddress::Any, _port))
    {
        std::cerr << "Server ready, Listening at port " << _port << std::endl;
    }
    else
    {
        _running = false;
        std::cerr << "Error while trying to listen to port " << _port
            << ". Shutting down." << std::endl;
    }
    
    while (_running)
    {
        
        newConnectionAvailable = _server->waitForNewConnection(1000, &timedOut);
        if (newConnectionAvailable)
        {
            std::cerr << "new connection available." << std::endl;
        }
    }
    _server->close();
    delete _server;
}

template <typename HttpRequestProcessorType>
void HttpServerThread<HttpRequestProcessorType>::stopServer()
{
    std::cerr << "Stopping Webserver..." << std::endl;
    //Server herunterfahren...
    _running = false;
    this->wait(10000);
}

template <typename HttpRequestProcessorType>
void HttpServer<HttpRequestProcessorType>::incomingConnection (int socketDescriptor)
{
    std::cerr << "Incoming connection. Starting HttpRequestProcessor." << std::endl;
    //Neue Verbindung herstellen.
    HttpRequestProcessorType* processor = new HttpRequestProcessorType(socketDescriptor);
    //Verbindung wird gestartet, wenn ein Thread verfügbar wird.
    _threadPool.start(processor);
}

template <typename HttpRequestProcessorType>
HttpServer<HttpRequestProcessorType>::HttpServer(boost::uint16_t port, boost::uint16_t threadPoolSize) :
    _port(port), _threadPoolSize(threadPoolSize)
{
    std::cerr << "Starting Webserver at port " << _port <<
        " with " << _threadPoolSize << " threads."<< std::endl;
    //Maximal threadPoolSize Threads gleichzeitig starten
    _threadPool.setMaxThreadCount(_threadPoolSize);
}
template <typename HttpRequestProcessorType>
HttpServer<HttpRequestProcessorType>::~HttpServer()
{
    
}

template <typename HttpRequestProcessorType>
HttpServerThread<HttpRequestProcessorType>::HttpServerThread(boost::uint16_t port, boost::uint16_t threadPoolSize) :
    _running(true), _port(port), _threadPoolSize(threadPoolSize),
    _server(0)
{
    this->start();
}
template <typename HttpRequestProcessorType>
HttpServerThread<HttpRequestProcessorType>::~HttpServerThread()
{
    this->stopServer();
    if (_server != 0)
        delete _server;
}


HttpRequestProcessor::HttpRequestProcessor(int socketDescriptor) :
    _socketDescriptor(socketDescriptor), _socket(0)
{
    
}

void HttpRequestProcessor::run()
{
    _socket = new QTcpSocket();
    if (_socket->setSocketDescriptor(_socketDescriptor))
    {
        /* TODO:
         * Aufbereitung der Parameter, Pfade, etc.
         * GET/POST/PUT?
         */
        //Testmodus, der nur einfach alles ausgibt was reinkommt.
        _socket->setTextModeEnabled(true);
        char c[100];
        int bytesRead=0;
        
        _socket->waitForReadyRead(10000);
        while (_socket->bytesAvailable()>0)
        {
            if ((bytesRead = _socket->read(c, 99)))
            {
                c[bytesRead] = '\0';
                std::cerr << c;
            }
            //_socket->waitForReadyRead(10000);
        }
        
        this->processRequest();
    }
    else
    {
        std::cerr << "Error while trying to initialize connection." << std::endl;
    }
    if (_socket != 0)
        delete _socket;
}

void BikerHttpRequestProcessor::processRequest()
{
    //TODO: Request bearbeiten
    std::cerr << "Request needs to be processed." << std::endl;
}

namespace biker_tests
{
    int testWebServer()
    {
        std::cerr << "Testing Webserver..." << std::endl;
        
        HttpServerThread<BikerHttpRequestProcessor> server(8081);
        server.wait(10000);
        
        return EXIT_FAILURE;
    }
}
