/**
 * @copyright Lizenz: GNU GPL v3
 */

#include "gpsposition.hpp"
#include <cmath>
#include <sstream>

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
    {
        double retVal = calcXi(p2)*EARTH_RADIUS;
        //auf NaN testen, und im Ernstfall einfach 0 zurückgeben.
        return (retVal == retVal) ? retVal : 0.0;
    }
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

std::ostream& operator<<(std::ostream& os, const GPSPosition& p)
{
    os << "(" << p.getLat() << "/" << p.getLon() << ")";
    return os;
}

std::istream& operator>>(std::istream& is, GPSPosition& p)
{
    double lon=0.0;
    double lat=0.0;
    char c;
    is >> c >> lat >> c >> lon >> c;
    p.setLat(lat);
    p.setLon(lon);
    return is;
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

GPSPosition::GPSPosition(gps_float lat, gps_float lon)
{
    this->setLat(lat);
    this->setLon(lon);
}


namespace biker_tests
{
    int testGPSPosition()
    {
        GPSPosition pos;
        CHECK(!pos.isInitialized());
        
        pos.setLat(1.0);
        CHECK_EQ(pos.getLat(), 1.0);
        
        CHECK(pos.isInitialized());
        
        pos.setLon(3.0);
        CHECK_EQ(pos.getLon(), 3.0);
        
        pos.setLon(181.0);
        CHECK_EQ(pos.getLon(), -179.0);
        
        pos.setLat(91.0);
        CHECK_EQ(pos.getLat(), 90.0);
        
        pos.setLat(-91.0);
        CHECK_EQ(pos.getLat(), -90.0);
        
        pos.setLat(1.0);
        CHECK_EQ(pos.getRadLat(), 0.017453292519943);
        
        pos.setLat(-1.0);
        CHECK_EQ(pos.getRadLat(), -0.017453292519943);
        
        pos.setLon(1.0+360.0);
        CHECK_EQ(pos.getRadLon(), 0.017453292519943);
        
        pos = GPSPosition(251.0, 251.0);
        CHECK_EQ(pos.getLat(), 90.0);
        CHECK_EQ(pos.getLon(), -109.0);
        
        
        pos = GPSPosition(51.0, 7.0);
        CHECK_EQ(pos.getLat(), 51.0);
        CHECK_EQ(pos.getLon(), 7.0);
        
        GPSPosition pos2 = pos.calcPositionInDistance(0.0, 5000.0);
        CHECK_EQ(pos2.getLat(), 51.04496608030);
        CHECK_EQ(pos2.getLon(), 7.0);
        
        CHECK_EQ(pos.calcXi(pos2), 0.000784806152905);
        
        CHECK_EQ_TYPE(pos.calcCourseAngle(pos2), 0.000459404801803, float);
        
        GPSPosition tmppos1, tmppos2;
        std::stringstream strstream (std::stringstream::in | std::stringstream::out);
        strstream << "(51/7)" << "(52.6/8.9)";
        strstream >> tmppos1;
        strstream >> tmppos2;
        CHECK_EQ(tmppos1, GPSPosition(51, 7));
        CHECK_EQ(tmppos2, GPSPosition(52.6, 8.9));
        
        return EXIT_SUCCESS;
    }
}
