#include "spacefillingcurves.hpp"

QVector<Interval<boost::uint32_t> > ZOrderCurve::getRectangleIndexList(boost::uint16_t upperLeftX,
			boost::uint16_t upperLeftY, boost::uint16_t bottomRightX, boost::uint16_t bottomRightY,
			int iterationCount)
{
	QVector<Interval<boost::uint32_t> > list;
	//TODO: Das ist erstmal nur, damit ich bei der Datenbank weiterbauen kann.
	list << Interval<boost::uint32_t>(getBucketID(upperLeftX, upperLeftY, iterationCount), getBucketID(bottomRightX, bottomRightY, iterationCount));
	return list;
}

