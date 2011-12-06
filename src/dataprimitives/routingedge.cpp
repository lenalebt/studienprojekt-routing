#include "routingedge.hpp"

bool RoutingEdge::hasTrafficLights() const
{
    return _properties.trafficLights;
}


bool RoutingEdge::hasTrafficCalmingBumps() const
{
    return _properties.trafficCalmingBumps;
}


bool RoutingEdge::hasStopSign() const
{
    return _properties.stopSign;
}


bool RoutingEdge::hasStairs() const
{
    return _properties.stairs;
}


bool RoutingEdge::hasCycleBarrier() const
{
    return _properties.cycleBarrier;
}


void RoutingEdge::setTrafficLights(const bool value)
{
    _properties.trafficLights = value;
}


void RoutingEdge::setTrafficCalmingBumps(const bool value)
{
    _properties.trafficCalmingBumps = value;
}


void RoutingEdge::setStopSign(const bool value)
{
    _properties.stopSign = value;
}


void RoutingEdge::setStairs(const bool value)
{
    _properties.stairs = value;
}


void RoutingEdge::setCycleBarrier(const bool value)
{
    _properties.cycleBarrier = value;
}


boost::uint8_t RoutingEdge::getStreetType() const
{
    return _properties.streetType;
}


boost::uint8_t RoutingEdge::getCyclewayType() const
{
    return _properties.cyclewayType;
}


boost::uint8_t RoutingEdge::getStreetSurfaceType() const
{
    return _properties.streetSurfaceType;
}


boost::uint8_t RoutingEdge::getStreetSurfaceQuality() const
{
    return _properties.streetSurfaceQuality;
}


boost::uint8_t RoutingEdge::getTurnType() const
{
    return _properties.turnType;
}


void RoutingEdge::setStreetType(const boost::uint8_t streetType)
{
    _properties.streetType = streetType;
}


void RoutingEdge::setCyclewayType(const boost::uint8_t cyclewayType)
{
    _properties.cyclewayType = cyclewayType;
}


void RoutingEdge::setStreetSurfaceType(const boost::uint8_t streetSurfaceType)
{
    _properties.streetSurfaceType = streetSurfaceType;
}


void RoutingEdge::setStreetSurfaceQuality(const boost::uint8_t streetSurfaceQuality)
{
    _properties.streetSurfaceQuality = streetSurfaceQuality;
}


void RoutingEdge::setTurnType(const boost::uint8_t turnType)
{
    _properties.turnType = turnType;
}

boost::uint64_t RoutingEdge::getProperties() const
{
    return (_properties.cycleBarrier             << BITPOS_CYCLEBARRIER) |
            (_properties.cyclewayType            << BITPOS_CYCLEWAYTYPE) |
            (_properties.stairs                  << BITPOS_STAIRS) |
            (_properties.stopSign                << BITPOS_STOPSIGN) |
            (_properties.streetSurfaceQuality    << BITPOS_STREETSURFACEQUALITY) |
            (_properties.streetSurfaceType       << BITPOS_STREETSURFACETYPE) |
            (_properties.streetType              << BITPOS_STREETTYPE) |
            (_properties.trafficCalmingBumps     << BITPOS_TRAFFICCALMINGBUMPS) |
            (_properties.trafficLights           << BITPOS_TRAFFICLIGHTS) |
            (_properties.turnType                << BITPOS_TURNTYPE);
            
}

/**
 * @todo Implementierung, Plattformunabhängig, ist so evtl falsch.
 */
void RoutingEdge::setProperties(const boost::uint64_t properties)
{
    _properties.cycleBarrier         = (properties >> BITPOS_CYCLEBARRIER)         & ((1ull << BITLENGTH_CYCLEBARRIER) - 1ull);
    _properties.cyclewayType         = (properties >> BITPOS_CYCLEWAYTYPE)         ;//& ((1ull << BITLENGTH_CYCLEBARRIER) - 1ull);
    _properties.stairs               = (properties >> BITPOS_STAIRS)               & ((1ull << BITLENGTH_CYCLEBARRIER) - 1ull);
    _properties.stopSign             = (properties >> BITPOS_STOPSIGN)             & ((1ull << BITLENGTH_CYCLEBARRIER) - 1ull);
    _properties.streetSurfaceQuality = (properties >> BITPOS_STREETSURFACEQUALITY) ;//& ((1ull << BITLENGTH_CYCLEBARRIER) - 1ull);
    _properties.streetSurfaceType    = (properties >> BITPOS_STREETSURFACETYPE)    ;//& ((1ull << BITLENGTH_CYCLEBARRIER) - 1ull);
    _properties.streetType           = (properties >> BITPOS_STREETTYPE)           ;//& ((1ull << BITLENGTH_CYCLEBARRIER) - 1ull);
    _properties.trafficCalmingBumps  = (properties >> BITPOS_TRAFFICCALMINGBUMPS)  & ((1ull << BITLENGTH_CYCLEBARRIER) - 1ull);
    _properties.trafficLights        = (properties >> BITPOS_TRAFFICLIGHTS)        & ((1ull << BITLENGTH_CYCLEBARRIER) - 1ull);
    _properties.turnType             = (properties >> BITPOS_TURNTYPE)             ;//& ((1ull << BITLENGTH_CYCLEBARRIER) - 1ull);
}


std::ostream& operator<<(std::ostream& os, const RoutingEdge& edge)
{
    os << "RoutingEdge, id " << edge._id << ", startNodeID " << edge._startNodeID <<
        ", endNodeID " << edge._endNodeID << std::endl;
    os << "streetType " << edge._properties.streetType << std::endl <<
        "cyclewayType " << edge._properties.cyclewayType << std::endl <<
        "streetSurfaceType " << edge._properties.streetSurfaceType << std::endl <<
        "streetSurfaceQuality " << edge._properties.streetSurfaceQuality << std::endl <<
        "turnType " << edge._properties.turnType << std::endl <<
        "trafficLights " << edge._properties.trafficLights << std::endl <<
        "trafficCalmingBumps " << edge._properties.trafficCalmingBumps << std::endl <<
        "stopSign " << edge._properties.stopSign << std::endl <<
        "cycleBarrier " << edge._properties.cycleBarrier << std::endl <<
        "stairs " << edge._properties.stairs << std::endl <<
        "properties " << edge.getProperties() << std::endl;
    
    return os;
}

RoutingEdge::RoutingEdge() :
    _id(0), _startNodeID(0), _endNodeID(0)
{
    _properties.cycleBarrier                = false;
    _properties.cyclewayType                = 0;
    _properties.stairs                      = false;
    _properties.stopSign                    = false;
    _properties.streetSurfaceQuality        = 0;
    _properties.streetSurfaceType           = 0;
    _properties.streetType                  = 0;
    _properties.trafficCalmingBumps         = false;
    _properties.trafficLights               = false;
    _properties.turnType                    = 0;
}

RoutingEdge::RoutingEdge(boost::uint64_t id) :
    _id(id), _startNodeID(0), _endNodeID(0)
{
    _properties.cycleBarrier                = false;
    _properties.cyclewayType                = 0;
    _properties.stairs                      = false;
    _properties.stopSign                    = false;
    _properties.streetSurfaceQuality        = 0;
    _properties.streetSurfaceType           = 0;
    _properties.streetType                  = 0;
    _properties.trafficCalmingBumps         = false;
    _properties.trafficLights               = false;
    _properties.turnType                    = 0;
}

RoutingEdge::RoutingEdge(boost::uint64_t id, boost::uint64_t startNodeID, boost::uint64_t endNodeID) :
    _id(id), _startNodeID(startNodeID), _endNodeID(endNodeID)
{
    _properties.cycleBarrier                = false;
    _properties.cyclewayType                = 0;
    _properties.stairs                      = false;
    _properties.stopSign                    = false;
    _properties.streetSurfaceQuality        = 0;
    _properties.streetSurfaceType           = 0;
    _properties.streetType                  = 0;
    _properties.trafficCalmingBumps         = false;
    _properties.trafficLights               = false;
    _properties.turnType                    = 0;
}
RoutingEdge::RoutingEdge(boost::uint64_t id, boost::uint64_t startNodeID, boost::uint64_t endNodeID, boost::uint64_t properties) :
    _id(id), _startNodeID(startNodeID), _endNodeID(endNodeID)
{
    setProperties(properties);
}

bool RoutingEdge::operator==(const RoutingEdge& other)
{
    return (this->_id == other._id) &&
        (this->_startNodeID == other._startNodeID) &&
        (this->_endNodeID == other._endNodeID) &&
        (this->_properties == other._properties);
}

bool operator==(const RoutingEdge::PropertyType& a, const RoutingEdge::PropertyType& b)
{
    return (a.streetType == b.streetType) &&
        (a.cyclewayType == b.cyclewayType) &&
        (a.streetSurfaceType == b.streetSurfaceType) &&
        (a.streetSurfaceQuality == b.streetSurfaceQuality) &&
        (a.turnType == b.turnType) &&
        (a.trafficLights == b.trafficLights) &&
        (a.trafficCalmingBumps == b.trafficCalmingBumps) &&
        (a.stopSign == b.stopSign) &&
        (a.cycleBarrier == b.cycleBarrier) &&
        (a.stairs == b.stairs);
}

namespace biker_tests
{
    int testRoutingEdge()
    {
        RoutingEdge edge1(1), edge2(1);
        
        CHECK_EQ(edge1.getID(), 1u);
        
        edge1.setStairs(true);
        CHECK(edge1.hasStairs());
        
        edge1.setCycleBarrier(true);
        CHECK(edge1.hasCycleBarrier());
        
        edge1.setStopSign(false);
        CHECK(!edge1.hasStopSign());
        
        edge1.setStreetSurfaceQuality(5);
        CHECK_EQ_TYPE(edge1.getStreetSurfaceQuality(), 5, boost::uint8_t);
        
        edge1.setCyclewayType(6);
        CHECK_EQ_TYPE(edge1.getCyclewayType(), 6, boost::uint8_t);
        
        edge1.setStreetType(15);
        CHECK_EQ_TYPE(edge1.getStreetType(), 15, boost::uint8_t);
        
        edge2.setProperties(edge1.getProperties());
        CHECK_EQ(edge1, edge2);
        
        CHECK_EQ(edge1.getProperties(), edge2.getProperties());
        
        return EXIT_SUCCESS;
    }
}
