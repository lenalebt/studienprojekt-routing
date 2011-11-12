#include "routingedge.hpp"

bool RoutingEdge::hasTrafficLights() const
{
    return properties.trafficLights;
}


bool RoutingEdge::hasTrafficCalmingBumps() const
{
    return properties.trafficCalmingBumps;
}


bool RoutingEdge::hasStopSign() const
{
    return properties.stopSign;
}


bool RoutingEdge::hasStairs() const
{
    return properties.stairs;
}


bool RoutingEdge::hasCycleBarrier() const
{
    return properties.cycleBarrier;
}


void RoutingEdge::setTrafficLights(bool value)
{
    properties.trafficLights = value;
}


void RoutingEdge::setTrafficCalmingBumps(bool value)
{
    properties.trafficCalmingBumps = value;
}


void RoutingEdge::setStopSign(bool value)
{
    properties.stopSign = value;
}


void RoutingEdge::setStairs(bool value)
{
    properties.stairs = value;
}


void RoutingEdge::setCycleBarrier(bool value)
{
    properties.cycleBarrier = value;
}


boost::uint8_t RoutingEdge::getStreetType() const
{
    return properties.streetType;
}


boost::uint8_t RoutingEdge::getCyclewayType() const
{
    return properties.cyclewayType;
}


boost::uint8_t RoutingEdge::getStreetSurfaceType() const
{
    return properties.streetSurfaceType;
}


boost::uint8_t RoutingEdge::getStreetSurfaceQuality() const
{
    return properties.streetSurfaceQuality;
}


boost::uint8_t RoutingEdge::getTurnType() const
{
    return properties.turnType;
}


void RoutingEdge::setStreetType(boost::uint8_t streetType)
{
    properties.streetType = streetType;
}


void RoutingEdge::setCyclewayType(boost::uint8_t cyclewayType)
{
    properties.cyclewayType = cyclewayType;
}


void RoutingEdge::setStreetSurfaceType(boost::uint8_t streetSurfaceType)
{
    properties.streetSurfaceType = streetSurfaceType;
}


void RoutingEdge::setStreetSurfaceQuality(boost::uint8_t streetSurfaceQuality)
{
    properties.streetSurfaceQuality = streetSurfaceQuality;
}


void RoutingEdge::setTurnType(boost::uint8_t turnType)
{
    properties.turnType = turnType;
}
