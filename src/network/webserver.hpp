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
 * @page webservicedocumentation Dokumentation des Webservers
 * 
 * @tableofcontents
 * 
 * Hier wird beschrieben, wie der Webserver zu behandeln ist, und welche
 * Funktionen er bietet. Zusätzlich wird die API des zur Verfügung gestellten
 * Webservices beschrieben.
 * 
 * @section webserver_functions Funktionen des Webservers
 * 
 * @subsection webserver_functions_files Ausliefern von Dateien
 * Der Webserver kann Dateien ausliefern, die in einem bestimmten Verzeichnis
 * abgelegt sind. Dieses Verzeichnis kann man beim Start des Programms
 * angeben mit dem Parameter
 * <code>--webserver-public-html-folder</code> (siehe auch
 * <code>biker --help</code>). Standardmäßig ist dieser Ordner eingestellt
 * auf ein Verzeichnis <code>./gui/</code> relativ zum Arbeitsverzeichnis
 * des Programms. Alle Dateien, die in diesem Verzeichnis liegen, werden
 * vom Webserver ausgeliefert. Die Dateien sind erreichbar über
 * @verbatim
http://server/files/
@endverbatim
 * gefolgt von dem Dateinamen. Es ist möglich, Unterordner zu verwenden.
 * @todo Aufschreiben, wie Dateien ausgeliefert werden, und wo das alles so liegt
 * @todo beschreiben was "server" bedeuten soll
 * @subsubsection webserver_security Sicherheitsfeatures
 * @todo Sicherheitsfeatures aufschrieben
 * @subsection webserver_functions_routes Ausliefern von Routen
 * @todo Aufschreiben, wie die API des Webservices funktioniert
 * @author Lena Brüder
 */

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
     * @remarks Liest maximal 4KiB Daten in einer Zeile, alles danach wird abgeschnitten.
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
     */
    bool sendFile(QFile& file);
    
    /**
     * @brief Sendet eine Datei über die Verbindung, inklusive der richtigen
     *      Header usw.
     * 
     * @param content Inhalt der Datei, die gesendet werden soll.
     * @return Ob das Senden erfolgreich war, oder nicht.
     * @bug Probleme bei der Übertragung: Z.B. bei Byte 212992 bricht die Übertragung ab.
     */
    bool sendFile(const QString& content);
    
    /**
     * @brief Schickt eine 400 (Bad Request)-Nachricht mit kleiner
     *      Webseite an den Peer.
     * 
     * Die Verbindung sollte nach dem Versenden geschlossen werden.
     */
    void send400();
    
    /**
     * @brief Schickt eine 403-Nachricht mit kleiner Webseite an den Peer (403 Access forbidden).
     * 
     * Die Verbindung sollte nach dem Versenden geschlossen werden.
     */
    void send403();
    
    /**
     * @brief Schickt eine 404-Nachricht mit kleiner Webseite an den Peer (404 not found).
     * 
     * Die Verbindung sollte nach dem Versenden geschlossen werden.
     */
    void send404();
    
    /**
     * @brief Schickt eine 405-Nachricht mit kleiner Webseite an den Peer (405 Method not allowed).
     * 
     * Die Verbindung sollte nach dem Versenden geschlossen werden.
     */
    void send405();
    
    /**
     * @brief Schickt eine 102-Nachricht an den Peer (102 Processing).
     * 
     * Wird gesendet, wenn eine zeitintensive Anfrage gestartet wurde.
     */
    void send102();
    
    /**
     * @brief Schickt eine 500-Nachricht an den Peer (500 Internal Server Error).
     * 
     * Die Verbindung sollte nach dem Versenden geschlossen werden.
     */
    void send500();
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
     * @remarks Es werden nicht mehr als 127 Header-Zeilen erlaubt
     * @remarks Es werden nicht mehr als 4KiB Daten pro Zeile angenommen.
     *      Bei 4095 Bytes Daten oder mehr in einer Zeile wird der Request abgelehnt.
     * @todo Request ablehnen, wenn er größer als 4KiB wird.
     */
    virtual void processRequest()=0;
};

/**
 * @brief Diese Klasse stellt einen Request an Biker dar.
 * 
 * Diese Klasse arbeitet folgendermaßen: Sie beantwortet nur HTTP GET-Anfragen,
 * und kann dabei:
 *   - Dateien vom Dateisystem ausliefern
 *   - dynamische Anfragen beantworten
 * 
 * Beim Ausliefern von Dateien müssen diese in einem
 * speziellen Verzeichnis liegen, das über den Aufrufparameter
 * <code>--webserver-public-html-folder</code> beim Starten der
 * Software festgelegt wird (dieser Aufruf legt
 * BikerHttpRequestProcessor::publicHtmlDirectory auf
 * einen Wert fest).
 * Nur Dateien in diesem Verzeichnis können
 * über eine HTTP-GET-Anfrage ausgeliefert werden. Wenn eine Anfrage
 * an den Webserver im <code>_requestPath</code> mit "/files/" beginnt,
 * werden Dateien aus dem freigegebenen Verzeichnis versendet.
 * Wird eine Datei nicht gefunden, oder ist sie nicht mit Leserechten für
 * jeden Benutzer (nicht-Besitzer und nicht-Gruppenmitglied) ausgestattet,
 * so wird sie nicht ausgeliefert. Eine solche Anfrage wird mit HTTP 404
 * beantwortet. Ebenso werden Anfragen beantwortet, die auf ein Verzeichnis
 * treffen. Directory-Listings werden nicht unterstützt.
 * 
 * Alle Anfragen, die nicht in das obige Schema passen, werden als dynamische
 * Anfragen behandelt und werden gesondert behandelt.
 * 
 * @ingroup network
 * @author Lena Brueder
 * @date 2012-01-08
 * @copyright GNU GPL v3
 */
class BikerHttpRequestProcessor : public HttpRequestProcessor
{
public:
    /**
     * @brief Diese Eigenschaft legt das öffentliche Verzeichnis des Webservers fest.
     * 
     * @attention Diese Eigenschaft vorsichtig behandeln. Um nicht versehentlich
     *      wichtige Daten auszuliefern werden nur solche Dateien ausgeliefert,
     *      die für alle Benutzer des Systems lesbar sind.
     */
    BikerHttpRequestProcessor(int socketDescriptor) :
        HttpRequestProcessor(socketDescriptor) {}
    /**
     * @remarks Es werden nur GET-Anfragen beantwortet. Alle anderen werden mit
     *      HTTP 405 beantwortet.
     */
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
