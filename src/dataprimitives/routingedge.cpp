#include "routingedge.hpp"

/**
 * @file
 * @todo Testen!
 */

#define LOWEST_BIT_STREETTYPE           0
#define LOWEST_BIT_CYCLEWAYTYPE         4
#define LOWEST_BIT_STREETSURFACETYPE    8
#define LOWEST_BIT_STREETSURFACEQUALITY 12
#define LOWEST_BIT_TURNTYPE             16

#define LOWEST_BIT_EXTRAOPTIONS         32
#define LOWEST_BIT_TRAFFICLIGHTS        (LOWEST_BIT_EXTRAOPTIONS + 0)
#define LOWEST_BIT_TRAFFICCALMINGBUMPS  (LOWEST_BIT_EXTRAOPTIONS + 1)
#define LOWEST_BIT_STOPSIGN             (LOWEST_BIT_EXTRAOPTIONS + 2)
#define LOWEST_BIT_CYCLEBARRIER         (LOWEST_BIT_EXTRAOPTIONS + 3)
#define LOWEST_BIT_STAIRS               (LOWEST_BIT_EXTRAOPTIONS + 4)

/**
 * @brief Gibt den Wert eines Bits an einer Stelle in einem Integer zurück.
 * @todo Testen!
 */
#define GET_BIT_FROM_INT(integer,bit)                   ((integer & (((boost::uint64_t)1) << bit)) > 0)
/**
 * @brief Setzt den Wert eines Bits an einer Stelle in einem Integer.
 * @todo Testen!
 */
#define SET_BIT_IN_INT(integer,bit,value)               if (value) integer |= (((boost::uint64_t)1)<<bit); else integer &= ~(((boost::uint64_t)1)<<bit);
/**
 * @brief Gibt den Wert eines Nibbles (4 Bit) an einer Stelle in einem Integer zurück.
 * @todo Testen!
 */
#define GET_NIBBLE_STARTING_AT_BIT(integer,bit)         (integer & (((boost::uint64_t)0xF) << bit)) >> bit
/**
 * @brief Setzt den Wert eines Nibbles (4 Bit) an einer Stelle in einem Integer.
 * @todo Testen!
 */
#define SET_NIBBLE_STARTING_AT_BIT(integer,bit,value)   integer &= ~(((boost::uint64_t)0xF) << bit); integer |= ((value & ((boost::uint64_t)0xF)) << bit) >> bit;

bool RoutingEdge::hasTrafficLights() const
{
    return GET_BIT_FROM_INT(properties, LOWEST_BIT_TRAFFICLIGHTS);
}


bool RoutingEdge::hasTrafficCalmingBumps() const
{
    return GET_BIT_FROM_INT(properties, LOWEST_BIT_TRAFFICCALMINGBUMPS);
}


bool RoutingEdge::hasStopSign() const
{
    return GET_BIT_FROM_INT(properties, LOWEST_BIT_STOPSIGN);
}


bool RoutingEdge::hasStairs() const
{
    return GET_BIT_FROM_INT(properties, LOWEST_BIT_STAIRS);
}


bool RoutingEdge::hasCycleBarrier() const
{
    return GET_BIT_FROM_INT(properties, LOWEST_BIT_CYCLEBARRIER);
}


void RoutingEdge::setTrafficLights(bool value)
{
    SET_BIT_IN_INT(properties, LOWEST_BIT_TRAFFICLIGHTS, value);
}


void RoutingEdge::setTrafficCalmingBumps(bool value)
{
    SET_BIT_IN_INT(properties, LOWEST_BIT_TRAFFICCALMINGBUMPS, value);
}


void RoutingEdge::setStopSign(bool value)
{
    SET_BIT_IN_INT(properties, LOWEST_BIT_STOPSIGN, value);
}


void RoutingEdge::setStairs(bool value)
{
    SET_BIT_IN_INT(properties, LOWEST_BIT_STAIRS, value);
}


void RoutingEdge::setCycleBarrier(bool value)
{
    SET_BIT_IN_INT(properties, LOWEST_BIT_CYCLEBARRIER, value);
}


boost::uint8_t RoutingEdge::getStreetType() const
{
    return GET_NIBBLE_STARTING_AT_BIT(properties, LOWEST_BIT_STREETTYPE);
}


boost::uint8_t RoutingEdge::getCyclewayType() const
{
    return GET_NIBBLE_STARTING_AT_BIT(properties, LOWEST_BIT_CYCLEWAYTYPE);
}


boost::uint8_t RoutingEdge::getStreetSurfaceType() const
{
    return GET_NIBBLE_STARTING_AT_BIT(properties, LOWEST_BIT_STREETSURFACETYPE);
}


boost::uint8_t RoutingEdge::getStreetSurfaceQuality() const
{
    return GET_NIBBLE_STARTING_AT_BIT(properties, LOWEST_BIT_STREETSURFACEQUALITY);
}


boost::uint8_t RoutingEdge::getTurnType() const
{
    return GET_NIBBLE_STARTING_AT_BIT(properties, LOWEST_BIT_TURNTYPE);
}


void RoutingEdge::setStreetType(boost::uint8_t streetType)
{
    SET_NIBBLE_STARTING_AT_BIT(properties, LOWEST_BIT_STREETTYPE, streetType);
}


void RoutingEdge::setCyclewayType(boost::uint8_t cyclewayType)
{
    SET_NIBBLE_STARTING_AT_BIT(properties, LOWEST_BIT_CYCLEWAYTYPE, cyclewayType);
}


void RoutingEdge::setStreetSurfaceType(boost::uint8_t streetSurfaceType)
{
    SET_NIBBLE_STARTING_AT_BIT(properties, LOWEST_BIT_STREETSURFACETYPE, streetSurfaceType);
}


void RoutingEdge::setStreetSurfaceQuality(boost::uint8_t streetSurfaceQuality)
{
    SET_NIBBLE_STARTING_AT_BIT(properties, LOWEST_BIT_STREETSURFACEQUALITY, streetSurfaceQuality);
}


void RoutingEdge::setTurnType(boost::uint8_t turnType)
{
    SET_NIBBLE_STARTING_AT_BIT(properties, LOWEST_BIT_TURNTYPE, turnType);
}
