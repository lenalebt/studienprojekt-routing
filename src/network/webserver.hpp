#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QRunnable>
#include <QThreadPool>
#include <QFile>
#include "tests.hpp"
#include <boost/cstdint.hpp>

/**
 * @brief Diese Klasse stellt einen nebenläufig arbeitenden
 *      Http-Server dar, der jeden Request als eigenen Thread
 *      abarbeitet.
 * 
 * 
 * @remarks Diese Klasse sollte nicht direkt angesprochehn werden, sondern
 *      nur über HttpServerThread aufgerufen werden!
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

/**
 * @brief Diese Klasse stellt einen ServerThread dar und sollte aufgerufen werden,
 *      um einen neuen Server zu starten.
 * 
 * @ingroup name
 * @author Lena Brueder
 * @date 2012-01-08
 * @copyright GNU GPL v3
 * @ingroup network
 */
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
    void startServer();
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
    
    QString _requestType;
    QString _requestPath;
    QString _httpVersion;
    QMap<QString, QString> _parameterMap;
    QMap<QString, QString> _headerMap;
    
    /**
     * @brief Liest eine Zeile aus dem socket, so verfügbar.
     * @param socket Das Socket, das verwendet werden soll
     * @param line[out] Hier wird die Zeile reingeschrieben, die gelesen wurde
     * @return Ob eine Zeile gelesen werden konnte
     */
    bool readLine(QTcpSocket* socket, QString& line);
    /**
     * @brief Schreibt einen String auf das Socket.
     * @param socket Das Socket, das verwendet werden soll
     * @param str Der String, der geschrieben werden soll
     */
    void writeString(QTcpSocket* socket, QString str);
    
    
    /**
     * @brief Sendet eine Datei über die Verbindung, inklusive der richtigen
     *      Header usw.
     * 
     * @param file Die Datei, die gesendet werden soll.
     * @return Ob das Senden erfolgreich war, oder nicht.
     * @todo Implementieren!
     */
    bool sendFile(QFile& file);
    
    /**
     * @brief Schickt eine 400 (Bad Request)-Nachricht mit kleiner
     *      Webseite an den Peer.
     * 
     * Die Verbindung sollte nach dem Versenden geschlossen werden.
     */
    void send400();
    
    /**
     * @brief Schickt eine 404-Nachricht mit kleiner Webseite an den Peer.
     * 
     * Die Verbindung sollte nach dem Versenden geschlossen werden.
     */
    void send404();
    
    /**
     * @brief Schickt eine 405-Nachricht mit kleiner Webseite an den Peer.
     * 
     * Die Verbindung sollte nach dem Versenden geschlossen werden.
     */
    void send405();
public:
    HttpRequestProcessor(int socketDescriptor);
    void run();
    
    /**
     * @brief Diese Funktion wird aufgerufen, um ein wenig Vorverarbeitung zu machen.
     * 
     * Hier wird sowas gemacht wie parsen von Headern und Parametern,
     * sowie heraussuchen des Pfades der geöffnet werden soll und sowas.
     * 
     */
    bool preprocessRequest();
    
    /**
     * @brief Diese Funktion wird aufgerufen, um einen neuen Request zu
     *      bearbeiten.
     * 
     * Dabei sind ein paar Vorverarbeitungsschritte schon gemacht worden:
     *  - Alle Header wurden bearbeitet. Sie können in
     *      <code>_headerMap</code> abgerufen werden.
     *  - Weiteres Lesen vom Socket liefert die Daten, die nach dem Header kommen
     *  - Alle Parameter wurden geparst. Sie können in
     *      <code>_parameterMap</code> abgerufen werden.
     *  - Der Pfad wurde in <code>_requestPath</code> abgelegt.
     *  - Der Typ des Requests wurde in <code>_requestType</code> abgelegt.
     * 
     * @todo Implementieren!
     */
    virtual void processRequest()=0;
};

/**
 * @brief Diese Klasse stellt einen Request an Biker dar.
 * 
 * 
 * 
 * @ingroup network
 * @author Lena Brueder
 * @date 2012-01-08
 * @copyright GNU GPL v3
 * @todo Implementierung!
 */
class BikerHttpRequestProcessor : public HttpRequestProcessor
{
public:
    static QString publicHtmlDirectory;
    BikerHttpRequestProcessor(int socketDescriptor) :
        HttpRequestProcessor(socketDescriptor) {}
    void processRequest();
};

namespace biker_tests
{
    /**
     * @ingroup tests
     */
    int testWebServer();
}

#endif //WEBSERVER_HPP
