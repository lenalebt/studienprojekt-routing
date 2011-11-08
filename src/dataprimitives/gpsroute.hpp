#ifndef GPSROUTE_HPP
#define GPSROUTE_HPP

#include "gpsposition.hpp"
#include <QList>

/**
 * @brief Diese Klasse stellt eine Route im Speicher dar.
 * 
 * 
 * 
 * @ingroup dataprimitives
 * @todo Implementierung fehlt genauso wie eine komplette Definition der
 *  nötigen Eigenschaften. Diese Klasse ist nicht komplett! Sie wurde definiert,
 *  um die Routing-Schnittstellen definieren zu können.
 * @todo Unterscheidung in Wegpunkte und Trackpunkte mit einbauen
 *      (Trackpunkte sind Punkte, die angefahren werden aber nicht unbedingt
 *      wichtig sind, wie z.B. wenn man durch eine S-Kurve fährt: Wichtig sind nur
 *      die Endpunkte, man kann die Straße ja nicht verlassen, aber man kann Trackpunkte
 *      in der Mitte setzen damit es schöner aussieht)
 * @todo Unterstützung für die Erzeugung einer Wegbeschreibung mit einbauen
 *      (muss noch nicht implementiert werden, es reichen Dummies)
 * @todo Export inx GPS-Format (XML) einbauen
 * @todo Export ins JSON-Format (wie bei Cloudmade) einbauen
 * @author Lena Brueder
 * @date 2011-11-01
 * @copyright GNU GPL v3
 */

class GPSRoute
{
public:
    /**
     * @brief Diese Funktion gibt die Länge der Route in Metern an.
     * 
     * Die Fu8nktion rechnet nicht ein, dass die Route der Erdoberfläche
     * folgt und sich entsprechend der zurückgelegten Höhenmeter eine
     * Verlängerung der STrecke ergibt.
     * 
     * @return Die Länge der Route in Metern.
     * @todo Implementierung fehlt.
     */
    double calcLength() const;

    /**
     * @brief Gibt den Zielpunkt der Route zurück.
     * 
     * @return Den Zielpunkt der Route.
     * @todo Implementierung fehlt.
     */
    GPSPosition getDestination() const;

    /**
     * @brief Gibt den Startpunkt der Route zurück.
     * 
     * @return Den Startpunkt der Route.
     * @todo Implementierung fehlt.
     */
    GPSPosition getStartingPoint() const;


    /**
     * @brief Gibt an, ob die Route leer ist oder nicht.
     * 
     * @return Ob die Route leer ist, oder nicht.
     * @todo Implementierung fehlt.
     */
    bool isEmpty() const;
    
    /**
     * @brief Leert die Route.
     * @todo Implementierung fehlt.
     */
    void clear();

    /**
     * @brief Kehrt die Route um.
     * @todo Implementierung fehlt.
     */
    void reverse();
    
    /**
     * @brief Erstellt eine leere Route.
     * @todo Implementierung fehlt.
     */
    GPSRoute();
    /**
     * @brief Kopiert eine Route.
     * @todo Implementierung fehlt.
     */
    GPSRoute(const GPSRoute& r);
private:
    
};

#endif // GPSROUTE_HPP
