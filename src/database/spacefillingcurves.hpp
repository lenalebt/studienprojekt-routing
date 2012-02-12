#ifndef SPACEFILLINGCURVES_HPP
#define SPACEFILLINGCURVES_HPP

#include <QVector>
#include <boost/cstdint.hpp>

/**
 * Diese Klasse implementiert ein Zahlenintervall. Es ist sichergestellt, dass
 * zu jedem Zeitpunkt p1<=p2 gilt.
 */
template <typename T>
class Interval
{
public:
	Interval(T p1_, T p2_) : p1(p1_), p2(p2_) {preserveOrder();}
	Interval() : p1(T()), p2(T()) {}
	T getP1() {return p1;}
	T getP2() {return p2;}
	void setP1(T p1_) {p1 = p1_; preserveOrder();}
	void setP2(T p2_) {p2 = p2_; preserveOrder();}
private:
	T p1, p2;
	inline void preserveOrder()
	{
		if (p1>p2)
		{
			T tmp = p1;
			p1=p2;
			p2=tmp;
		}
	}
};

/**
 * Dies ist das Interface für eine raumfüllende Kurve. So kann jederzeit die benutzte
 * raumfüllende Kurve für die Datenbank ausgetauscht werden, ohne viele andere Code-
 * zeilen anfassen zu müssen.
 */
class SpaceFillingCurve
{
private:
    double minLon, minLat, maxLon, maxLat;
    uint16_t meshSize;
public:
    SpaceFillingCurve(double minLon, double minLat, double maxLon, double maxLat) :
            minLon(minLon), minLat(minLat), maxLon(maxLon), maxLat(maxLat), meshSize(16) {}
    SpaceFillingCurve() :
            minLon(0.0), minLat(0.0), maxLon(180.0), maxLat(360.0), meshSize(16) {}
    
	/**
	 * Diese Funktion gibt zu einer bestimmten Koordinate und der Angabe,
	 * wie viele Iterationen mit der Raumfüllenden Kurve durchgeführt werden
	 * sollen, die entsprechende BucketID zurück. Das Ergebnis soll in einen
	 * 32bit-Integer passen, also dürfen die Eingabewerte höchstens 16bit-Integer
	 * sein.
	 */
	virtual boost::uint32_t getBucketID(boost::uint16_t x, boost::uint16_t y, int iterationCount)=0;
	
    
    virtual boost::uint32_t getBucketID(double lon, double lat)
    {
        double meshx = (maxLon-minLon);
        double meshy = (maxLat-minLat);
        
        boost::uint64_t x,y;
        /* Rundungsfehler werden hässlich, wenn man das nicht unterscheidet.
         * Vergleich ist okay, weil die Werte im Fall gleichgesetzt wurden vorher,
         * und nicht Ergebnis einer Rechnung sind.
         */
        if (lon != maxLon)
        {
            if (lon != minLon)
                x = (boost::uint64_t)((lon-minLon) / meshx * (2<<meshSize));
            else
                x = 0;
        }
        else
            x = (2<<meshSize);
        
        if (lat != maxLat)	//s.o.
        {
            if (lon != minLon)
                y = (boost::uint64_t)((lat-minLat) / meshy * (2<<meshSize));
            else
                y = 0;
        }
        else
            y = (2<<meshSize);
        
        return this->getBucketID(x, y, 16);
    }

	/**
	 * Diese Funktion berechnet eine Liste von Indicies, die abgefragt werden müssen
	 * um alle Punkte in einem bestimmten Intervall zu erreichen - aber keinen mehr
	 * als nötig. Damit soll vermieden werden, unnötig viele Punkte aus der Datenbank
	 * zu laden und betrachten zu müssen.
	 */
	virtual QVector<Interval<boost::uint32_t> > getRectangleIndexList(boost::uint16_t upperLeftX,
			boost::uint16_t upperLeftY, boost::uint16_t bottomRightX, boost::uint16_t bottomRightY,
			int iterationCount)=0;
};

/**
 * Implementiert die Z-Kurve. Die Implementierung hat, bis auf Schleifen, 2 Zeilen :D.
 */
class ZOrderCurve : public SpaceFillingCurve
{
public:
	boost::uint32_t getBucketID(boost::uint16_t x, boost::uint16_t y, int iterationCount)
    {
        /*
         * Die folgende Zeile macht:
         * **  Zuerst werden x und y um die rechts stehenden Bits beschnitten, je nachdem wie viele
         *     "Iterationen" gemacht werden sollen (d.h., wieviele Buckets entstehen sollen).
         * **  Dann werden die Bits "gespreizt", d.h. dass z.B. aus 1001 1_0_0_1_ wird, wobei an
         *     den "_"-Positionen Nullen stehen müssen.
         * **  Die y-Koordinaten werden dann um eins nach links geschoben und mit den x-Koordinaten
         *     vermengt. Fertig sind die Z-Order-Buckets.
         */
        return (spreadInteger(x >> (16-iterationCount)) << 1) | (spreadInteger(y >> (16-iterationCount)));
    }
	QVector<Interval<boost::uint32_t> > getRectangleIndexList(boost::uint16_t upperLeftX,
			boost::uint16_t upperLeftY, boost::uint16_t bottomRightX, boost::uint16_t bottomRightY,
			int iterationCount);
private:
	boost::uint32_t spreadInteger(boost::uint32_t number)
    {
        uint32_t retVal = 0;
        for (int i=0; i<16; i++)
        {
            //Diese Zeile schaut, was sich an Position i von number befindet, und schreibt das an Position 2*i von retVal.
            retVal |= (((number << (15-i)) & (1 << 15)) == (1 << 15)) ? (1 << (i*2)) : 0;
        }
        return retVal;
    }
};


#endif // SPACEFILLINGCURVES_HPP
