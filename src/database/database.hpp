#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "gpsposition.hpp"
#include "routingnode.hpp"
#include "routingedge.hpp"
#include <boost/shared_ptr.hpp>
#include <QString>
#include <QVector>
#include <boost/cstdint.hpp>

/**
 * @brief Diese Klasse ist ein Interface für alle Formen von Datenbankverbindungen.
 * 
 * 
 * 
 * @ingroup database
 * @author Lena Brueder
 * @date 2011-11-03
 * @copyright GNU GPL v3
 * @todo Möglicherweise muss man sich noch Gedanken machen, wie man hier
 *      mit mehreren Threads zugreifen kann. Muss man das Interface ändern?
 *      Evtl reicht es, einen Wrapper zu schreiben, der das Interface threadsicher
 *      macht.
 */
class DatabaseConnection
{
private:
    
public:
    /**
     * @brief Schließt eine Verbindung zur Datenbank
     */
    virtual void close()=0;
    
    /**
     * @brief Öffnet eine Verbindung zur Datenbank.
     * 
     * 
     * 
     * @param dbConnectionString Gibt an, welche Datenbank geöffnet werden soll.
     *      Kann ein Dateiname sein (z.B. SQLite) oder Benutzername/Passwort
     *      und Adresse (z.B. PostgeSQL).
     */
    virtual void open(QString dbConnectionString)=0;
    /**
     * @brief Gibt an, ob gerade eine Datenbank geöffnet ist oder nicht.
     * @return Ob die Datenbank geöffnet ist.
     */
    virtual bool isDBOpen()=0;
    
    /**
     * @brief Lädt aus der DB den Knoten mit der angegebenen ID.
     * 
     * Wenn die ID im kurzen Format war, wird einfach der Knoten geladen.
     * War er im langen Format, wird das kurze Format gebildet und dieser
     * Knoten dann geladen.
     * 
     * @return Den herausgesuchten Knoten, so er gefunden wurde. Sonst ist
     *      der Zeiger nicht initialisiert (-> 0).
     */
    virtual boost::shared_ptr<RoutingNode> getNodeByID(boost::uint64_t id)=0;
    
    /**
     * @brief Sucht aus der Datenbank alle Knoten heraus, die sich im Umkreis von
     *      <code>radius</code> Metern um den Suchmittelpunkt befinden.
     * 
     * 
     * @remarks Es ist zulässig, mehr Punkte zurückzugeben als sich im <code>radius</code>
     *      befinden. Dies ist beispielweise möglich, wenn statt einem Kreis ein umgebendes
     *      Rechteck berechnet wird und die dort enthaltenen Punkte zurückgegeben werden.
     * @param searchMidpoint Der Suchmittelpunkt
     * @param radius Der Radius um den Suchmittelpunkt, in dem gesucht wird.
     * @return Eine Liste mit mindestens allen Punkten innerhalb des angegebenen Kreises.
     */
    virtual QVector<boost::shared_ptr<RoutingNode> > getNodes(const GPSPosition& searchMidpoint, double radius)=0;
    
    /**
     * @brief Sucht aus der Datenbank alle Knoten heraus, die sich im
     *      angegebenen umgebenden Rechteck befinden.
     * 
     * @param ulCorner Die obere linke Ecke des Rechtecks (upper left)
     * @param brCorner Die untere rechte Ecke des Rechtecks (bottom right)
     * @return Eine Liste mit allen Punkten innerhalb des Rechtecks.
     */
    virtual QVector<boost::shared_ptr<RoutingNode> > getNodes(const GPSPosition& ulCorner, const GPSPosition& brCorner)=0;
    
    /**
     * @brief Legt einen Knoten in der Datenbank ab.
     * @param node Ein Knoten, der in der Datenbank abgelegt wird
     * @return Ob Speichern erfolgreich war
     */
    virtual bool saveNode(const RoutingNode& node)=0;
    
    /**
     * @brief Gibt alle Kanten zurück, die von dem Knoten mit angegebener ID
     *      starten.
     * @param startNodeID Die ID des Startknotens der Kante
     * @return Eine Liste mit Kanten, die im angegebenen Knoten starten.
     */
    virtual QVector<boost::shared_ptr<RoutingEdge> > getEdgesByStartNodeID(boost::uint64_t startNodeID)=0;
    
    /**
     * @brief Gibt alle Kanten zurück, die in dem Knoten mit angegebener ID
     *      enden.
     * 
     * Wird benötigt, um z.B. Dijkstra von 2 Seiten aus starten zu können.
     * 
     * @param endNodeID Die ID des Endknotens der Kante.
     * @return Eine Liste mit Kanten, die im angegebenen Knoten enden.
     */
    virtual QVector<boost::shared_ptr<RoutingEdge> > getEdgesByEndNodeID(boost::uint64_t endNodeID)=0;
    
    /**
     * @brief Gibt die Kante zurück, die die angegebene ID hat.
     * 
     * 
     * 
     * @param edgeID Die ID der Kante, die herausgesucht werden soll.
     * @return Die Kante, die herausgesucht werden sollte. Wenn die Kante nicht gefunden werden konnte
     *      enthält der shared_ptr einen Nullzeiger.
     */
    virtual boost::shared_ptr<RoutingEdge> getEdgeByEdgeID(boost::uint64_t edgeID)=0;
    
    /**
     * @brief Speichert die übergebene Kante in der Datenbank.
     * @param edge Die Kante, die in der Datenbank abgelegt werden soll.
     * @return Ob Speichern erfolgreich war.
     */
    virtual bool saveEdge(const RoutingEdge& edge)=0;
    
    /**
     * @brief Speichert die übergebene Kante in der Datenbank und legt auch den Straßennamen ab.
     * @param edge Die Kante, die in der Datenbank abgelegt werden soll.
     * @param name Der Name der Straße, zu der diese Kante gehört.
     * @return Ob Speichern erfolgreich war.
     */
    virtual bool saveEdge(const RoutingEdge& edge, QString name)=0;
    
    /**
     * @brief Gibt zu einer angegebenen Kante den Namen der Straße zurück, sofern es einen gibt.
     * @return den Namen der zugehörigen Straße, sofern es einen gibt
     */
    virtual QString getStreetName(const RoutingEdge& edge)=0;
    
	/**
	 * @brief Entfernt eine bestimmte Kante aus der Datenbank.
	 * @return Ob das Loeschen erfolgreich war, oder nicht.
	 */
    virtual bool deleteEdge(boost::uint64_t startNodeID, boost::uint64_t endNodeID)=0;
    
    /**
     * @brief Beginnt eine neue Transaktion auf der Datenbank.
     * 
     * Wenn große Datenmengen in der Datenbank abgelegt werden
     * sollen ist sie wesentlich schneller, wenn man viele Daten
     * in einer Transaktion ablegt. Daher sollten Transaktionen
     * immer verwendet werden, wenn mehr als nur zwei oder drei
     * Datensätze in der Datenbank abgelegt werden.
     * 
     * @return Ob das Starten einer Transaktion erfolgreich war,
     *  oder nicht
     */
    virtual bool beginTransaction()=0;
    
    /**
     * @brief Beendet die letzte Transaktion auf der Datenbank.
     * 
     * Wenn große Datenmengen in der Datenbank abgelegt werden
     * sollen ist sie wesentlich schneller, wenn man viele Daten
     * in einer Transaktion ablegt. Daher sollten Transaktionen
     * immer verwendet werden, wenn mehr als nur zwei oder drei
     * Datensätze in der Datenbank abgelegt werden.
     * 
     * @return Ob das Starten einer Transaktion erfolgreich war,
     *  oder nicht
     */
    virtual bool endTransaction()=0;
};

#endif //DATABASE_HPP 
