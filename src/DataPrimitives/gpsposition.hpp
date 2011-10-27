//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.
//      


#ifndef GPSPOSITION_HPP
#define GPSPOSITION_HPP

#include "DataPrimitiveDefines.hpp"
#include "math.h"

class GPSPosition
{
public:
    virtual double getLon() const;
    virtual double getLat() const;
    virtual double getRadLon() const;
    virtual double getRadLat() const;
    virtual void setLon(double lon);
    virtual void setLat(double lat);
    virtual void setRadLon(double lon);
    virtual void setRadLat(double lat);

    //Die folgenden Funktionen dienen zur Berechnung von Zusammenhängen mehrerer Koordinaten (Entfernung, ...)
    /**
     * Berechnet die Entfernung zweier Punkte in Metern.
     * Die Berechnung erfolgt nicht nach dem WGS84-Ellipsoid, kann daher
     * für größere Entfernungen ungenau werden.
     */
    virtual double calcDistance(GPSPosition p2) const;
    /**
     * Berechnet den Kurswinkel, gesehen vom Punkt auf p2 zu, in Grad.
     */
    virtual double calcCourseAngle(GPSPosition p2) const;

    /**
     * Wegpunktprojektion.
     * Berechnet einen Punkt in einer Entfernung von distance Metern unter einem Kurswinkel
     * von courseAngle.
     * @param courseAngle
     * @param distance
     * @return
     */
    virtual GPSPosition calcPositionInDistance(double courseAngle, double distance) const;
    virtual bool isInitialized() const;

    GPSPosition() : lon(0.0), lat(0.0) {}
    GPSPosition(double lon, double lat) : lon(lon), lat(lat) {}
    virtual ~GPSPosition() {}
protected:
    double lon;
    double lat;

private:
    double calcXi(GPSPosition p2) const;
    /**
     * Rechnet um von Grad nach Bogenmaß. Template-Funktion,
     * gedacht für float und double.
     */
    template<typename T>
    T deg2rad(T deg) const
    {
        return deg/180.0*M_PI;
    }

    /**
     * Rechnet um von Bogenmaß nach Grad. Template-Funktion,
     * gedacht für float und double.
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
