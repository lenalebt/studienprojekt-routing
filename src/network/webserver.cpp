#include "webserver.hpp"
#include <iostream>
#include <QRegExp>

template <typename HttpRequestProcessorType>
void HttpServerThread<HttpRequestProcessorType>::run()
{
    bool timedOut = false;
    bool newConnectionAvailable = false;
    
    _server = new HttpServer<HttpRequestProcessorType>(_port, _threadPoolSize);
    
    //Socket in Wartemodus auf Verbindungen setzen
    if (_server->listen(QHostAddress::Any, _port))
    {
        std::cerr << "Server ready, Listening at port " << _port << std::endl;
    }
    else
    {   //Wartemodus einschalten ging nicht, wieder herunterfahren
        _running = false;
        std::cerr << "Error while trying to listen to port " << _port
            << ". Shutting down." << std::endl;
    }
    
    //So lange auf Verbindungen warten, wie der Server läuft...
    while (_running)
    {
        //Es wird incomingConnection() auf _server aufgerufen, wenn eine Verbindung reinkommt
        newConnectionAvailable = _server->waitForNewConnection(1000, &timedOut);
        if (newConnectionAvailable)
        {
            //std::cerr << "new connection available." << std::endl;
            //do fancy stuff
        }
    }
    
    //Server zumachen und herunterfahren, Speicher freigeben
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
    //10 Sekunden warten, höchstens. Wenn der Thread vorher fertig ist, prima.
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
    _running(false), _port(port), _threadPoolSize(threadPoolSize),
    _server(0)
{
    
}

template <typename HttpRequestProcessorType>
void HttpServerThread<HttpRequestProcessorType>::startServer()
{
    //Server-Thread starten
    _running = true;
    this->start();
}

template <typename HttpRequestProcessorType>
HttpServerThread<HttpRequestProcessorType>::~HttpServerThread()
{
    //Server stoppen und dann Speicher freigeben
    this->stopServer();
    if (_server != 0)
        delete _server;
}


HttpRequestProcessor::HttpRequestProcessor(int socketDescriptor) :
    _socketDescriptor(socketDescriptor), _socket(0)
{
    
}

bool HttpRequestProcessor::readLine(QTcpSocket* socket, QString& line)
{
    bool didReadLine = false;
    int tries=0;
    int bytesRead=0;
    char c[1024];
    while (!didReadLine)
    {
        tries++;
        if (socket->canReadLine())
        {
            if ((bytesRead = socket->readLine(c, 1023)))
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
    line = c;
    return true;
}

void HttpRequestProcessor::writeString(QTcpSocket* socket, QString str)
{
    socket->write(str.toLatin1());
}

void HttpRequestProcessor::send404()
{
    writeString(_socket, "HTTP/1.");
    writeString(_socket, _httpVersion);
    writeString(_socket, " 404 Not found\n\n");
    writeString(_socket, "<!DOCTYPE html>\n<html><head><title>Not found</title></head><body><p>404 Not found</p></body></html>");
    _socket->flush();
}
void HttpRequestProcessor::send405()
{
    writeString(_socket, "HTTP/1.");
    writeString(_socket, _httpVersion);
    writeString(_socket, " 405 Method not allowed\n\n");
    writeString(_socket, "<!DOCTYPE html>\n<html><head><title>Method not allowed</title></head><body><p>405 Method not allowed</p></body></html>");
    _socket->flush();
}

bool HttpRequestProcessor::preprocessRequest()
{
    //Es kommt erstmal auf jeden Fall etwas Text.
    _socket->setTextModeEnabled(true);
    QString line;
    
    
    if (readLine(_socket, line))
    {
        //do fancy stuff
    }
    else
        return false;
    
    //Erst beliebige Leerzeichen, dann GET oder PUT oder POST, dann Pfad, Evtl. Parameter, dann welche HTTP-Version.
    QRegExp httpHelloRegExp("(GET|PUT|POST)\\s+([^\\?]+)(\\?\\S*)?\\s+HTTP/1.(0|1)");
    //TODO: Vielleicht besser die RegExp einmal statisch erstellen statt immer wieder?
    
    if (httpHelloRegExp.indexIn(line) == -1)
    {
        return false;
    }
    
    //Es kam ein gültiger Request.
    _requestType = httpHelloRegExp.cap(1);
    _requestPath = httpHelloRegExp.cap(2);
    _httpVersion = httpHelloRegExp.cap(4);
    QString parameters = httpHelloRegExp.cap(3);
    
    /*std::cerr << "requestType: " << requestType << std::endl
        << "requestPath: " << _requestPath << std::endl
        << "httpVersion: 1." << _httpVersion << std::endl
        << "parameters: " << parameters << std::endl;*/
    
    //Erst Parameter abfragen, dann können in der Zwischenzeit Daten
    //für die Header reinkommen. Müsste so rum schneller sein.
    QRegExp httpParameter("([^=]+)=([^&]+)&?");
    int pos=1;
    while ((pos = httpParameter.indexIn(parameters, pos)) != -1)
    {
        pos += httpParameter.matchedLength();
        _parameterMap[httpParameter.cap(1)] = httpParameter.cap(2);
        //std::cerr << httpParameter.cap(1) << " = " << httpParameter.cap(2) << std::endl;
    }
    
    //Header abfragen.
    QRegExp httpHeader("(\\S\\S*):\\s\\s*([^\\n]*)");
    while (readLine(_socket, line))
    {
        if (httpHeader.indexIn(line) != -1)
        {
            _headerMap[httpHeader.cap(1)] = httpHeader.cap(2);
            //std::cerr << httpHeader.cap(1) << ": " << httpHeader.cap(2) << std::endl;
        }
        else
            break;
    }
    
    //Jetzt kommt potentiell kein Text mehr.
    _socket->setTextModeEnabled(false);
    
    
    
    return true;
}

void HttpRequestProcessor::run()
{
    _socket = new QTcpSocket();
    //Erstmal ein neues Socket aufmachen in diesem Thread.
    if (_socket->setSocketDescriptor(_socketDescriptor))
    {
        //Zuerst Vorverarbeitung, sowas wie Header, Parameter, ...
        if (this->preprocessRequest())
        {
            //Schönen Request bekommen, Parsen klappte alles. Weiter gehts mit der Verarbeitung.
            this->processRequest();
        }
        else
        {
            std::cerr << "Did not get a well-formed request." << std::endl;
        }
    }
    else
    {
        std::cerr << "Error while trying to initialize connection." << std::endl;
    }
    
    //Alles fertig. Daten raussenden und Socket schließen. Speicher freigeben.
    _socket->flush();
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
    std::cerr << "processing request..." << std::endl;
    
    //Es wird nur GET unterstützt, der Rest nicht. Bei was anderem: Grantig sein und 405 antworten.
    if (_requestType != "GET")
    {
        send405();
        return;
    }
    
    send404();
}

namespace biker_tests
{
    int testWebServer()
    {
        std::cerr << "Testing Webserver..." << std::endl;
        
        HttpServerThread<BikerHttpRequestProcessor> server(8081);
        server.startServer();
        server.wait(10000);
        
        return EXIT_FAILURE;
    }
}