#ifndef GPSPOSITION_HPP
#define GPSPOSITION_HPP

#include "math.h"
#include "tests.hpp"

#define EARTH_RADIUS 6371000

/**
 * @brief GPSPosition speichert Längen- und Breitengrad und ist entsprechend eine Repräsentation eines
 * Punktes auf der Erdoberfläche.
 * 
 * Höhendaten werden nicht gespeichert. Es ist möglich, auf die Eigenschaften statt in Grad, auch
 * in Bogenmaß zuzugreifen - lediglich der Konstruktor ist nur für die (gebräuchlichere) Angabe in
 * Grad verfügbar.
 * 
 * @ingroup dataprimitives
 * @author Lena Brüder
 * @todo Winkel werden noch nicht begrenzt auf ihre gültigen Wertebereiche!
 * 
 * @copyright Lizenz: GNU GPL v3
 */

class GPSPosition
{
public:
    /**
     * @brief Gibt den Längengrad des Punktes zurück, in Grad.
     * @return den Längengrad in Grad
     */
    virtual double getLon() const;
    /**
     * @brief Gibt den Breitengrad des Punktes zurück, in Grad.
     * @return den Breitengrad in Grad
     */
    virtual double getLat() const;
    /**
     * @brief Gibt den Längengrad des Punktes zurück, in Bogenmaß.
     * @return den Längengrad in Bogenmaß
     */
    virtual double getRadLon() const;
    /**
     * @brief Gibt den Breitengrad des Punktes zurück, in Bogenmaß.
     * @return den Breitengrad in Bogenmaß
     */
    virtual double getRadLat() const;
    /**
     * @brief Setzt den Längengrad des Punktes in Grad.
     * @remarks Wenn der Winkel nicht in [-180°, 180°] liegt, wird er
     *      umgewandelt, dass er in dem Intervall liegt.
     * @param lon Längengrad in Grad
     */
    virtual void setLon(const double lon);
    /**
     * @brief Setzt den Breitengrad des Punktes in Grad.
     * @remarks Wenn der Winkel nicht in [-90°, 90°] liegt, wird er
     *      einfach abgeschnitten.
     * @param lat Breitengrad in Grad
     */
    virtual void setLat(const double lat);
    /**
     * @brief Setzt den Längengrad des Punktes in Bogenmaß.
     * @remarks Wenn der Winkel nicht in [-180°, 180°] liegt, wird er
     *      umgewandelt, dass er in dem Intervall liegt.
     * @param lon Längengrad in Bogenmaß
     */
    virtual void setRadLon(const double lon);
    /**
     * @brief Setzt den Breitengrad des Punktes in Bogenmaß.
     * @remarks Wenn der Winkel nicht in [-180°, 180°] liegt, wird er
     *      einfach abgeschnitten.
     * @param lat Breitengrad in Bogenmaß
     */
    virtual void setRadLat(const double lat);

    //Die folgenden Funktionen dienen zur Berechnung von Zusammenhängen mehrerer Koordinaten (Entfernung, ...)
    /**
     * @brief Berechnet die Entfernung zweier Punkte in Metern.
     * 
     * Die Berechnung erfolgt nicht nach dem WGS84-Ellipsoid, kann daher
     * für größere Entfernungen ungenau werden.
     * @param p2 ein anderer Punkt
     * @return die Entfernung zwischen diesem und dem anderen Punkt.
     */
    virtual double calcDistance(const GPSPosition& p2) const;
    /**
     * @brief Berechnet den Kurswinkel (gegenüber Nord), gesehen vom Punkt auf p2 zu, in Grad.
     * @param p2 ein anderer Punkt
     * @return den Kurswinkel, gesehen vom Punkt auf p2 zu, in Grad
     */
    virtual double calcCourseAngle(const GPSPosition& p2) const;

    /**
     * @brief Wegpunktprojektion.
     * Berechnet einen Punkt in einer Entfernung von <code>distance</code> Metern unter einem Kurswinkel
     * von <code>courseAngle</code>.
     * @param courseAngle der Kurswinkel zu Nord
     * @param distance Die Entfernung vom Startpunkt
     * @return
     */
    virtual GPSPosition calcPositionInDistance(const double courseAngle, const double distance) const;
    
    /**
     * @brief Gibt zurück, ob die Position noch auf (0.0/0.0) gesetzt ist.
     * @return <code>true</code>, wenn <code>lon==lat==0.0</code>, <code>false</code> sonst.
     */
    virtual bool isInitialized() const;
    
    /**
     * @brief Initialisiert eine GPSPosition mit den Werten <code>lon=0.0, lat=0.0</code>
     */
    GPSPosition() : lon(0.0), lat(0.0) {}
    /**
     * @brief Initialisiert eine GPSPosition mit den angegebenen Werten.
     * @param lat der Breitengrad
     * @param lon der Längengrad
     */
    GPSPosition(double lat, double lon);
    virtual ~GPSPosition() {}
protected:
    /**
     * @brief Längengrad
     */
    double lon;
    
    /**
     * @brief Breitengrad
     */
    double lat;

private:
    
    double calcXi(const GPSPosition& p2) const;
    /**
     * @brief Rechnet um von Grad nach Bogenmaß.
     * 
     * Template-Funktion,
     * gedacht für float und double.
     * @param deg Winkel in Grad
     * @return Winkel in Bogenmaß
     */
    template<typename T>
    T deg2rad(T deg) const
    {
        return deg/180.0*M_PI;
    }

    /**
     * @brief Rechnet um von Bogenmaß nach Grad.
     * 
     * Template-Funktion,
     * gedacht für float und double.
     * @param rad Winkel in Bogenmaß
     * @return Winkel in Grad
     */
    template<typename T>
    T rad2deg(T rad) const
    {
        return rad*180.0/M_PI;
    }
};

bool operator==(const GPSPosition& p1, const GPSPosition& p2);
bool operator!=(const GPSPosition& p1, const GPSPosition& p2);

namespace biker_tests
{
    int testGPSPosition();
}

#endif // GPSPOSITION_HPP
