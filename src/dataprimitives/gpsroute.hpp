#ifndef GPSROUTE_HPP
#define GPSROUTE_HPP

#include "gpsposition.hpp"
// QList wird verwendet, weil an verschiedenen Stellen in der Liste Objekte eingefügt werden können (und nicht nur einseitig).
#include <QList>
#include "tests.hpp"

/**
 * @brief Diese Klasse stellt eine Route im Speicher dar.
 * @ingroup dataprimitives
 * @todo Unterstützung für die Erzeugung einer Wegbeschreibung mit einbauen
 *      (muss noch nicht implementiert werden, es reichen Dummies)
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
    double calcLength()
    {
        double length = 0.0;
        for (int index = 1; index < route.size(); index++)
            {
                length = length + route.at(index).calcDistance(route.at(index - 1));
            }
        return length;
    }
    /**
     * @brief Gibt den Zielpunkt der Route zurück.
     * @return Den Zielpunkt der Route.
     */
    GPSPosition getDestination() const
    {
        return route.last();
    }   
    /**
     * @brief Gibt den Startpunkt der Route zurück.
     * @return Den Startpunkt der Route.
     */
    GPSPosition getStartingPoint() const
    {
        return route.first();
    }
    /**
     * @brief Gibt an, ob die Route leer ist oder nicht.
     * @return Ob die Route leer ist, oder nicht.
     */
    bool isEmpty() const
    {
        return route.empty();
    }   
    /**
     * @brief Leert die Route.
     * 
     */
    void clear()
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
    void insertAtPosition(int pos, GPSPosition position)
    {
        route.insert(pos, position);
    } 
    /**
     * @brief führt einen Wegpunkt vorwärts in die Route ein.
     * @param position Element, dass an den Anfang der Liste gesetzt werden soll.
     */
    void insertForward(GPSPosition position)
    {
        route.insert(0, position);
    }    
    /**
     * @brief führt einen Wegpunkt rückwärts in die Route ein.
     * @param position Element, dass an das Ende der Lise angefügt werden soll.
     */
    void insertBackward(GPSPosition position)
    {
        route.append(position);
    }
    /**
     * @brief gibt die Anzahl der Elemente zurück.
     * @return die Anzahl an Elementen.
     */
    int getSize()
    {
       return route.size(); 
    }
    /**
     * @brief Kehrt die Route um.
     */
    void reverse()
    {
        int size = route.size();
        QList<GPSPosition>  newRoute;
        for (int index = 0; index < size; index++)
        {
            newRoute.insert(index, route.at(size - 1 - index));
        }
        clear();
        for (int index = 0; index < size; index++)
        {
            route.insert(index, newRoute.at(index));
        }
    }
    /**
     * @brief Diese Funktion exportiert die Route in das GPX-Format, sodass sie
     * von anderen Applikationen benutzt werden kann.
     * @param filename Name unter der die GPX-Datei gespeichert werden soll
     */
    static void exportGPX(QString filename, GPSRoute& route);
    /**
     * @brief Diese Funktion exportiert die Route in das JSON-Format, sodass sie
     * von anderen Applikationen benutzt werden kann.
     * @param filename Name unter der die JSON-Datei gespeichert werden soll
     * @param route die Route, die ausgegeben werden soll
     */
    static void exportJSON(QString filename, GPSRoute& route);
    /**
     * @brief Diese Funktion exportiert die Route in einen QString mit  JSON-Format, sodass sie
     * von anderen Applikationen benutzt werden kann.
     * @param route die Route, die ausgegeben werden soll
     */
    QString exportJSONString(GPSRoute& route);

    /**
     * @brief Kopiert eine Route.
     * @param r die Route, dei Kopiert werden soll.
     */
    GPSRoute(GPSRoute& r)
    {
        for (int index = 0; index < route.size(); index++)
        {
            route.insert(index, r[index]);
        }
    }
    /**
    * @brief Erstellt eine leere Route.
    *
    */
    GPSRoute()
    {
        route = QList<GPSPosition>();
    }
    /**
    * @brief Erstellt eine Route mit einem Element.
    * @param firstPosition das erste Element in der Liste.
    */
    GPSRoute(GPSPosition firstPosition)
    {
        route = QList<GPSPosition>();
        route << firstPosition;
    }

    /**
     * @brief gibt die GPSPosition an der Indexstelle i zurück.
     * @param i Indexstelle, an der sich das gesuchte Objekt befinden soll.
     * @return die GPSPosition an der Stelle i.
     */
    GPSPosition operator[](int i)
    {
        return route.at(i);
    }
    /**
     * @brief gibt an, ob zwei Listen gleich sind.
     * @param r die Liste, die mit der aktuellen verglichen wird.
     * @return ein Boolischer Wert, der angibt, ob zwei Listen gleich sind, oder nicht.
     */
    bool operator == (GPSRoute& r)
    {
        return route == r.get();
    }
private:
    /**
     * @brief gibt die Liste zurück.
     * @return die Liste.
     */
    QList<GPSPosition> get()
    {
       return route;
    }
    QList<GPSPosition> route;
};


namespace biker_tests
{
    /**
     * @ingroup tests
     */
    int testGPSRoute();
}

#endif // GPSROUTE_HPP
