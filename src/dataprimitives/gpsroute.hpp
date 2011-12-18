#ifndef GPSROUTE_HPP
#define GPSROUTE_HPP

#include "gpsposition.hpp"
#include <QVector>
#include "tests.hpp"

/**
 * @brief Diese Klasse stellt eine Route im Speicher dar.
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
 * @author Thorsten Scheller
 * @date 2011-12-18
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
     */
    double calcLength() const;
    {
        double length = 0.0;
        for (int index = 0; index < route.getSize(); index++)
            {
                length = length + route.at(index).calcDistance(route.at(index + 1));
            }
        return length;
    }
    /**
     * @brief Gibt den Zielpunkt der Route zurück.
     * @return Den Zielpunkt der Route.
     */
    GPSPosition getDestination() const;
    {
        return route.last();
    }   
    /**
     * @brief Gibt den Startpunkt der Route zurück.
     * @return Den Startpunkt der Route.
     */
    GPSPosition getStartingPoint() const;
    {
        return route.first();
    }
    /**
     * @brief Gibt an, ob die Route leer ist oder nicht.
     * @return Ob die Route leer ist, oder nicht.
     */
    bool isEmpty() const;
    {
        return route.empty();
    }   
    /**
     * @brief Leert die Route.
     * 
     */
    void clear();
    {
        int index = route.size();
        while (index != 0)
        {
            route.removeLast();
            index--;
        }
    }
    /**
     * @brief führt einen Wegpunkt in die Route an die Stelle pos ein.
     * @param position Element, dass an den Anfang der Liste gesetzt werden soll.
     * @param pos Stelle in der Liste, an die das Element eingefügt werden soll.
     */
    void insertAtPosition(int pos, GPSPosition position);
    {
        route.insert(pos, position);
    } 
    /**
     * @brief führt einen Wegpunkt vorwärts in die Route ein.
     * @param position Element, dass an den Anfang der Liste gesetzt werden soll.
     */
    void insertForward(GPSPosition position);
    {
        route.insert(0, position);
    }    
    /**
     * @brief führt einen Wegpunkt rückwärts in die Route ein.
     * @param position Element, dass an das Ende der Lise angefügt werden soll.
     */
    void insertBackward(GPSPosition position);
    {
        route.append(position);
    }
    /**
     * @brief gibt die Anzahl der Elemente zurück.
     * @return die Anzahl an Elementen.
     */
    int getSize();
    {
       return route.size(); 
    }
    /**
     * @brief Kehrt die Route um.
     * @todo Implementierung fehlt.
     */
    void reverse();
    {
        int size = route.size();
        for (int index; index <= int(size/2); index++)
        {
            route.swap(index, (size - index - 1));
        }
    }
    /**
     * @brief Diese Funktion exportiert die Route in das GPX-Format, sodass sie
     * von anderen Applikationen benutzt werden kann.
     * @param filename Name unter der die GPX-Datei gespeichert werden soll
     * @param r Route, die gespeichert werden soll
     * @todo funktionsfähig umschreiben (bisher wurde nur die alte Version kopiert)
     */
    static void exportGPX(QString filename, GPSRoute r);
    {
    QDomDocument doc;
    //zuerst das Grundelement erstellen, dann runter bis die Wegpunkte eingefügt werden.
    QDomElement root = doc.createElement("gpx");
    root.setAttribute("version", "1.0");
    root.setAttribute("xmlns", "http://www.topografix.com/GPX/1/0");			//TODO: Werte dazu!
    root.setAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    root.setAttribute("xsi:schemaLocation", "http://www.topografix.com/GPX/1/0 http://www.topografix.com/GPX/1/0/gpx.xsd");
    root.setAttribute("creator", "Biker https://github.com/lenalebt/Biker");
    doc.appendChild(root);
    
    QDomNode node( doc.createProcessingInstruction( "xml", "version=\"1.0\" standalone=\"no\"" ) );
    doc.insertBefore(node, doc.firstChild());

    QDomElement wptNode = doc.createElement("wpt");
    root.appendChild(trkNode);

    QDomElement trkSegNode = doc.createElement("trkseg");
    trkNode.appendChild(trkSegNode);

    QDomElement trkPoint;
    QLocale locale(QLocale::C); //sonst schreibt er in eine GPX-Datei Kommas statt Punkte
    //Wegpunkt zur Liste hinzutun
    for (int i=0; i<r.size(); i++)
    {
        trkPoint = doc.createElement("trkpt");
        trkPoint.setAttribute("lon", locale.toString(r.getWaypoint(i).getLon()));
        trkPoint.setAttribute("lat", locale.toString(r.getWaypoint(i).getLat()));
        trkSegNode.appendChild(trkPoint);
    }

    //Datei öffnen und so
    QFile file(gpxFilename);
    if (!file.open(QIODevice::WriteOnly))	//Versuche, die Datei zu öffnen
    {
        std::cerr << "Opening file for writing failed." << std::endl;
        throw std::ios_base::failure("Opening file for writing failed.");
        return;
    }
    //jetzt noch Daten wegschreiben
    QTextStream stream(&file);
    stream << doc.toString();
    file.close();

    }
    /**
     * @brief Diese Funktion exportiert die Route in das JSON-Format, sodass sie
     * von anderen Applikationen benutzt werden kann.
     * @param filename Name unter der die JSON-Datei gespeichert werden soll
     * @param r Route, die gespeichert werden soll
     * @todo implementieren
     */
    static void exportJSON(QString filename, GPSRoute r);
    /**
     * @brief Erstellt eine leere Route.
     * 
     */
    GPSRoute();
    {
        route = new QList<GPSPosition>;
    }
    /**
     * @brief Erstellt eine Route mit einem Element.
     * @param firstPosition das erste Element in der Liste.
     */
    GPSRoute(GPSPosition firstPosition);
    {
        route = new QList<GPSPosition>;
        route << firstPosition;
    }
    /**
     * @brief Kopiert eine Route.
     * @param r die Route, dei Kopiert werden soll.
     * @todo eventuell at() nochmals implementieren
     */
    GPSRoute(const GPSRoute& r);
    {
        for (int index = 0; index < route.getSize(); index++)
        {
            route.insert(index, r.at(index));
        }
    }
private:
    QList<GPSPosition> route;
};
/**
 * @todo alle Funktionen testen
 */
namespace biker_tests
{
    int testGPSRoute();
}

#endif // GPSROUTE_HPP
