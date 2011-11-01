#ifndef ALTITUDEPROVIDER_HPP
#define ALTITUDEPROVIDER_HPP

#include "gpsposition.hpp"


/**
 * @brief Ein AltitudeProvider kann für jeden Punkt auf der Erde einen
 *      Höhenwert zurückgeben.
 * 
 * AltitudeProvider ist ein Interface und kann selbst nicht instanziiert
 * werden.
 * 
 * @see SRTMProvider
 * @author Lena Brüder
 * @date 2011-11-01
 * @copyright GNU GPL v3
 */
class AltitudeProvider
{
private:
    
public:
    /**
     * @brief Gibt einen Höhenwert für einen Punkt auf der Erdoberfläche zurück.
     * 
     * Dabei wird der Punkt über seine beiden Koordinaten angegeben.
     * 
     * @param lon Der Längengrad.
     * @param lat Der Breitengrad.
     * @return Den Höhenwert an dieser Stelle
     */
    virtual double getAltitude(double lon, double lat)=0;
    /**
     * @brief Gibt einen Höhenwert für einen Punkt auf der Erdoberfläche zurück.
     * 
     * Dabei wird der Punkt über seine GPSPosition angegeben.
     * 
     * @param pos Die Position, an der der Höhenwert ausgegeben werden soll
     * @return Den Höhenwert an dieser Stelle
     */
    virtual double getAltitude(const GPSPosition& pos)=0;
};

/**
 * @brief SRTM-Implementierung vom AltitudeProvider.
 * 
 * 
 * 
 * @author Lena Brüder
 * @date 2011-11-01
 * @copyright GNU GPL v3
 * @todo Doxygen-Kommentare und Implementierung
 */
class SRTMProvider : public AltitudeProvider
{
private:
    
public:
    /**
     * @brief Gibt einen Höhenwert für einen Punkt auf der Erdoberfläche zurück.
     * 
     * Dabei wird der Punkt über seine beiden Koordinaten angegeben.
     * 
     * @param lon Der Längengrad.
     * @param lat Der Breitengrad.
     * @return Den Höhenwert an dieser Stelle
     */
    double getAltitude(double lon, double lat);
    /**
     * @brief Gibt einen Höhenwert für einen Punkt auf der Erdoberfläche zurück.
     * 
     * Dabei wird der Punkt über seine GPSPosition angegeben.
     * 
     * @param pos Die Position, an der der Höhenwert ausgegeben werden soll
     * @return Den Höhenwert an dieser Stelle
     */
    double getAltitude(const GPSPosition& pos);
};

#endif //ALTITUDEPROVIDER_HPP
