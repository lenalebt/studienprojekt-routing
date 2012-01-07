#include "webserver.hpp"
#include <iostream>
#include <QRegExp>

template <typename HttpRequestProcessorType>
void HttpServerThread<HttpRequestProcessorType>::run()
{
    bool timedOut = false;
    bool newConnectionAvailable = false;
    
    _server = new HttpServer<HttpRequestProcessorType>(_port, _threadPoolSize);
    
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
    if (_server != 0)
    {
        delete _server;
        _server = 0;
    }
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

bool HttpRequestProcessor::readLine(QTcpSocket* socket, char* c, size_t csize)
{
    bool didReadLine = false;
    int tries=0;
    int bytesRead=0;
    while (!didReadLine)
    {
        tries++;
        if (socket->canReadLine())
        {
            if ((bytesRead = socket->readLine(c, csize-1)))
            {
                c[bytesRead] = '\0';
            }
            didReadLine = true;
        }
        else
        {
            if (tries > 20)
                return false;
            socket->waitForReadyRead(100);
        }
    }
    return true;
}

void HttpRequestProcessor::writeString(QTcpSocket* socket, QString str)
{
    socket->write(str.toLatin1());
}

void HttpRequestProcessor::send404()
{
    writeString(_socket, "HTTP/1.");
    writeString(_socket, httpVersion);
    writeString(_socket, " 404 Not found\n\n");
    writeString(_socket, "<!DOCTYPE html>\n<html><head><title>Not found</title></head><body><p>404 Not found</p></body></html>");
    _socket->flush();
}
void HttpRequestProcessor::send405()
{
    writeString(_socket, "HTTP/1.");
    writeString(_socket, httpVersion);
    writeString(_socket, " 405 Method not allowed\n\n");
    writeString(_socket, "<!DOCTYPE html>\n<html><head><title>Method not allowed</title></head><body><p>405 Method not allowed</p></body></html>");
    _socket->flush();
}

bool HttpRequestProcessor::preprocessRequest()
{
    /* TODO:
     * Aufbereitung der Parameter, Pfade, etc.
     */
    //Testmodus, der nur einfach alles ausgibt was reinkommt.
    _socket->setTextModeEnabled(true);
    char c[1024];
    QString line;
    
    
    if (readLine(_socket, c, 1024))
    {
        std::cerr << c;
        line = c;
    }
    else
        return false;
    
    //Erst beliebige Leerzeichen, dann GET oder PUT oder POST, dann Pfad, Evtl. Parameter, dann welche HTTP-Version.
    QRegExp httpHelloRegExp("(GET|PUT|POST)\\s\\s*([^\\?][^\\?]*)(\\?\\S*)?\\s\\s*HTTP/1.(\\d)");
    //TODO: Vielleicht besser die RegExp einmal statisch erstellen statt immer wieder?
    
    if (httpHelloRegExp.indexIn(line) == -1)
    {
        return false;
    }
    
    //GET-Request
    QString requestType = httpHelloRegExp.cap(1);
    requestPath = httpHelloRegExp.cap(2);
    httpVersion = httpHelloRegExp.cap(4);
    QString parameters = httpHelloRegExp.cap(3);
    
    std::cerr << "requestType: " << requestType << std::endl
        << "requestPath: " << requestPath << std::endl
        << "httpVersion: 1." << httpVersion << std::endl
        << "parameters: " << parameters << std::endl;
    
    if (requestType != "GET")
    {
        std::cerr << "Only GET requests are supported." << std::endl
            << "request type was: " << httpHelloRegExp.cap(1) << std::endl;
        send405();
        return false;
    }
    
    QRegExp httpHeader("(\\S\\S*):\\s\\s*([^\\n]*)");
    while (readLine(_socket, c, 1024))
    {
        line = c;
        if (httpHeader.indexIn(line) != -1)
        {
            headerMap[httpHeader.cap(1)] = httpHeader.cap(2);
            std::cerr << httpHeader.cap(1) << ": " << httpHeader.cap(2) << std::endl;
        }
        else
            break;
    }
    
    
    return true;
}

void HttpRequestProcessor::run()
{
    _socket = new QTcpSocket();
    if (_socket->setSocketDescriptor(_socketDescriptor))
    {
        
        if (this->preprocessRequest())
            this->processRequest();
        else
            std::cerr << "Did not get a well-formed request." << std::endl;
    }
    else
    {
        std::cerr << "Error while trying to initialize connection." << std::endl;
    }
    _socket->close();
    
    if (_socket != 0)
    {
        delete _socket;
        _socket = 0;
    }
}

void BikerHttpRequestProcessor::processRequest()
{
    //TODO: Request bearbeiten
    std::cerr << "Request needs to be processed." << std::endl;
    
    std::cerr << "processing..." << std::endl;
    send404();
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
