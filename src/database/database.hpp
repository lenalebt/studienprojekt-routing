#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "gpsposition.hpp"
#include "routingnode.hpp"
#include "routingedge.hpp"
#include <boost/shared_ptr.hpp>
#include <QString>
#include <QList>
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
    virtual QList<boost::shared_ptr<RoutingNode> > getNodes(const GPSPosition& searchMidpoint, double radius)=0;
    
    /**
     * @brief Sucht aus der Datenbank alle Knoten heraus, die sich im
     *      angegebenen umgebenden Rechteck befinden.
     * 
     * @param ulCorner Die obere linke Ecke des Rechtecks (upper left)
     * @param brCorner Die untere rechte Ecke des Rechtecks (bottom right)
     * @return Eine Liste mit allen Punkten innerhalb des Rechtecks.
     */
    virtual QList<boost::shared_ptr<RoutingNode> > getNodes(const GPSPosition& ulCorner, const GPSPosition& brCorner)=0;
    
    /**
     * @brief Legt einen Knoten in der Datenbank ab.
     * @param node Ein Knoten, der in der Datenbank abgelegt wird
     */
    virtual void saveNode(const RoutingNode& node);
    
    /**
     * @brief Gibt alle Kanten zurück, die von dem Knoten mit angegebener ID
     *      starten.
     * @param startNodeID Die ID des Startknotens der Kante
     * @return Eine Liste mit Kanten, die im angegebenen Knoten starten.
     */
    virtual QList<boost::shared_ptr<RoutingEdge> > getEdgesByStartNodeID(boost::uint64_t startNodeID)=0;
    
    /**
     * @brief Gibt alle Kanten zurück, die in dem Knoten mit angegebener ID
     *      enden.
     * 
     * Wird benötigt, um z.B. Dijkstra von 2 Seiten aus starten zu können.
     * 
     * @param endNodeID Die ID des Endknotens der Kante.
     * @return Eine Liste mit Kanten, die im angegebenen Knoten enden.
     */
    virtual QList<boost::shared_ptr<RoutingEdge> > getEdgesByEndNodeID(boost::uint64_t endNodeID)=0;
    
    /**
     * @brief Gibt die Kante zurück, die die angegebene ID hat.
     * 
     * 
     * 
     * @param edgeID Die ID der Kante, die herausgesucht werden soll.
     * @return Die Kante, die herausgesucht werden sollte. Wenn die Kante nicht gefunden werden konnte
     *      enthält der shared_ptr einen Nullzeiger.
     */
    virtual boost::shared_ptr<RoutingEdge> getEdgesByEdgeID(boost::uint64_t edgeID)=0;
    
    /**
     * @brief Speichert die übergebene Kante in der Datenbank.
     * @param edge Die Kante, die in der Datenbank abgelegt werden soll.
     */
    virtual void saveEdge(const RoutingEdge& edge);
    
    /**
     * @brief Speichert die übergebene Kante in der Datenbank und legt auch den Straßennamen ab.
     * @param edge Die Kante, die in der Datenbank abgelegt werden soll.
     * @param name Der Name der Straße, zu der diese Kante gehört.
     */
    virtual void saveEdge(const RoutingEdge& edge, QString name);
    
};

#endif //DATABASE_HPP 
