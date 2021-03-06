#include "webserver.hpp"
#include <iostream>
#include <QRegExp>
#include <QUrl>
#include <QDir>
#include "filedownloader.hpp"
#include "gpsposition.hpp"
#include "router.hpp"
#include "routingmetric.hpp"
#include "database.hpp"
#include "dijkstra.hpp"
#include "astar.hpp"
#include "programoptions.hpp"
#include "spatialitedatabase.hpp"
#include "sqlitedatabase.hpp"
#include "srtmprovider.hpp"
#include "databaseramcache.hpp"

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
    //TODO: Aufs Hochfahren warten, schöner als so.
    this->wait(300);
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
        writeString(_socket, "Pragma: no-cache\n");
        writeString(_socket, "Cache-Control: no-store\n");
        writeString(_socket, "\n");
        _socket->flush();
        char data[66000];
        int bytesRead=0;
        int bytesWritten=0;
        while ((file.bytesAvailable() > 0) && (bytesWritten != -1))
        {
            //64KB-Häppchen der Datei lesen und versenden
            bytesRead = file.read(data, 1023);
            if (bytesRead != -1)
            {
                bytesWritten = _socket->write(data, bytesRead);
                _socket->waitForBytesWritten();
                _socket->flush();
                if (bytesRead != bytesWritten)
                {
                    std::cerr << "error while sending data" << std::endl;
                }
            }
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
bool HttpRequestProcessor::sendFile(const QString& content)
{
    writeString(_socket, "HTTP/1.");
    writeString(_socket, _httpVersion);
    writeString(_socket, " 200 OK\n");
    writeString(_socket, "Content-Length: ");
    writeString(_socket, QString::number(content.length()) + "\n");
    writeString(_socket, "Pragma: no-cache\n");
    writeString(_socket, "Cache-Control: no-store\n");
    writeString(_socket, "\n");
    _socket->flush();
    QByteArray data = content.toUtf8();
    //std::cerr << "content length: " << data.size() << std::endl;
    for (int i=0; i<data.size(); i+=512)
    {
        _socket->write(data.mid(i, 512));
        _socket->waitForBytesWritten();
        _socket->flush();
    }
    //_socket->write(data);
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
void HttpRequestProcessor::send403()
{
    writeString(_socket, "HTTP/1.");
    writeString(_socket, _httpVersion);
    writeString(_socket, " 403 Access forbidden\n\n");
    writeString(_socket, "<!DOCTYPE html>\n<html><head><title>Access forbidden</title></head><body><p>403 Access forbidden</p></body></html>");
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
        this->send400();
        return false;
    }
    
    //Es kam ein gültiger Request.
    _requestType = httpHelloRegExp.cap(1);
    _requestPath = QUrl::fromPercentEncoding(httpHelloRegExp.cap(2).toUtf8());
    _httpVersion = httpHelloRegExp.cap(4);
    QString parameters = httpHelloRegExp.cap(3);
    
    std::cerr << "requestPath: " << _requestPath << std::endl
        << "parameters: " << parameters << std::endl
        << "httpVersion: 1." << _httpVersion << std::endl;
    
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
        {
            this->send400();
            return false;
        }
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
    std::cerr << "processing request..." << std::endl;
    
    QRegExp numberRegExp("(\\d+(?:.\\d+)?)");
    
    //Es wird nur GET unterstützt, der Rest nicht. Bei was anderem: Grantig sein und 405 antworten.
    if (_requestType != "GET")
    {
        this->send405();
        return;
    }
    if (_requestPath.contains(".."))
    {
        //".." im Pfad ist ein falscher Request. Damit könnte man ins Dateisystem gelangen.
        std::cerr << "\"..\" in request: not allowed." << std::endl;
        this->send400();
    }
    
    std::cerr << "request file: " << _requestPath << std::endl;
    
    if (_requestPath.startsWith("/files/"))
    {
        if (! ProgramOptions::getInstance()->webserver_no_serve_files)
        {
            //"/files/" entfernen!
            QString _myRequestPath = _requestPath.remove(0, 7);
            QDir mainDir((ProgramOptions::getInstance()->webserver_public_html_folder).c_str());
            if ((ProgramOptions::getInstance()->webserver_public_html_folder == "") || !mainDir.exists())
            {
                this->send404();
                return;
            }
            QFile file(QString(ProgramOptions::getInstance()->webserver_public_html_folder.c_str()) + "/" + _myRequestPath);
            QDir dir(QString(ProgramOptions::getInstance()->webserver_public_html_folder.c_str()) + "/" + _myRequestPath);
            
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
        {   //Dateien ausliefern durch Einstellungen verboten: Nicht ausliefern.
            std::cerr << "webserver configured not to serve files." << std::endl;
            this->send404();
            return;
        }
    }
    else
    {
        /**
         * @todo RegExp nur einmal erzeugen und dann wiederverwenden!
         */
        QRegExp cloudmadeApiKeyRegExp("^/([\\da-fA-F]{1,64})/(?:api|API)/0.(\\d)");
        //QRegExp cloudmadeApiPointListRegExp("^/(?:(\\d{1,3}.\\d{1,16}),(\\d{1,3}.\\d{1,16})),(?:\\[(?:(\\d{1,3}.\\d{1,16}),(\\d{1,3}.\\d{1,16}))(?:,(?:(\\d{1,3}.\\d{1,16}),(\\d{1,3}.\\d{1,16}))){0,20}\\],)?(?:(\\d{1,3}.\\d{1,16}),(\\d{1,3}.\\d{1,16}))");
        QRegExp cloudmadeApiPointListRegExp("^/(?:(\\d{1,3}.\\d{1,16}),(\\d{1,3}.\\d{1,16})),(?:\\[(?:(\\d{1,3}.\\d{1,16}),(\\d{1,3}.\\d{1,16}))(?:,(\\d{1,3}.\\d{1,16}),(\\d{1,3}.\\d{1,16})){0,200}\\],)?(?:(\\d{1,3}.\\d{1,16}),(\\d{1,3}.\\d{1,16}))");
        QRegExp cloudmadeApiPointListExtractor("(?:(\\d{1,3}.\\d{1,16}),(\\d{1,3}.\\d{1,16}))");
        QRegExp cloudmadeApiRouteTypeRegExp("^/([a-zA-Z0-9]{1,64})(?:/([a-zA-Z0-9]{1,64}))?.(gpx|GPX|js|JS)$");
        
        QString apiKey="";
        int apiVersion=0;
        QVector<GPSPosition> routePointList;
        QString routeType="";
        QString routeModifier="";
        QString routeDataType="";
        
        int position=0;
        if ((position=cloudmadeApiKeyRegExp.indexIn(_requestPath)) != -1)
        {
            apiKey = cloudmadeApiKeyRegExp.cap(1).toLower();
            apiVersion = cloudmadeApiKeyRegExp.cap(2).toInt();
            //API-Key gefunden. Falls uns der interessiert, hier was damit machen!
            
            if (ProgramOptions::getInstance()->webserver_apikey != "")
            {
                if (ProgramOptions::getInstance()->webserver_apikey != apiKey.toStdString())
                {
                    std::cerr << "api key \"" << apiKey << "\" is not valid." << std::endl;
                    this->send403();
                    return;
                }
            }
            
            if (apiVersion != 3)
            {
                std::cerr << "requested api version 0." << apiVersion << ", which is not supported." << std::endl;
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
            //Neue RegExp zum Punkte herausholen...
            cloudmadeApiPointListExtractor.indexIn(cloudmadeApiPointListRegExp.cap(0));
            QString strLat, strLon;
            routePointList.clear();
            for (int pos=0; pos>=0; pos=cloudmadeApiPointListExtractor.indexIn(cloudmadeApiPointListRegExp.cap(0), cloudmadeApiPointListExtractor.cap(0).length()+pos))
            {
                strLat = cloudmadeApiPointListExtractor.cap(1);
                strLon = cloudmadeApiPointListExtractor.cap(2);
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
            routeType = cloudmadeApiRouteTypeRegExp.cap(1).toLower();
            routeModifier = cloudmadeApiRouteTypeRegExp.cap(2).toLower();
            routeDataType = cloudmadeApiRouteTypeRegExp.cap(3).toLower();
            //Routentyp gefunden. Auswerten!
        }
        else
        {
            this->send400();
            return;
        }
        
        //this->send102();
        
        boost::shared_ptr<RoutingMetric> metric;
        boost::shared_ptr<Router> router;
        boost::shared_ptr<DatabaseConnection> dbA;
        boost::shared_ptr<DatabaseConnection> dbB;
        boost::shared_ptr<AltitudeProvider> altitudeProvider;
        
        #ifdef ZZIP_FOUND
            altitudeProvider.reset(new SRTMProvider());
        #else
            altitudeProvider.reset(new ZeroAltitudeProvider());
        #endif
        
        if ((routeType == "bicycle") || (routeType == "bike"))
        {
            //altitudeProvider.reset(new ZeroAltitudeProvider());
            
            //Routingmetrik festlegen anhand der Benutzerwahl
            if ((routeModifier == "euclidean"))
            {
                metric.reset(new EuclideanRoutingMetric(altitudeProvider));
            }
            else if ((routeModifier == "simpleheight") || (routeModifier == "shortest"))
            {
                float detourPerHeightMeter = 100.0f;
                if (numberRegExp.indexIn(_parameterMap["detourperheightmeter"]) != -1)
                {
                    detourPerHeightMeter = numberRegExp.cap(1).toFloat();
                }
                metric.reset(new SimpleHeightRoutingMetric(altitudeProvider, detourPerHeightMeter));
            }
            else if (routeModifier == "advancedheight")
            {
                float punishment = 1.0f;
                float detourPerHeightMeter = 200.0f;
                if (numberRegExp.indexIn(_parameterMap["punishment"]) != -1)
                {
                    punishment = numberRegExp.cap(1).toFloat();
                }
                if (numberRegExp.indexIn(_parameterMap["detourperheightmeter"]) != -1)
                {
                    detourPerHeightMeter = numberRegExp.cap(1).toFloat();
                }
                metric.reset(new AdvancedHeightRoutingMetric(altitudeProvider, detourPerHeightMeter, punishment));
            }
            else if (routeModifier == "simplepower")
            {
                double weight = 90.0;
                double efficiency = 3 * weight;
                
                if (numberRegExp.indexIn(_parameterMap["weight"]) != -1)
                    weight = numberRegExp.cap(1).toDouble();
                if (numberRegExp.indexIn(_parameterMap["efficiency"]) != -1)
                    efficiency = numberRegExp.cap(1).toDouble();
                metric.reset(new SimplePowerRoutingMetric(altitudeProvider, weight, efficiency));
            }
            else if ((routeModifier == "power") || (routeModifier == "") || (routeModifier == "fastest"))
            {
                double weight = 90.0;
                //double maxPower = 140.0;
                double maxPower = 150.0;
                double minSpeed = 2.5;
                double pushBikeSpeed = 0.5;
                double haltungskorrekturfaktor = 0.4;
                double maxSpeed = -1.0;
                double noCyclewayPunishmentFactor = 5;
                
                if (numberRegExp.indexIn(_parameterMap["weight"]) != -1)
                    weight = numberRegExp.cap(1).toDouble();
                if (numberRegExp.indexIn(_parameterMap["maxpower"]) != -1)
                    maxPower = numberRegExp.cap(1).toDouble();
                if (numberRegExp.indexIn(_parameterMap["minspeed"]) != -1)
                    minSpeed = numberRegExp.cap(1).toDouble();
                if (numberRegExp.indexIn(_parameterMap["maxspeed"]) != -1)
                    maxSpeed = numberRegExp.cap(1).toDouble();
                if (numberRegExp.indexIn(_parameterMap["pushbikespeed"]) != -1)
                    pushBikeSpeed = numberRegExp.cap(1).toDouble();
                if (numberRegExp.indexIn(_parameterMap["nocyclewaypunishmentfactor"]) != -1)
                    noCyclewayPunishmentFactor = numberRegExp.cap(1).toDouble();
                if (numberRegExp.indexIn(_parameterMap["haltungskorrekturfaktor"]) != -1)
                    haltungskorrekturfaktor = numberRegExp.cap(1).toDouble();
                metric.reset(new PowerRoutingMetric(altitudeProvider, weight, maxPower, minSpeed, pushBikeSpeed, haltungskorrekturfaktor, noCyclewayPunishmentFactor ,maxSpeed));
            }
            else if ((routeModifier == "biketourpower") || (routeModifier == "biketour"))
            {
                double weight = 90.0;
                //double maxPower = 140.0;
                double maxPower = 100.0;
                double minSpeed = 2.5;
                double pushBikeSpeed = 0.5;
                double haltungskorrekturfaktor = 0.4;
                double maxSpeed = -1.0;
                double noCyclewayPunishmentFactor = 7;
                
                if (numberRegExp.indexIn(_parameterMap["weight"]) != -1)
                    weight = numberRegExp.cap(1).toDouble();
                if (numberRegExp.indexIn(_parameterMap["maxpower"]) != -1)
                    maxPower = numberRegExp.cap(1).toDouble();
                if (numberRegExp.indexIn(_parameterMap["minspeed"]) != -1)
                    minSpeed = numberRegExp.cap(1).toDouble();
                if (numberRegExp.indexIn(_parameterMap["maxspeed"]) != -1)
                    maxSpeed = numberRegExp.cap(1).toDouble();
                if (numberRegExp.indexIn(_parameterMap["pushbikespeed"]) != -1)
                    pushBikeSpeed = numberRegExp.cap(1).toDouble();
                if (numberRegExp.indexIn(_parameterMap["nocyclewaypunishmentfactor"]) != -1)
                    noCyclewayPunishmentFactor = numberRegExp.cap(1).toDouble();
                if (numberRegExp.indexIn(_parameterMap["haltungskorrekturfaktor"]) != -1)
                    haltungskorrekturfaktor = numberRegExp.cap(1).toDouble();
                metric.reset(new BikeTourPowerRoutingMetric(altitudeProvider, weight, maxPower, minSpeed, pushBikeSpeed, haltungskorrekturfaktor, noCyclewayPunishmentFactor ,maxSpeed));
            }
            else
            {
                std::cerr << "routeModifier \"" << routeModifier << "\" not supported." << std::endl;
                this->send405();
                return;
            }
        }
        else if (routeType == "car")
        {
            //TODO
            this->send405();
            return;
        }
        else if (routeType == "foot")
        {
            if ((routeModifier == "euclidean") || (routeModifier == "") || (routeModifier == "shortest") || (routeModifier == "fastest"))
            {
                metric.reset(new EuclideanRoutingMetric(altitudeProvider));
            }
        }
        else
        {
            std::cerr << "requested routeType=" << routeType << ", which is not supported." << std::endl;
            this->send405();
            return;
        }
        
        #ifdef SPATIALITE_FOUND
            if (ProgramOptions::getInstance()->dbBackend == "spatialite")
            {
                dbA.reset(new SpatialiteDatabaseConnection());
                dbB.reset(new SpatialiteDatabaseConnection());
            }
            else 
        #endif
        if (ProgramOptions::getInstance()->dbBackend == "sqlite")
        {
            dbA.reset(new SQLiteDatabaseConnection());
            dbB.reset(new SQLiteDatabaseConnection());
        }
        //Datenbank ist die globale DB...
        dbA->open(ProgramOptions::getInstance()->dbFilename.c_str());
        dbB->open(ProgramOptions::getInstance()->dbFilename.c_str());
        
        //TODO: Testen, ob das mit dem Cache überhaupt was bringt...
        dbA = boost::shared_ptr<DatabaseConnection>(new DatabaseRAMCache(dbA, ProgramOptions::getInstance()->dbCacheSize));
        dbB = boost::shared_ptr<DatabaseConnection>(new DatabaseRAMCache(dbB, ProgramOptions::getInstance()->dbCacheSize));
        
        //Routingalgorithmus heraussuchen, je nach Angabe. Standard: Mehrthread-A* oder Mehrthread-Dijkstra - je nach Metrik.
        if (_parameterMap["algorithm"] == "multithreadeddijkstra")
            router.reset(new MultithreadedDijkstraRouter(dbA, dbB, metric));
        else if (_parameterMap["algorithm"] == "dijkstra")
            router.reset(new DijkstraRouter(dbA, metric));
        else if (_parameterMap["algorithm"] == "astar")
            router.reset(new AStarRouter(dbA, metric));
        else if (_parameterMap["algorithm"] == "multithreadedastar")
            router.reset(new MultithreadedAStarRouter(dbA, dbB, metric));
        else
        {
            if (metric->getMeasurementUnit() == DISTANCE)
                router.reset(new MultithreadedAStarRouter(dbA, dbB, metric));
            else
                router.reset(new MultithreadedDijkstraRouter(dbA, dbB, metric));
        }
        
        //Route berechnen
        GPSRoute route = router->calculateShortestRoute(routePointList);
        //Keine Route gefunden? 404 senden.
        if (route.isEmpty())
        {
            std::cerr << "no route found." << std::endl;
            this->send404();
            return;
        }
        else
        {
            std::cerr << "found route." << std::endl
                << "  length: " << route.calcLength()/1000.0 << "km" << std::endl
                << "  duration: " << route.getDuration()/60.0 << "min" << std::endl
                << "  has " << route.getSize() << " points." << std::endl;
        }
        
        //Antwort entsprechend des Routentypen senden.
        if (routeDataType == "gpx")
            this->sendFile(route.exportGPXString(altitudeProvider));
        else if (routeDataType == "js")
            this->sendFile(route.exportJSONString());
        else
            std::cerr << "route datatype \"" << routeDataType  << 
                "\" not supported." << std::endl;
        return;
    }
}

template class HttpServerThread<BikerHttpRequestProcessor>;

namespace biker_tests
{
    int testWebServer()
    {
        std::cerr << "Testing Webserver..." << std::endl;
        
        ProgramOptions::getInstance()->webserver_public_html_folder = "./gui/";
        HttpServerThread<BikerHttpRequestProcessor> server(8081);
        server.startServer();
        //todo: aufs hochfahren warten toller lösen als so
        
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
        
        return EXIT_SUCCESS;
    }
}
