#ifndef ROUTINGEDGE_HPP
#define ROUTINGEDGE_HPP

#include <boost/cstdint.hpp>
#include <iostream>
#include "tests.hpp"

#define STREETTYPE_HIGHWAY_PRIMARY      0
#define STREETTYPE_HIGHWAY_SECONDARY    1
#define STREETTYPE_HIGHWAY_TERTIARY     2
#define STREETTYPE_HIGHWAY_TRACK	    3
#define STREETTYPE_HIGHWAY_PATH 	    4
#define STREETTYPE_HIGHWAY_CYCLEWAY		5
#define STREETTYPE_HIGHWAY_FOOTWAY		6
#define STREETTYPE_HIGHWAY_LIVINGSTREET 7
#define STREETTYPE_HIGHWAY_RESIDENTIAL  8
#define STREETTYPE_HIGHWAY_UNKNOWN      9
#define STREETTYPE_HIGHWAY_PEDESTRIAN   10
#define STREETTYPE_HIGHWAY_SERVICE      11


#define STREETSURFACEQUALITY_EXCELLENT    	0
#define STREETSURFACEQUALITY_GOOD  			1
#define STREETSURFACEQUALITY_INTERMEDIATE	2
#define STREETSURFACEQUALITY_BAD			3
#define STREETSURFACEQUALITY_VERYBAD		4
#define STREETSURFACEQUALITY_HORRIBLE		5
#define STREETSURFACEQUALITY_VERYHORRIBLE	6
#define STREETSURFACEQUALITY_IMPASSIBLE		7


//CONCRETE wird zu PAVED oder was aehnliches
//MUD wird UNPAVED
//DIRT wird UNPAVED
#define STREETSURFACETYPE_PAVED			0
#define STREETSURFACETYPE_UNPAVED		1
#define STREETSURFACETYPE_ASPHALT		2
#define STREETSURFACETYPE_SETT			3
#define STREETSURFACETYPE_COBBLESTONE	4
#define STREETSURFACETYPE_COMPACTED		5
#define STREETSURFACETYPE_GRASSPAVER	6
#define STREETSURFACETYPE_GRAVEL		7
#define STREETSURFACETYPE_FINEGRAVEL	8
#define STREETSURFACETYPE_PEBBLESTONE	9
#define STREETSURFACETYPE_SAND			10
#define STREETSURFACETYPE_EARTH			11
#define STREETSURFACETYPE_GRASS			12
#define STREETSURFACETYPE_WOOD			13
#define STREETSURFACETYPE_METAL			14

//Umlaufgitter etc: nur wirkliche Hindernisse zaehlen als Hindernis.
//Ein Poller, an dem man vorbeifahren kann z.B. nicht.

#define TURNTYPE_LEFTCROSS		0
#define TURNTYPE_RIGHTCROSS		1
#define TURNTYPE_STRAIGHT		2
#define TURNTYPE_STRAIGHTCROSS	3
#define TURNTYPE_RIGHT			4


#define BITLENGTH_STREETTYPE            4ull
#define BITLENGTH_CYCLEWAYTYPE          4ull
#define BITLENGTH_STREETSURFACETYPE     4ull
#define BITLENGTH_STREETSURFACEQUALITY  4ull
#define BITLENGTH_TURNTYPE              4ull
#define BITLENGTH_TRAFFICLIGHTS         1ull
#define BITLENGTH_TRAFFICCALMINGBUMPS   1ull
#define BITLENGTH_STOPSIGN              1ull
#define BITLENGTH_CYCLEBARRIER          1ull
#define BITLENGTH_STAIRS                1ull

#define BITPOS_STREETTYPE               (0)
#define BITPOS_CYCLEWAYTYPE             (BITLENGTH_STREETTYPE)
#define BITPOS_STREETSURFACETYPE        (BITLENGTH_STREETTYPE + BITLENGTH_CYCLEWAYTYPE)
#define BITPOS_STREETSURFACEQUALITY     (BITLENGTH_STREETTYPE + BITLENGTH_CYCLEWAYTYPE + BITLENGTH_STREETSURFACETYPE)
#define BITPOS_TURNTYPE                 (BITLENGTH_STREETTYPE + BITLENGTH_CYCLEWAYTYPE + BITLENGTH_STREETSURFACETYPE + BITLENGTH_STREETSURFACEQUALITY)
#define BITPOS_TRAFFICLIGHTS            (BITLENGTH_STREETTYPE + BITLENGTH_CYCLEWAYTYPE + BITLENGTH_STREETSURFACETYPE + BITLENGTH_STREETSURFACEQUALITY + BITLENGTH_TURNTYPE)
#define BITPOS_TRAFFICCALMINGBUMPS      (BITLENGTH_STREETTYPE + BITLENGTH_CYCLEWAYTYPE + BITLENGTH_STREETSURFACETYPE + BITLENGTH_STREETSURFACEQUALITY + BITLENGTH_TURNTYPE + BITLENGTH_TRAFFICLIGHTS)
#define BITPOS_STOPSIGN                 (BITLENGTH_STREETTYPE + BITLENGTH_CYCLEWAYTYPE + BITLENGTH_STREETSURFACETYPE + BITLENGTH_STREETSURFACEQUALITY + BITLENGTH_TURNTYPE + BITLENGTH_TRAFFICLIGHTS + BITLENGTH_TRAFFICCALMINGBUMPS)
#define BITPOS_CYCLEBARRIER             (BITLENGTH_STREETTYPE + BITLENGTH_CYCLEWAYTYPE + BITLENGTH_STREETSURFACETYPE + BITLENGTH_STREETSURFACEQUALITY + BITLENGTH_TURNTYPE + BITLENGTH_TRAFFICLIGHTS + BITLENGTH_TRAFFICCALMINGBUMPS + BITLENGTH_STOPSIGN)
#define BITPOS_STAIRS                   (BITLENGTH_STREETTYPE + BITLENGTH_CYCLEWAYTYPE + BITLENGTH_STREETSURFACETYPE + BITLENGTH_STREETSURFACEQUALITY + BITLENGTH_TURNTYPE + BITLENGTH_TRAFFICLIGHTS + BITLENGTH_TRAFFICCALMINGBUMPS + BITLENGTH_STOPSIGN + BITLENGTH_CYCLEBARRIER)

/**
 * @brief Beschreibt eine Kante, der in der Datenbank abgelegt werden kann
 *      und fürs Routing verwendet wird.
 * 
 * 
 * 
 * @ingroup dataprimitives
 * @author Lena Brüder
 * @date 2011-11-01
 * @copyright GNU GPL v3
 * @todo Evtl. Rampen an Treppen dazu. Stufenanzahl in Schritten (1, 5, 10, 20, ...)
 * @todo verkehrsberuhigende sachen in mehr sachen aufteilen. 2 kategorien? umfahrbar/nicht
 * @todo Flag, ob Kante erlaubt für: Fahrrad, Fußgänger (gibt große Straßen wo Fußgänger nicht lang dürfen!)
 */
class RoutingEdge
{
private:
    boost::uint64_t _id;
    boost::uint64_t _startNodeID;
    boost::uint64_t _endNodeID;
    
public:
    typedef struct
    {
        unsigned int streetType             : BITLENGTH_STREETTYPE;
        unsigned int cyclewayType           : BITLENGTH_CYCLEWAYTYPE;
        unsigned int streetSurfaceType      : BITLENGTH_STREETSURFACETYPE;
        unsigned int streetSurfaceQuality   : BITLENGTH_STREETSURFACEQUALITY;
        unsigned int turnType               : BITLENGTH_TURNTYPE;
        bool trafficLights                  : BITLENGTH_TRAFFICLIGHTS;
        bool trafficCalmingBumps            : BITLENGTH_TRAFFICCALMINGBUMPS;
        bool stopSign                       : BITLENGTH_STOPSIGN;
        bool cycleBarrier                   : BITLENGTH_CYCLEBARRIER;
        bool stairs                         : BITLENGTH_STAIRS;
    } PropertyType;
private:
    PropertyType _properties;
    
    friend std::ostream & operator<<(std::ostream &os, const RoutingEdge& p);
public:
    /**
     * @brief Gibt die ID der Kante zurück.
     * @return Die ID der Kante.
     */
    boost::uint64_t getID() const {return _id;}
    
    /**
     * @brief Setzt die ID der Kante auf den entsprechenden Wert.
     * @param id Die neue ID der Kante.
     * @return 
     */
    void setID(const boost::uint64_t id) {this->_id = id;}
    
    /**
     * @brief Gibt die ID des Startknotens zurück.
     * @return Die ID des Startknotens.
     */
    boost::uint64_t getStartNodeID() const {return _startNodeID;}
    
    /**
     * @brief Setzt die ID des Startknotens auf den entsprechenden Wert.
     * @param startNodeID Die neue ID des Startknotens.
     * @return 
     */
    void setStartNodeID(const boost::uint64_t startNodeID) {this->_startNodeID = startNodeID;}
    
    /**
     * @brief Gibt die ID des Endknotens zurück.
     * @return Die ID des Endknotens.
     */
    boost::uint64_t getEndNodeID() const {return _endNodeID;}
    
    /**
     * @brief Setzt die ID des Endknotens auf den entsprechenden Wert.
     * @param endNodeID Die neue ID des Endknotens.
     */
    void setEndNodeID(const boost::uint64_t endNodeID) {this->_endNodeID = endNodeID;}
    
    /**
     * @brief Gibt die Eigenschaften der Kante als 64Bit-Wert zurück.
     * @return Die Eigenschaften der Kante als 64Bit-Wert.
     */
    boost::uint64_t getProperties() const;
    
    /**
     * @brief Setzt die Eigenschaften der Kante auf den angegebenen 64Bit-Wert.
     * @param properties Die neuen Eigenschaften der Kante als 64Bit-Wert.
     */
    void setProperties(const boost::uint64_t properties);
    
    /**
     * @brief Gibt an, ob an der entsprechenden Kante eine Ampel steht oder nicht.
     * @return Ob eine Ampel vorhanden ist
     */
    bool hasTrafficLights() const {return _properties.trafficLights;}
    /**
     * @brief Gibt an, ob an der entsprechenden Kante Bremsschwellen vorhanden sind
     * @return Ob Bremsschwellen vorhanden sind
     */
    bool hasTrafficCalmingBumps() const {return _properties.trafficCalmingBumps;}
    /**
     * @brief Gibt an, ob an der entsprechenden Kante ein Stoppschild vorhanden ist
     * @return Ob ein Stoppschild vorhanden ist
     */
    bool hasStopSign() const {return _properties.stopSign;}
    /**
     * @brief Gibt an, ob an der entsprechenden Kante Treppen sind
     * @return Ob an der Kante Treppen sind
     * @todo Evtl. sind Treppen ein eigener Straßentyp?
     */
    bool hasStairs() const {return _properties.stairs;}
    /**
     * @brief Gibt an, ob an der entsprechenden Kante ein Umlaufgitter o.Ä. vorhanden ist
     * @return Ob ein Umlaufgitter vorhanden ist
     */
    bool hasCycleBarrier() const {return _properties.cycleBarrier;}
    
    /**
     * @brief Legt fest, ob an der entsprechenden Kante eine Ampel ist oder nicht.
     * @param value 
     */
    void setTrafficLights(const bool value) {_properties.trafficLights = value;}
    
    /**
     * @brief Legt fest, ob an der entsprechenden Kante Bremsschwellen sind oder nicht.
     * @param value 
     */
    void setTrafficCalmingBumps(const bool value) {_properties.trafficCalmingBumps = value;}
    
    /**
     * @brief Legt fest, ob an der entsprechenden Kante ein Stoppschild ist oder nicht.
     * @param value 
     */
    void setStopSign(const bool value) {_properties.stopSign = value;}
    
    /**
     * @brief Legt fest, ob an der entsprechenden Kante Treppen sind oder nicht.
     * @param value 
     */
    void setStairs(const bool value) {_properties.stairs = value;}
    
    /**
     * @brief Legt fest, ob an der entsprechenden Kante Umlaufgitter sind oder nicht.
     * @param value 
     */
    void setCycleBarrier(const bool value) {_properties.cycleBarrier = value;}
    
    
    /**
     * @brief Gibt den Typ der Straße an der Kante an.
     * @return Den Typ der Straße
     * @todo Evtl einen Enum für den Straßentyp machen
     */
    boost::uint8_t getStreetType() const {return _properties.streetType;}
    /**
     * @brief Gibt den Typ des Radwegs an der Kante an.
     * @return Den Typ des Radwegs
     * @todo Evtl einen Enum für den Radwegtyp machen
     */
    boost::uint8_t getCyclewayType() const {return _properties.cyclewayType;}
    /**
     * @brief Gibt den Typ des Straßenbelags an
     * @return Den Typ des Straßenbelags
     * @todo Evtl einen Enum für den Belagstyp machen
     */
    boost::uint8_t getStreetSurfaceType() const {return _properties.streetSurfaceType;}
    /**
     * @brief Gibt die Qualität des Straßenbelags an.
     * @return Die Qualität des Straßenbelags der Kante
     */
    boost::uint8_t getStreetSurfaceQuality() const{return _properties.streetSurfaceQuality;}
    /**
     * @brief Gibt den Abbiegetyp der Kante an.
     * @return Den Abbiegetyp der Kante
     * @todo Evtl einen Enum für den Abbiegetyp machen. Geradeaus, links, rechts, umkehren. Evtl noch mehr?
     */
    boost::uint8_t getTurnType() const {return _properties.turnType;}
    
    /**
     * @brief Setzt den Typ der Straße fest
     * @param streetType Der Typ der Straße
     */
    void setStreetType(const boost::uint8_t streetType) {_properties.streetType = streetType;}
    /**
     * @brief Legt den Typ des Radwegs fest
     * @param cyclewayType Der Typ des Radwegs
     */
    void setCyclewayType(const boost::uint8_t cyclewayType) {_properties.cyclewayType = cyclewayType;}
    /**
     * @brief Legt den Typ des Straßenbelags fest
     * @param streetsurfaceType Der Typ des STraßenbelags
     */
    void setStreetSurfaceType(const boost::uint8_t streetSurfaceType) {_properties.streetSurfaceType = streetSurfaceType;}
    /**
     * @brief Legt die Qualität des Straßenbelags fest.
     * @param streetSurfaceQuality Die Qualität des Straßenbelags
     */
    void setStreetSurfaceQuality(const boost::uint8_t streetSurfaceQuality) {_properties.streetSurfaceQuality = streetSurfaceQuality;}
    /**
     * @brief Legt den Abbiegetyp der Kante fest
     * @param turnType Der Abbiegetyp der Kante
     */
    void setTurnType(const boost::uint8_t turnType) {_properties.turnType = turnType;}
    
    /**
     * @brief Gibt zurück, auf welchem Level die Kante eingeordnet ist.
     * 
     * Abkürzungen haben einen Wert größer als Null. Null steht für
     * "keine Abkürzung", und ist somit der Standardwert. Über diese
     * Eigenschaft kann geprüft werden, ob eine Kante Abkürzung ist, oder
     * nicht.
     * 
     * @return Das Abkürzungslevel dieser Kante.
     * @todo Implementierung in einer abgeleiteten Klasse, die das hier auch speichert.
     */
    virtual boost::uint64_t getRoutingLevel() {return 0;}
    /**
     * @brief Setzt fest, auf welchem Level die Kante eingeordnet ist.
     * 
     * Abkürzungen haben einen Wert größer als Null. Null steht für
     * "keine Abkürzung", und ist somit der Standardwert. Über diese
     * Eigenschaft kann geprüft werden, ob eine Kante Abkürzung ist, oder
     * nicht.
     * 
     * Diese Implementierung gibt immer Null für das Level zurück, und
     * speichert den Wert auch nicht.
     * 
     * @param level Das Abkürzungslevel dieser Kante.
     * @todo Implementierung in einer abgeleiteten Klasse, die das hier auch speichert.
     */
    virtual void setRoutingLevel(const boost::uint64_t level) {}
    
    RoutingEdge();
    RoutingEdge(boost::uint64_t id);
    RoutingEdge(boost::uint64_t id, boost::uint64_t startNodeID, boost::uint64_t endNodeID);
    RoutingEdge(boost::uint64_t id, boost::uint64_t startNodeID, boost::uint64_t endNodeID, boost::uint64_t properties);
    
    /**
     * @brief Vergleicht zwei RoutingEdges.
     * 
     * @return <code>true</code>, wenn die RoutingEdges gleich sind, <code>false</code> sonst.
     */
    bool operator==(const RoutingEdge& other);
};

/**
 * @brief Ausgabeoperator für RoutingEdge.
 */
std::ostream& operator<<(std::ostream& os, const RoutingEdge& edge);

/**
 * @brief Vergleichsoperator für den struct innerhalb der RoutingEdge
 * @return Ob zwei structs vom Typ RoutingEdge::PropertyType gleich sind
 */
bool operator==(const RoutingEdge::PropertyType& a, const RoutingEdge::PropertyType& b);

namespace biker_tests
{
    /**
     * @ingroup tests
     */
    int testRoutingEdge();
}

#endif //ROUTINGNODE_HPP
