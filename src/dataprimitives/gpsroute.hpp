#ifndef GPSROUTE_HPP
#define GPSROUTE_HPP

#include "gpsposition.hpp"
// QList wird verwendet, weil an verschiedenen Stellen in der Liste Objekte eingefügt werden können (und nicht nur einseitig).
#include <QList>
#include "tests.hpp"
#include "altitudeprovider.hpp"
#include <boost/shared_ptr.hpp>

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
    double calcLength() const
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
    void insertAtPosition(int pos, const GPSPosition position)
    {
        route.insert(pos, position);
    } 
    /**
     * @brief führt einen Wegpunkt vorwärts in die Route ein.
     * @param position Element, dass an den Anfang der Liste gesetzt werden soll.
     */
    void insertForward(const GPSPosition position)
    {
        route.insert(0, position);
    }    
    /**
     * @brief führt einen Wegpunkt rückwärts in die Route ein.
     * @param position Element, dass an das Ende der Lise angefügt werden soll.
     */
    void insertBackward(const GPSPosition position)
    {
        route.append(position);
    }
    /**
     * @brief gibt die Anzahl der Elemente zurück.
     * @return die Anzahl an Elementen.
     */
    int getSize() const
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
     * @param provider Der Höhenprovider, der für die Ausgabe der Höhenwerte verwendet werden soll
     */
    void exportGPX(const QString filename, boost::shared_ptr<AltitudeProvider> provider = boost::shared_ptr<AltitudeProvider>());//, GPSRoute& route);
    /**
     * @brief Diese Funktion exportiert die Route in einen QString mit  GPX-Format, sodass sie
     * von anderen Applikationen benutzt werden kann.
     * @param provider Der Höhenprovider, der für die Ausgabe der Höhenwerte verwendet werden soll
     */
    QString exportGPXString(boost::shared_ptr<AltitudeProvider> provider = boost::shared_ptr<AltitudeProvider>());//(GPSRoute& route);

    /**
     * @brief Diese Funktion exportiert die Route in das JSON-Format, sodass sie
     * von anderen Applikationen benutzt werden kann.
     * @param filename Name unter der die JSON-Datei gespeichert werden soll
     */
    void exportJSON(const QString filename);//, GPSRoute& route);
    /**
     * @brief Diese Funktion exportiert die Route in einen QString mit  JSON-Format, sodass sie
     * von anderen Applikationen benutzt werden kann.
     */
    QString exportJSONString();//GPSRoute& route);

    /**
     * @brief Kopiert eine Route.
     * @param r die Route, dei Kopiert werden soll.
     */
    GPSRoute(const GPSRoute& r) :
        duration(r.duration)
    {
        for (int index = 0; index < r.getSize(); index++)
        {
            route.insert(index, r[index]);
        }
    }
    /**
    * @brief Erstellt eine leere Route.
    *
    */
    GPSRoute() :
        route(), duration(0.0)
    {
    }
    /**
    * @brief Erstellt eine Route mit einem Element.
    * @param firstPosition das erste Element in der Liste.
    */
    GPSRoute(GPSPosition firstPosition) :
        route(), duration(0.0)
    {
        route << firstPosition;
    }

    /**
     * @brief gibt die GPSPosition an der Indexstelle i zurück.
     * @param i Indexstelle, an der sich das gesuchte Objekt befinden soll.
     * @return die GPSPosition an der Stelle i.
     */
    GPSPosition operator[](int i) const
    {
        return route.at(i);
    }
    /**
     * @brief gibt an, ob zwei Listen gleich sind.
     * @param r die Liste, die mit der aktuellen verglichen wird.
     * @return ein Boolischer Wert, der angibt, ob zwei Listen gleich sind, oder nicht.
     */
    bool operator == (const GPSRoute& r) const
    {
        return route == r.get();
    }
    
    //GPSRoute route; route << GPSPosition(1, 2);
    /**
      * @brief ein Operator zum einfügen eines GPSPositon Punkts
      * @return diese GPSRoute
      */
    GPSRoute& operator<<(const GPSPosition& point)
    {
        route << point;
        return *this;
    }
    //route << route2;
    /**
     * @brief ein Operator zum einfügen einer anderen Route in die aktuelle
     * @return diese GPSRoute
     */
    GPSRoute& operator<<(const GPSRoute& otherRoute)
    {
        this->setDuration(this->getDuration() + otherRoute.getDuration());
        route << otherRoute.get();
        return *this;
    }
    
    /**
     * @brief Gibt, sofern gesetzt, zurück wie lang man braucht um diese
     *      Route zu befahren.
     * 
     * Die Dauer muss vorher über setDuration() gesetzt werden, da sie nur
     * in Zusammenhang mit einer Routingmetrik berechnet werden kann.
     * 
     * @return Die Dauer, um eine Route zu befahren.
     */
    double getDuration() const {return duration;}
    
    /**
     * @brief Setzt die Dauer, die man benötigt, um die Route zu befahren.
     */
    void setDuration(double duration) {this->duration = duration;}
private:
    /**
     * @brief gibt die Liste zurück.
     * @return die Liste.
     */
    QList<GPSPosition> get() const
    {
       return route;
    }
    QList<GPSPosition> route;
    
    //Dauer in Sekunden
    double duration;
};


namespace biker_tests
{
    /**
     * @ingroup tests
     */
    int testGPSRoute();
}

#endif // GPSROUTE_HPP
