#ifndef GPSPOSITION_HPP
#define GPSPOSITION_HPP

#include "math.h"

#define EARTH_RADIUS 6371000

/**
 * @brief GPSPosition speichert Längen- und Breitengrad und ist entsprechend eine Repräsentation eines
 * Punktes auf der Erdoberfläche.
 * 
 * Höhendaten werden nicht gespeichert. Es ist möglich, auf die Eigenschaften statt in Grad, auch
 * in Bogenmaß zuzugreifen - lediglich der Konstruktor ist nur für die (gebräuchlichere) Angabe in
 * Grad verfügbar.
 * 
 * @author Lena Brüder
 * 
 * @copyright Lizenz: GNU GPL v3
 */

class GPSPosition
{
public:
    /**
     * Gibt den Längengrad des Punktes zurück, in Grad.
     * @return den Längengrad in Grad
     */
    virtual double getLon() const;
    /**
     * Gibt den Breitengrad des Punktes zurück, in Grad.
     * @return den Breitengrad in Grad
     */
    virtual double getLat() const;
    /**
     * Gibt den Längengrad des Punktes zurück, in Bogenmaß.
     * @return den Längengrad in Bogenmaß
     */
    virtual double getRadLon() const;
    /**
     * Gibt den Breitengrad des Punktes zurück, in Bogenmaß.
     * @return den Breitengrad in Bogenmaß
     */
    virtual double getRadLat() const;
    /**
     * Setzt den Längengrad des Punktes in Grad.
     * @param lon Längengrad in Grad
     */
    virtual void setLon(double lon);
    /**
     * Setzt den Breitengrad des Punktes in Grad.
     * @param lat Breitengrad in Grad
     */
    virtual void setLat(double lat);
    /**
     * Setzt den Längengrad des Punktes in Bogenmaß.
     * @param lon Längengrad in Bogenmaß
     */
    virtual void setRadLon(double lon);
    /**
     * Setzt den Breitengrad des Punktes in Bogenmaß.
     * @param lat Breitengrad in Bogenmaß
     */
    virtual void setRadLat(double lat);

    //Die folgenden Funktionen dienen zur Berechnung von Zusammenhängen mehrerer Koordinaten (Entfernung, ...)
    /**
     * Berechnet die Entfernung zweier Punkte in Metern.
     * Die Berechnung erfolgt nicht nach dem WGS84-Ellipsoid, kann daher
     * für größere Entfernungen ungenau werden.
     * @param p2 ein anderer Punkt
     * @return die Entfernung zwischen diesem und dem anderen Punkt.
     */
    virtual double calcDistance(GPSPosition p2) const;
    /**
     * Berechnet den Kurswinkel (gegenüber Nord), gesehen vom Punkt auf p2 zu, in Grad.
     * @param p2 ein anderer Punkt
     * @return den Kurswinkel, gesehen vom Punkt auf p2 zu, in Grad
     */
    virtual double calcCourseAngle(GPSPosition p2) const;

    /**
     * Wegpunktprojektion.
     * Berechnet einen Punkt in einer Entfernung von <code>distance</code> Metern unter einem Kurswinkel
     * von <code>courseAngle</code>.
     * @param courseAngle der Kurswinkel zu Nord
     * @param distance Die Entfernung vom Startpunkt
     * @return
     */
    virtual GPSPosition calcPositionInDistance(double courseAngle, double distance) const;
    
    /**
     * Gibt zurück, ob die Position noch auf (0.0/0.0) gesetzt ist.
     * @return <code>true</code>, wenn <code>lon==lat==0.0</code>, <code>false</code> sonst.
     */
    virtual bool isInitialized() const;
    
    /**
     * Initialisiert eine GPSPosition mit den Werten <code>lon=0.0, lat=0.0</code>
     */
    GPSPosition() : lon(0.0), lat(0.0) {}
    /**
     * Initialisiert eine GPSPosition mit den angegebenen Werten.
     * @param lon der Längengrad
     * @param lat der Breitengrad
     */
    GPSPosition(double lon, double lat) : lon(lon), lat(lat) {}
    virtual ~GPSPosition() {}
protected:
    /**
     * Längengrad
     */
    double lon;
    
    /**
     * Breitengrad
     */
    double lat;

private:
    
    double calcXi(GPSPosition p2) const;
    /**
     * Rechnet um von Grad nach Bogenmaß. Template-Funktion,
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
     * Rechnet um von Bogenmaß nach Grad. Template-Funktion,
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


#endif // GPSPOSITION_HPP
