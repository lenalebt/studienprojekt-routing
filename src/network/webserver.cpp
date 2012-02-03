#include "webserver.hpp"
#include <iostream>
#include <QRegExp>
#include <QUrl>
#include <QDir>
#include "filedownloader.hpp"
#include "gpsposition.hpp"
#include "router.hpp"
#include "routingmetric.hpp"

QString BikerHttpRequestProcessor::publicHtmlDirectory = "";

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
    char c[4096];
    while (!didReadLine)
    {
        tries++;
        if (socket->canReadLine())
        {
            if ((bytesRead = socket->readLine(c, 4095)))
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

bool HttpRequestProcessor::sendFile(QFile& file)
{
    writeString(_socket, "HTTP/1.");
    writeString(_socket, _httpVersion);
    if (file.open(QIODevice::ReadOnly))
    {
        writeString(_socket, " 200 OK\n");
        writeString(_socket, "Content-Length: ");
        writeString(_socket, QString::number(file.size()) + "\n");
        writeString(_socket, "\n");
        _socket->flush();
        char data[66000];
        int bytesRead=0;
        int bytesWritten=0;
        while ((file.bytesAvailable() > 0) && (bytesWritten != -1))
        {
            //64KB-Häppchen der Datei lesen und versenden
            bytesRead = file.read(data, 65536);
            if (bytesRead != -1)
                bytesWritten = _socket->write(data, bytesRead);
            _socket->flush();
        }
    }
    else
    {
        this->send500();
        _socket->flush();
        return false;
    }
    //TODO: Header, die zum Dateiinhalt passen
    //TODO: Dateiinhalt senden
    //TODO: Bei Fehler false zurückgeben
    //writeString(_socket, "<!DOCTYPE html>\n<html><head><title>Bad request</title></head><body><p>400 Bad Request</p></body></html>");
    _socket->flush();
    
    return true;
}
void HttpRequestProcessor::send400()
{
    writeString(_socket, "HTTP/1.");
    writeString(_socket, _httpVersion);
    writeString(_socket, " 400 Bad Request\n\n");
    writeString(_socket, "<!DOCTYPE html>\n<html><head><title>Bad request</title></head><body><p>400 Bad Request</p></body></html>");
    _socket->flush();
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
void HttpRequestProcessor::send102()
{
    writeString(_socket, "HTTP/1.");
    writeString(_socket, _httpVersion);
    writeString(_socket, " 102 Processing\n\n");
    _socket->flush();
}
void HttpRequestProcessor::send500()
{
    writeString(_socket, "HTTP/1.");
    writeString(_socket, _httpVersion);
    writeString(_socket, " 500 Internal Server Error\n\n");
    writeString(_socket, "<!DOCTYPE html>\n<html><head><title>Internal Server error</title></head><body><p>500 Internal Server Error</p></body></html>");
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
    QRegExp httpHelloRegExp("(GET|PUT|POST|DELETE)\\s+([^\\?]+)(\\?\\S*)?\\s+HTTP/1.(0|1)");
    //TODO: Vielleicht besser die RegExp einmal statisch erstellen statt immer wieder?
    
    if (httpHelloRegExp.indexIn(line) == -1)
    {
        std::cerr << "not well-formed: \"" << line << "\"" << std::endl;
        return false;
    }
    
    //Es kam ein gültiger Request.
    _requestType = httpHelloRegExp.cap(1);
    _requestPath = QUrl::fromPercentEncoding(httpHelloRegExp.cap(2).toUtf8());
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
        _parameterMap[QUrl::fromPercentEncoding(httpParameter.cap(1).toUtf8())] = 
            QUrl::fromPercentEncoding(httpParameter.cap(2).toUtf8());
        //std::cerr << httpParameter.cap(1) << " = " << httpParameter.cap(2) << std::endl;
    }
    
    //Header abfragen.
    QRegExp httpHeader("(\\S\\S*):\\s\\s*([^\\n]*)");
    int httpHeaderCount=0;
    while (readLine(_socket, line))
    {
        httpHeaderCount++;
        //mehr als 127 Header-Zeilen wollen wir nicht verarbeiten: Da ist sicher jemand böses am Werk...
        if (httpHeaderCount>127)
            return false;
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
        this->send405();
        return;
    }
    if (_requestPath.contains(".."))
    {
        //".." im Pfad ist ein falscher Request. Damit könnte man ins Dateisystem gelangen.
        this->send400();
    }
    
    std::cerr << "request file: " << _requestPath << std::endl;
    
    if (_requestPath.startsWith("/files/"))
    {
        //"/files/" entfernen!
        QString _myRequestPath = _requestPath.remove(0, 7);
        QDir mainDir(publicHtmlDirectory);
        if ((publicHtmlDirectory == "") || !mainDir.exists())
        {
            this->send404();
            return;
        }
        QFile file(publicHtmlDirectory + "/" + _myRequestPath);
        QDir dir(publicHtmlDirectory + "/" + _myRequestPath);
        
        //Wenn die Datei existiert, und alle sie lesen dürfen (nicht nur
        //    Benutzer oder Gruppe): Datei senden. Sonst: 404 Not found.
        if ((!dir.exists()) && file.exists() && (file.permissions() & QFile::ReadOther))
        {
            std::cerr << "serving file: \"" << file.fileName() << "\"" << std::endl;
            this->sendFile(file);
        }
        else
        {
            if (dir.exists())
                std::cerr << "file is a directory: \"" << file.fileName() << "\". Not serving." << std::endl;
            else if (!file.exists())
                std::cerr << "file not found: \"" << file.fileName() << "\". Not serving." << std::endl;
            else if (file.permissions() & QFile::ReadOther)
                std::cerr << "file does not have read permissions for everybody: \"" << file.fileName() << "\". Not serving." << std::endl;
            
            //In jedem Fall: 404 senden.
            this->send404();
        }
        return;
    }
    else
    {
        std::cerr << "dynamic request. TODO." << std::endl;
        /**
         * @todo RegExp nur einmal erzeugen und dann wiederverwenden!
         * @todo Regexpe sind nicht 100% richtig, dadurch dass sie getrennt wurden...
         */
        QRegExp cloudmadeApiKeyRegExp("^/([\\da-fA-F]{1,64})/(?:api|API)/0.(\\d)");
        QRegExp cloudmadeApiPointListRegExp("^/(?:(\\d{1,3}.\\d{1,10}),(\\d{1,3}.\\d{1,10})),(?:\\[(?:(\\d{1,3}.\\d{1,10}),(\\d{1,3}.\\d{1,10}))(?:,(?:(\\d{1,3}.\\d{1,10}),(\\d{1,3}.\\d{1,10}))){0,20}\\],)?(?:(\\d{1,3}.\\d{1,10}),(\\d{1,3}.\\d{1,10}))");
        QRegExp cloudmadeApiRouteTypeRegExp("^/([a-zA-Z0-9]{1,64})(/([a-zA-Z0-9]{1,64}))?.(gpx|GPX|js|JS)$");
        
        QString apiKey="";
        int apiVersion=0;
        QVector<GPSPosition> routePointList;
        QString routeType="";
        QString routeModifier="";
        QString routeDataType="";
        
        int position=0;
        if ((position=cloudmadeApiKeyRegExp.indexIn(_requestPath)) != -1)
        {
            apiKey = cloudmadeApiKeyRegExp.cap(1);
            apiVersion = cloudmadeApiKeyRegExp.cap(2).toInt();
            //API-Key gefunden. Falls uns der interessiert, hier was damit machen!
            
            if (apiVersion != 3)
            {
                std::cerr <<< "requested api version 0." << apiVersion << ", which is not supported." << std::endl;
                this->send405();
                return;
            }
            
            position += cloudmadeApiKeyRegExp.cap(0).length();
        }
        else
        {
            this->send400();
            return;
        }
        position+=cloudmadeApiPointListRegExp.indexIn(_requestPath.mid(position));
        if (cloudmadeApiPointListRegExp.cap(0).length() != 0)
        {
            //Punktliste gefunden. Auswerten!
            QString strLat, strLon;
            routePointList.clear();
            for (int i=1; i<=cloudmadeApiPointListRegExp.captureCount(); i+=2)
            {
                strLat = cloudmadeApiPointListRegExp.cap(i);
                strLon = cloudmadeApiPointListRegExp.cap(i+1);
                GPSPosition point(strLat.toDouble(), strLon.toDouble());
                routePointList << point;
            }
            
            position += cloudmadeApiPointListRegExp.cap(0).length();
        }
        else
        {
            this->send400();
            return;
        }
        position+=cloudmadeApiRouteTypeRegExp.indexIn(_requestPath.mid(position));
        if (cloudmadeApiRouteTypeRegExp.cap(0).length() != 0)
        {
            routeType = cloudmadeApiRouteTypeRegExp.cap(1);
            routeModifier = cloudmadeApiRouteTypeRegExp.cap(2);
            routeDataType = cloudmadeApiRouteTypeRegExp.cap(3);
            //Routentyp gefunden. Auswerten!
        }
        else
        {
            this->send400();
            return;
        }
        
        //TODO: Routing starten, http-"wird bearbeitet" senden
        this->send102();
        
        this->send500();
        return;
    }
}

template class HttpServerThread<BikerHttpRequestProcessor>;

namespace biker_tests
{
    int testWebServer()
    {
        std::cerr << "Testing Webserver..." << std::endl;
        
        BikerHttpRequestProcessor::publicHtmlDirectory = "./gui/";
        HttpServerThread<BikerHttpRequestProcessor> server(8081);
        server.startServer();
        //todo: aufs hochfahren warten toller lösen als so
        server.wait(300);
        
        FileDownloader downloader;
        QByteArray gui_html = downloader.downloadURL(QUrl("http://localhost:8081/files/gui.html"));
        CHECK(gui_html.size()>0);
        QByteArray marker_red_png = downloader.downloadURL(QUrl("http://localhost:8081/files/img/marker-red.png"));
        CHECK(marker_red_png.size()>0);
        QByteArray bad_request_404 = downloader.downloadURL(QUrl("http://localhost:8081/files/lalala"));
        //QByteArray wird leer sein, wenn die Datei nicht heruntergeladen wurde
        CHECK_EQ(bad_request_404.size(), 0);
        
        QRegExp cloudmadeApiKeyRegExp("/([\\da-fA-F]{1,64})/(?:api|API)/0.(\\d)");
        QRegExp cloudmadeApiPointListRegExp("/(?:(\\d{1,3}.\\d{1,10}),(\\d{1,3}.\\d{1,10})),(?:\\[(?:(\\d{1,3}.\\d{1,10}),(\\d{1,3}.\\d{1,10}))(?:,(?:(\\d{1,3}.\\d{1,10}),(\\d{1,3}.\\d{1,10}))){0,20}\\],)?(?:(\\d{1,3}.\\d{1,10}),(\\d{1,3}.\\d{1,10}))");
        QRegExp cloudmadeApiRouteTypeRegExp("/([a-zA-Z0-9]{1,64})(/([a-zA-Z0-9]{1,64}))?.(gpx|GPX|js|JS)");
        
        
        //http://routes.cloudmade.com/8ee2a50541944fb9bcedded5165f09d9/api/0.3/51.22545,4.40730,%5B51.22,4.41,51.2,4.41%5D,51.23,4.42/car.js?lang=de&units=miles
        //http://routes.cloudmade.com/8ee2a50541944fb9bcedded5165f09d9/api/0.3/47.25976,9.58423,47.26117,9.59882/bicycle.gpx
        //http://routes.cloudmade.com/8ee2a50541944fb9bcedded5165f09d9/api/0.3/47.25976,9.58423,47.26117,9.59882/car/shortest.js
        QString line = "http://routes.cloudmade.com/8ee2a50541944fb9bcedded5165f09d9/api/0.3/51.22545,4.40730,[51.22,4.41,51.2,4.41,51.22,4.41,51.2,4.41],51.23,4.42/car.js";
        int i=0;
        i=cloudmadeApiKeyRegExp.indexIn(line, i);
        CHECK(i!=-1);
        i=cloudmadeApiPointListRegExp.indexIn(line, i);
        CHECK(i!=-1);
        i=cloudmadeApiRouteTypeRegExp.indexIn(line, i);
        CHECK(i!=-1);
        
        server.wait(100);
        
        return EXIT_FAILURE;
    }
}
