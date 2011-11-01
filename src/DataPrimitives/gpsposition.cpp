/**
 * @copyright Lizenz: GNU GPL v3
 */

#include "gpsposition.hpp"

double GPSPosition::getLon() const
{
    return this->lon;
}
double GPSPosition::getLat() const
{
    return this->lat;
}

void GPSPosition::setLon(const double lon)
{
    this->lon = lon;
}

void GPSPosition::setLat(const double lat)
{
    this->lat = lat;
}

double GPSPosition::getRadLon() const
{
    return deg2rad<double>(this->lon);
}
double GPSPosition::getRadLat() const
{
    return deg2rad<double>(this->lat);
}

void GPSPosition::setRadLon(const double lon)
{
    this->lon = rad2deg<double>(lon);
}

void GPSPosition::setRadLat(const double lat)
{
    this->lat = rad2deg<double>(lat);
}

double GPSPosition::calcCourseAngle(const GPSPosition& p2) const
{
    if (!p2.isInitialized())
        return 0.0;
    //Berechnung mittels sphärischer Trigonometrie.
    //Fallunterscheidung nötig, da sonst durch Null geteilt würde (xi==0 => sin(xi)==0)
    if (*this == p2)
    {
        return 0.0;
    }
    else
    {
        double xi=calcXi(p2);
        if (this->getLon() <= p2.getLon())
            return rad2deg<double>(acos( (sin(p2.getRadLat()) - sin(this->getRadLat())*cos(xi))/(cos(this->getRadLat()) * sin(xi)) ));
        else
            return 360.0 - rad2deg<double>(acos( (sin(p2.getRadLat()) - sin(this->getRadLat())*cos(xi))/(cos(this->getRadLat()) * sin(xi)) ));
    }
}

double GPSPosition::calcDistance(const GPSPosition& p2) const
{
    if (!p2.isInitialized())
        return 0.0;
    /* Berechnung bezieht nicht ein, dass die Erde ein Rotationsellipsoid ist, daher
     * können größere Entfernungen ggf. ungenau werden. Die Berechnung nach WGS84 ist
     * allerdings ungleich rechenintensiver, daher soll das hier so bleiben.
     */
    //Fallunterscheidung ist nötig, da sonst Ergebnis NaN.
    if (*this == p2)
        return 0.0;
    else
        return calcXi(p2)*EARTH_RADIUS;
}

/**
 * Berechnet xi, das bei der Entfernungs- und Winkelberechnung gebraucht wird.
 */
double GPSPosition::calcXi(const GPSPosition& p2) const
{
    return acos(sin(this->getRadLat()) * sin(p2.getRadLat()) + cos(this->getRadLat()) * cos(p2.getRadLat()) * cos(p2.getRadLon() - this->getRadLon()));
}

bool operator==(const GPSPosition& p1, const GPSPosition& p2)
{
    return ((p1.getLat() == p2.getLat()) && (p1.getLon() == p2.getLon()));
}

bool operator!=(const GPSPosition& p1, const GPSPosition& p2)
{
    return !(p1 == p2);
}


bool GPSPosition::isInitialized() const
{
    return !((lon == 0.0) && (lat == 0.0));
}

GPSPosition GPSPosition::calcPositionInDistance(const double courseAngle, const double distance) const
{
    if (!this->isInitialized())
        return GPSPosition();

    GPSPosition target;

    //Ich züchte hier einen Klammernwald. An Weihnachten kann man sich dann schöne Klammerbäume hinstellen.
    double xi = distance / EARTH_RADIUS;
    target.setLat(    rad2deg(asin(  sin(this->getRadLat())*cos(xi) + cos(this->getRadLat())*sin(xi)*cos(deg2rad(courseAngle))  ))    );
    target.setLon(    this->lon + rad2deg(asin(   sin(xi)/cos(this->getRadLat())*sin(deg2rad(courseAngle))   ))    );

    return target;
}

