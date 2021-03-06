#ifndef GPSPOSITION_HPP
#define GPSPOSITION_HPP

#include "math.h"
#include "tests.hpp"
#include <iostream>

#define EARTH_RADIUS 6371000
#ifndef M_PI
    #define M_PI 3.14159265358979
#endif

typedef double gps_float;

//Muss oben stehen, da man den Test sonst nicht als friend deklarieren kann
namespace biker_tests
{
    /**
     * @ingroup tests
     */
    int testGPSPosition();
}

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
    gps_float getLon() const {return this->lon;}
    /**
     * @brief Gibt den Breitengrad des Punktes zurück, in Grad.
     * @return den Breitengrad in Grad
     */
    gps_float getLat() const {return this->lat;}
    /**
     * @brief Gibt den Längengrad des Punktes zurück, in Bogenmaß.
     * @return den Längengrad in Bogenmaß
     */
    gps_float getRadLon() const {return deg2rad<gps_float>(this->lon);}
    /**
     * @brief Gibt den Breitengrad des Punktes zurück, in Bogenmaß.
     * @return den Breitengrad in Bogenmaß
     */
    gps_float getRadLat() const {return deg2rad<gps_float>(this->lat);}
    /**
     * @brief Setzt den Längengrad des Punktes in Grad.
     * @remarks Wenn der Winkel nicht in [-180°, 180°] liegt, wird er
     *      umgewandelt, dass er in dem Intervall liegt.
     * @param lon Längengrad in Grad
     */
    void setLon(const gps_float lon)
    {
        this->lon = lon;
        
        while (this->lon >= 360.0)
            this->lon -= 360.0;
        while (this->lon < -360.0)
            this->lon += 360.0;
        if (this->lon >= 180.0)
            this->lon -= 360.0;
        if (this->lon < -180.0)
            this->lon += 360.0;
    }
    /**
     * @brief Setzt den Breitengrad des Punktes in Grad.
     * @remarks Wenn der Winkel nicht in [-90°, 90°] liegt, wird er
     *      einfach abgeschnitten.
     * @param lat Breitengrad in Grad
     */
    void setLat(const gps_float lat)
    {
        if (lat > 90.0)
            this->lat = 90.0;
        else if (lat < -90.0)
            this->lat = -90.0;
        else
            this->lat = lat;
    }
    /**
     * @brief Setzt den Längengrad des Punktes in Bogenmaß.
     * @remarks Wenn der Winkel nicht in [-180°, 180°] liegt, wird er
     *      umgewandelt, dass er in dem Intervall liegt.
     * @param lon Längengrad in Bogenmaß
     */
    void setRadLon(const gps_float lon)
    {
        this->lon = rad2deg<gps_float>(lon);
        
        while (this->lon >= 360.0)
            this->lon -= 360.0;
    }
    /**
     * @brief Setzt den Breitengrad des Punktes in Bogenmaß.
     * @remarks Wenn der Winkel nicht in [-180°, 180°] liegt, wird er
     *      einfach abgeschnitten.
     * @param lat Breitengrad in Bogenmaß
     */
    void setRadLat(const gps_float lat)
    {
        this->lat = rad2deg<gps_float>(lat);
        
        while (this->lat >= 360.0)
            this->lat -= 360.0;
    }

    //Die folgenden Funktionen dienen zur Berechnung von Zusammenhängen mehrerer Koordinaten (Entfernung, ...)
    /**
     * @brief Berechnet die Entfernung zweier Punkte in Metern.
     * 
     * Die Berechnung erfolgt nicht nach dem WGS84-Ellipsoid, kann daher
     * für größere Entfernungen ungenau werden.
     * @param p2 ein anderer Punkt
     * @return die Entfernung zwischen diesem und dem anderen Punkt.
     */
    double calcDistance(const GPSPosition& p2) const;
    /**
     * @brief Berechnet den Kurswinkel (gegenüber Nord), gesehen vom Punkt auf p2 zu, in Grad.
     * @param p2 ein anderer Punkt
     * @return den Kurswinkel, gesehen vom Punkt auf p2 zu, in Grad
     */
    double calcCourseAngle(const GPSPosition& p2) const;

    /**
     * @brief Wegpunktprojektion.
     * Berechnet einen Punkt in einer Entfernung von <code>distance</code> Metern unter einem Kurswinkel
     * von <code>courseAngle</code>.
     * @param courseAngle der Kurswinkel zu Nord
     * @param distance Die Entfernung vom Startpunkt
     * @return
     */
    GPSPosition calcPositionInDistance(const double courseAngle, const double distance) const;
    
    /**
     * @brief Gibt zurück, ob die Position noch auf (0.0/0.0) gesetzt ist.
     * @return <code>false</code>, wenn <code>lon==lat==0.0</code>, <code>true</code> sonst.
     */
    bool isInitialized() const;
    
    /**
     * @brief Initialisiert eine GPSPosition mit den Werten <code>lon=0.0, lat=0.0</code>
     */
    GPSPosition() : lon(0.0), lat(0.0) {}
    /**
     * @brief Initialisiert eine GPSPosition mit den angegebenen Werten.
     * @param lat der Breitengrad
     * @param lon der Längengrad
     */
    GPSPosition(gps_float lat, gps_float lon);
    virtual ~GPSPosition() {}
    
    friend int biker_tests::testGPSPosition();
protected:
    /**
     * @brief Längengrad
     */
    gps_float lon;
    
    /**
     * @brief Breitengrad
     */
    gps_float lat;

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
std::ostream& operator<<(std::ostream& os, const GPSPosition& p);
std::istream& operator>>(std::istream& is, GPSPosition& p);

#endif // GPSPOSITION_HPP
