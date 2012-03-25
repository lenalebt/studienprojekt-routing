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
 * Webservices beschrieben, und aus welchen Gründen überhaupt ein
 * Webserver in das Programm integriert wurde.
 * 
 * @section webserver_why Warum wurde ein Webserver in das Programm integriert?
 * Der integrierte Webserver hat vor allem Vorteile für Benutzer, die nicht
 * sehr erfahren mit der Einrichtung eines Webservers haben. Dies sind
 * für einen normalen PC-Benutzer keine Standardkenntnisse. Ziel war, das
 * Starten des Programms möglichst einfach zu gestalten. Im Idealfall
 * ist nur nötig, nach der Installation des Programms eine Datenbank
 * herunterzuladen (<code>->database.db</code>),
 * das Programm mittels <code>biker</code> zu starten,
 * und einen Webbrowser auf die Adresse
 * <code>http://localhost:8080/files/gui.html</code> zu leiten.
 * 
 * Da das Programm vor allem auch lokal funktionieren soll, und nicht nur
 * als Serverdienst auf einem entfernten Rechner, ist ein integrierter
 * Webserver von Vorteil.
 * 
 * Für die Zukunft wäre trotzdem wünschenswert, auch die CGI-Schnittstelle
 * eines normalen Webservers zu unterstützen (siehe auch
 * \ref functions_properties_planned). Ein besser getesteter Webserver ist
 * für einen öffentlich verfügbaren Service eine deutlich bessere Wahl,
 * als der integrierte.
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
 * Dabei bezeichnet <code>server</code> die Serveradresse, wie in
 * \ref benutzung_gui beschrieben.
 * 
 * Es werden aus Sicherheitsgründen nicht einfach alle Dateien ausgeliefert,
 * näheres ist in \ref webserver_security.
 * 
 * 
 * Anfragen, die nicht mit <code>/files/</code> beginnen, werden als dynamische
 * Requests behandelt und wie in \ref webserver_functions_routes
 * beschrieben behandelt.
 * 
 * @subsection webserver_functions_routes Ausliefern von Routen
 * Die Routing-API verhält sich zum großen Teil so, wie die API zum Erstellen
 * von Routen von Cloudmade, die unter
 * <a href="http://developers.cloudmade.com/wiki/routing-http-api/Documentation">Cloudmade Routing API Documentation</a>
 * beschrieben ist. Dennoch gibt es ein paar Unterschiede, die in \ref webserver_functions_routes_cloudmade_differences
 * beschrieben werden.
 * 
 * @code
 * http://yourhostgoeshere:yourportgoeshere/yourapikeygoeshere/api/0.3/start_point,[transit_point1,...,transit_pointN],end_point/route_type[/route_type_modifier].output_format[?algorithm=value&parameter=value]
 * @endcode
 * 
 * <table>
 *  <tr><th>Befehlsteil</th><th>Funktion</th></tr>
 *  <tr><td><code>yourhostgoeshere</code></td><td>An dieser Stelle wird der Host eingetragen, auf dem der Service läuft.</td></tr>
 *  <tr><td><code>yourportgoeshere</code></td><td>Hier wird der Port angegeben, auf dem der Service lauscht.</td></tr>
 *  <tr><td><code>yourapikeygoeshere</code></td><td>Hier muss ein Key angegeben werden, der aus alphanumerischen Zeichen besteht. Es sind alle Buchstaben und Zahlen erlaubt. Es müssen zwischen 1 und 64 Zeichen angegeben werden.
 *      Standardmäßig akzeptiert der Server alle Keys, er kann jedoch beschränkt werden durch den Aufrufparameter <code>--webserver-api-key</code> beim Starten des Programms.</td></tr>
 *  <tr><td><code>start_point</code></td><td>Der Startpunkt der Route, zuerst Breiten- und dann Längengrad. Beispiel: <code>51.0,7.2</code></td></tr>
 *  <tr><td><code>transit_pointN</code></td><td>Ein Zwischenpunkt in der Route, Format wie bei <code>start_point</code>. Transitpunkte werden durch Komma getrennt,
 *      alle Transitpunkte werden in eckige Klammern eingeschlossen. Beispiel: <code>[51.0,7.2,51.2,7.3]</code></td></tr>
 *  <tr><td><code>end_point</code></td><td>Der Endpunkt, Format wie bei <code>start_point</code>.</td></tr>
 *  <tr><td><code>output_format</code></td><td>Das Ausgabeformat der Route. Hier ist erlaubt: <code>js</code>,<code>gpx</code></td></tr>
 *  <tr><td><code>route_type</code></td><td>Typ der Route. Hier ist vornehmlich <code>bike</code>
 *      oder <code>bicycle</code> vorgesehen, aber auch <code>foot</code> und <code>car</code> ist
 *      möglich. Dabei werden die letztgenannten Typen eher stiefmütterlich behandelt, es sollten keine
 *      allzu großen Erwartungen an sie gestellt werden. Hier ist die Cloudmade-API wesentlich besser geeignet.</td></tr>
 *  <tr><td><code>route_modifier</code></td><td>Muss nicht angegeben werden, wird nur bei Routen vom Typ <code>bike</code> oder <code>bicycle</code>
 *      beachtet und sonst ignoriert. Mögliche Werte: <code>power</code>, <code>simpleheight</code>, <code>advancedheight</code>, <code>euclidian</code>.
 *      Es ist möglich, dass sich die Liste der erlaubten Metriken erweitert.</td></tr>
 *  <tr><td><code>algorithm=value</code></td><td>Hier wird der zur Berechnung verwendete Algorithmus angegeben.
 *      Mögliche Werte: <code>standard</code> wählt abhängig von der verwendeten Metrik einen geeigneten Algorithmus aus.
 *      <table>
     *      <tr><td><code>dijkstra</code></td><td>wählt den Algorithmus von Dijkstra.</td></tr>
     *      <tr><td><code>astar</code></td><td>wählt den A*-Algorithmus.</td></tr>
     *      <tr><td><code>multithreadeddijkstra</code></td><td>wählt eine Version von Dijkstra, die mit 2 Threads arbeitet. Auf geeigneter Hardware (>2 Prozessoren, viel RAM für Datenbankcaching)
     *      etwa 4x schneller als Dijkstra - auf weniger gut geeigneter Hardware (1 Prozessor, wenig Speicher, langsame Festplatte) immerhin noch 2x schneller.</td></tr>
     *      <tr><td><code>multithreadedastar</code></td><td>wählt eine Version von A*, die mit 2 Threads arbeitet.</td></tr>
 *      </table>
 *      </td></tr>
 *  <tr><td><code>parameter=value</code></td><td>Diese Parameter sind abhängig von der Routingmetrik und bei der entsprechenden Metrik erklärt.</td></tr>
 * </table>
 * 
 * @todo Auf die Parameter der einzelnen Routingmetriken eingehen!
 * @subsubsection webserver_functions_routes_cloudmade_differences Unterschiede zur Cloudmade-API
 * @todo Die Unterschiede hinschreiben (z.B. Callbackfunktion, Einheiten, Sprache)
 * 
 * @subsection webserver_security Sicherheitsfeatures
 * Da ein Webserver, der einfach alle Dateien eines Ordners ausliefern kann, ein
 * potentielles Risiko für die Sicherheit eines Systems ist, sind ein paar
 * Hürden für potentielle Angreifer eingebaut worden.
 * 
 * - Es werden nur Dateien ausgeliefert, die von jedem Benutzer des Systems
 *   lesbar sind. Unter Unix bedeutet dies, dass alle Dateien mindestens o+r-Rechte
 *   (<code>chmod o+r <i>datei</i></code>) besitzen müssen, damit der Webserver sie ausliefert.
 *   Dateien, die diese Rechte nicht besitzen, werden nicht ausgeliefert, eine entsprechende
 *   Anfrage wird mit <code>404 not found</code> beantwortet.
 * 
 * - Ebenso werden Anfragen an Ordner mit <code>404 not found</code> beantwortet. Directory Listings
 *   werden nicht erstellt.
 * 
 * - Es werden nur <code>GET</code>-Anfragen beantwortet. Andere Anfragen erhalten
 *   <code>405 Method not allowed</code> als Antwort.
 * 
 * - Anfragen, die ein <code>..</code> enthalten, werden mit <code>400 Bad request</code>
 *   beantwortet. Dies soll sicherstellen, dass ein Angreifer nicht einfach in das
 *   Dateisystem ausbrechen kann und andere Dateien auslesen, als die vorgesehenen.
 * 
 * - Es sind nur 127 Headerzeilen erlaubt. Wer mehr Headerzeilen angibt, wird als Angreifer
 *   betrachtet. Auch hier erhält er <code>400 Bad request</code> als Antwort.
 * 
 * - Es ist nicht erlaubt, in einer Anfrage mehr als 4KiB Daten pro Zeile zu verschicken.
 *   Dies schließt auch die Headerzeile ein.
 *   Normale Anfragen an diesen Server sind viel kürzer. Wer so viele Daten pro Zeile in einem
 *   Request an diesen Server schickt, wird mit <code>400 Bad request</code> begrüßt.
 * 
 * - Der Server bearbeitet nur Anfragen weiter, die sich mit einem korrekten HTTP-Header
 *   melden. Auch hier: Ist dies nicht erfüllt, antwortet der Server mit <code>400 Bad request</code>.
 * 
 * Diese Features verstehen sich als zusätzliche Hürden für einen Angreifer.
 * Natürlich können Programmfehler immer dazu führen, dass ein Programm
 * dazu gebracht werden kann, Dinge zu tun, für die es nicht gedacht war.
 * Solche Fehler werden nach Bekanntwerden so schnell wie möglich behoben.
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
 * @todo eine Option <code>--no-serve-files</code> in den Server einbauen,
 *  der verhindert dass Dateien ausgeliefert werden (->Sicherheit)
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
     * @relates HttpServerThread
     */
    int testWebServer();
}

#endif //WEBSERVER_HPP
