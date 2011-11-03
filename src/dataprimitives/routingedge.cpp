#include "routingedge.hpp"

#define LOWEST_BIT_STREETTYPE 0
#define LOWEST_BIT_CYCLEWAYTYPE 4
#define LOWEST_BIT_STREETSURFACETYPE 8
#define LOWEST_BIT_STREETSURFACEQUALITY 12
#define LOWEST_BIT_TURNTYPE 16

#define LOWEST_BIT_EXTRAOPTIONS 32
#define LOWEST_BIT_TRAFFICLIGHTS LOWEST_BIT_EXTRAOPTIONS + 0
#define LOWEST_BIT_TRAFFICCALMINGBUMPS LOWEST_BIT_EXTRAOPTIONS + 1
#define LOWEST_BIT_STOPSIGN LOWEST_BIT_EXTRAOPTIONS + 2
#define LOWEST_BIT_CYCLEBARRIER LOWEST_BIT_EXTRAOPTIONS + 3
#define LOWEST_BIT_STAIRS LOWEST_BIT_EXTRAOPTIONS + 4

bool RoutingEdge::hasTrafficLights()
{
    return (properties & (1<<LOWEST_BIT_TRAFFICLIGHTS));
}


bool RoutingEdge::hasTrafficCalmingBumps()
{
    return (properties & (1<<LOWEST_BIT_TRAFFICCALMINGBUMPS));
}


bool RoutingEdge::hasStopSign()
{
    return (properties & (1<<LOWEST_BIT_STOPSIGN));
}


bool RoutingEdge::hasStairs()
{
    return (properties & (1<<LOWEST_BIT_STAIRS));
}


bool RoutingEdge::hasCycleBarrier()
{
    return (properties & (1<<LOWEST_BIT_CYCLEBARRIER));
}


void RoutingEdge::setTrafficLights(bool value)
{
    if (value)  //Wenn Wert gesetzt werden soll: Bit setzen über logisches ODER
        properties |= (1<<LOWEST_BIT_TRAFFICLIGHTS);
    else    //Wenn nicht: Bit löschen über logisches UND mit Bitmaske, die an einer Stelle Null ist
        properties &= ~(1<<LOWEST_BIT_TRAFFICLIGHTS);
}


void RoutingEdge::setTrafficCalmingBumps(bool value)
{
    if (value)  //Wenn Wert gesetzt werden soll: Bit setzen über logisches ODER
        properties |= (1<<LOWEST_BIT_TRAFFICCALMINGBUMPS);
    else    //Wenn nicht: Bit löschen über logisches UND mit Bitmaske, die an einer Stelle Null ist
        properties &= ~(1<<LOWEST_BIT_TRAFFICCALMINGBUMPS);
}


void RoutingEdge::setStopSign(bool value)
{
    if (value)  //Wenn Wert gesetzt werden soll: Bit setzen über logisches ODER
        properties |= (1<<LOWEST_BIT_STOPSIGN);
    else    //Wenn nicht: Bit löschen über logisches UND mit Bitmaske, die an einer Stelle Null ist
        properties &= ~(1<<LOWEST_BIT_STOPSIGN);
}


void RoutingEdge::setStairs(bool value)
{
    if (value)  //Wenn Wert gesetzt werden soll: Bit setzen über logisches ODER
        properties |= (1<<LOWEST_BIT_STAIRS);
    else    //Wenn nicht: Bit löschen über logisches UND mit Bitmaske, die an einer Stelle Null ist
        properties &= ~(1<<LOWEST_BIT_STAIRS);
}


void RoutingEdge::setCycleBarrier(bool value)
{
    if (value)  //Wenn Wert gesetzt werden soll: Bit setzen über logisches ODER
        properties |= (1<<LOWEST_BIT_CYCLEBARRIER);
    else    //Wenn nicht: Bit löschen über logisches UND mit Bitmaske, die an einer Stelle Null ist
        properties &= ~(1<<LOWEST_BIT_CYCLEBARRIER);
}


boost::uint8_t RoutingEdge::getStreetType()
{
}


boost::uint8_t RoutingEdge::getCyclewayType()
{
}


boost::uint8_t RoutingEdge::getStreetSurfaceType()
{
}


boost::uint8_t RoutingEdge::getStreetSurfaceQuality()
{
}


boost::uint8_t RoutingEdge::getTurnType()
{
}


void RoutingEdge::setStreetType(boost::uint8_t streetType)
{
}


void RoutingEdge::setCyclewayType(boost::uint8_t cyclewayType)
{
}


void RoutingEdge::setStreetSurfaceType(boost::uint8_t streetSurfaceType)
{
}


void RoutingEdge::setStreetSurfaceQuality(boost::uint8_t streetSurfaceQuality)
{
}


void RoutingEdge::setTurnType(boost::uint8_t turnType)
{
}
