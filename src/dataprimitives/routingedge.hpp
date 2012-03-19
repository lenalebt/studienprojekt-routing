#ifndef ROUTINGEDGE_HPP
#define ROUTINGEDGE_HPP

#include <boost/cstdint.hpp>
#include <iostream>
#include "tests.hpp"

//Streettypes
#define STREETTYPE_HIGHWAY_PEDESTRIAN           0
#define STREETTYPE_HIGHWAY_PRIMARY              1
#define STREETTYPE_HIGHWAY_SECONDARY            2
#define STREETTYPE_HIGHWAY_TERTIARY             3
#define STREETTYPE_HIGHWAY_TRACK                4
#define STREETTYPE_HIGHWAY_PATH                 5
#define STREETTYPE_HIGHWAY_LIVINGSTREET         6
#define STREETTYPE_HIGHWAY_RESIDENTIAL          7
#define STREETTYPE_HIGHWAY_SERVICE              8
#define STREETTYPE_HIGHWAY_FORD                 9
#define STREETTYPE_HIGHWAY_JUNCTION             10
#define STREETTYPE_HIGHWAY_UNCLASSIFIED         11 // expliciedly unclassified
#define STREETTYPE_UNKNOWN                      (1<<BITLENGTH_STREETTYPE)-1 //last bit, highest possible value


#define CYCLEWAYTYPE_NO_CYCLEWAY                0 //No Cycleway
#define CYCLEWAYTYPE_LANE                       1 //Cyclewaylane in direction of the road
#define CYCLEWAYTYPE_LANE_OP                    2 //Cyclewaylane in opposite direction of the road
#define CYCLEWAYTYPE_LANE_SEGREGAETD            3 //Cyclewaylane in direction of the road, combined cycle- and footlane
#define CYCLEWAYTYPE_LANE_SEGREGAETD_OP         4 //Cyclewaylane in opposite direction of the road, combined cycle- and footlane
#define CYCLEWAYTYPE_LANE_SHARED_BUSWAY         5 //Cyclewaylane in direction of the road, combined cycle- and buslane
#define CYCLEWAYTYPE_LANE_SHARED_BUSWAY_OP      6 //Cyclewaylane in opposite direction of the road, combined cycle- and footlane
#define CYCLEWAYTYPE_TRACK                      7 //Cyclewaytrack
#define CYCLEWAYTYPE_TRACK_SEGREGATED           8 //Cyclewaytrack, combined cycle- and footway
#define CYCLEWAYTYPE_TRACK_SHARED_BUSWAY        9 //Cyclewaytrack, combined cycle- and footway
#define CYCLEWAYTYPE_MTB_0                      10 //Mountainbiketrack, mtb:scale=0, no special driving skills
#define CYCLEWAYTYPE_MTB_1                      11 //Mountainbiketrack, mtb:scale=1, basic driving skills
#define CYCLEWAYTYPE_MTB_2                      12 //Mountainbiketrack, mtb:scale=2, advanced driving skills
#define CYCLEWAYTYPE_MTB_3                      13 //Mountainbiketrack, mtb:scale=3, very good bike control required
#define CYCLEWAYTYPE_MTB_HIGH                   14 //Mountainbiketrack, mtb:scale>=4, you're a little bit insane
#define CYCLEWAYTYPE_UNKNOWN                    (1<<BITLENGTH_CYCLEWAYTYPE)-1 //last bit, highest possible value, there is a cycleway along this edge // Used, whenever there is a cycleway with no further specifications


#define CYCLEWAYTYPE_UNKNOWN                    (1<<BITLENGTH_CYCLEWAYTYPE)-1 //last bit, highest possible value

//Smoothness
#define STREETSURFACEQUALITY_EXCELLENT    	0
#define STREETSURFACEQUALITY_GOOD  		1
#define STREETSURFACEQUALITY_INTERMEDIATE	2
#define STREETSURFACEQUALITY_BAD		3
#define STREETSURFACEQUALITY_VERYBAD		4
#define STREETSURFACEQUALITY_HORRIBLE		5
#define STREETSURFACEQUALITY_VERYHORRIBLE	6
#define STREETSURFACEQUALITY_IMPASSABLE		7
#define STREETSURFACEQUALITY_UNKNOWN		(1<<BITLENGTH_STREETSURFACEQUALITY)-1 //last bit, highest possible value


// PAVED Types:
#define STREETSURFACETYPE_PAVED			0
#define STREETSURFACETYPE_ASPHALT		1
#define STREETSURFACETYPE_SETT			2
#define STREETSURFACETYPE_PAVING_STONES		3
#define STREETSURFACETYPE_TARTAN		4
#define STREETSURFACETYPE_CONCRETE              5
#define STREETSURFACETYPE_COBBLESTONE           6
// UNPAVED Types:
#define STREETSURFACETYPE_COMPACTED		7
#define STREETSURFACETYPE_FINEGRAVEL            8
#define STREETSURFACETYPE_GRASSPAVER            9
#define STREETSURFACETYPE_GRAVEL		10 //GRAVEL, PEBBLESTONE
#define STREETSURFACETYPE_GROUND		11 //DIRT, MUD, EARTH, GROUND, CLAY, SAND
#define STREETSURFACETYPE_GRASS 		12 //GRASS, ARTIFICIAL_TURF
#define STREETSURFACETYPE_METAL                 13
#define STREETSURFACETYPE_UNPAVED		14
// NO Type:
#define STREETSURFACETYPE_UNKNOWN		(1<<BITLENGTH_STREETSURFACETYPE)-1 //last bit, highest possible value


//Umlaufgitter etc: nur wirkliche Hindernisse zaehlen als Hindernis.
//Ein Poller, an dem man vorbeifahren kann z.B. nicht.

#define TURNTYPE_STRAIGHT		0 //no crossing
#define TURNTYPE_RIGHTCROSS		2
#define TURNTYPE_STRAIGHTCROSS          1
#define TURNTYPE_LEFTCROSS		4
#define TURNTYPE_UTURNCROSS		3

#define ACCESS_YES			0 // at least for bikes
#define ACCESS_PERMISSIVE		1
#define ACCESS_COMPULSORY		2 // if compulsory for bikes
#define ACCESS_DESIGNATED		3 // if designated for bike
#define ACCESS_FORESTRY			4
#define ACCESS_AGRICULTURAL		5
#define ACCESS_CUSTOMER			6 // open for customers
#define ACCESS_DESTINATION              7 // access only to reach a certain destination
#define ACCESS_DELIVERY			8 // open for delivery
#define ACCESS_PRIVATE			9
#define ACCESS_NOT_PERMITTED		10
#define ACCESS_NOT_USABLE_FOR_BIKES	11 // like motorways, impassable surfaces
#define ACCESS_FOOT_ONLY        	12 // You're not allowed to ride your bike, but you may push it.
#define ACCESS_UNKNOWN			(1<<BITLENGTH_ACCESS)-1 //last bit, highest possible value


#define BITLENGTH_STREETTYPE            4ull
#define BITLENGTH_CYCLEWAYTYPE          4ull
#define BITLENGTH_STREETSURFACETYPE     4ull
#define BITLENGTH_STREETSURFACEQUALITY  4ull
#define BITLENGTH_TURNTYPE              4ull
#define BITLENGTH_ACCESS                4ull
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
#define BITPOS_ACCESS                   (BITLENGTH_STREETTYPE + BITLENGTH_CYCLEWAYTYPE + BITLENGTH_STREETSURFACETYPE + BITLENGTH_STREETSURFACEQUALITY + BITLENGTH_TURNTYPE)
#define BITPOS_TRAFFICLIGHTS            (BITLENGTH_STREETTYPE + BITLENGTH_CYCLEWAYTYPE + BITLENGTH_STREETSURFACETYPE + BITLENGTH_STREETSURFACEQUALITY + BITLENGTH_TURNTYPE + BITLENGTH_ACCESS)
#define BITPOS_TRAFFICCALMINGBUMPS      (BITLENGTH_STREETTYPE + BITLENGTH_CYCLEWAYTYPE + BITLENGTH_STREETSURFACETYPE + BITLENGTH_STREETSURFACEQUALITY + BITLENGTH_TURNTYPE + BITLENGTH_ACCESS + BITLENGTH_TRAFFICLIGHTS)
#define BITPOS_STOPSIGN                 (BITLENGTH_STREETTYPE + BITLENGTH_CYCLEWAYTYPE + BITLENGTH_STREETSURFACETYPE + BITLENGTH_STREETSURFACEQUALITY + BITLENGTH_TURNTYPE + BITLENGTH_ACCESS + BITLENGTH_TRAFFICLIGHTS + BITLENGTH_TRAFFICCALMINGBUMPS)
#define BITPOS_CYCLEBARRIER             (BITLENGTH_STREETTYPE + BITLENGTH_CYCLEWAYTYPE + BITLENGTH_STREETSURFACETYPE + BITLENGTH_STREETSURFACEQUALITY + BITLENGTH_TURNTYPE + BITLENGTH_ACCESS + BITLENGTH_TRAFFICLIGHTS + BITLENGTH_TRAFFICCALMINGBUMPS + BITLENGTH_STOPSIGN)
#define BITPOS_STAIRS                   (BITLENGTH_STREETTYPE + BITLENGTH_CYCLEWAYTYPE + BITLENGTH_STREETSURFACETYPE + BITLENGTH_STREETSURFACEQUALITY + BITLENGTH_TURNTYPE + BITLENGTH_ACCESS + BITLENGTH_TRAFFICLIGHTS + BITLENGTH_TRAFFICCALMINGBUMPS + BITLENGTH_STOPSIGN + BITLENGTH_CYCLEBARRIER)

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
        unsigned int access                 : BITLENGTH_ACCESS;
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
    bool hasCycleBarrier() const {return _properties.cycleBarrier;};
    
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
     */
    boost::uint8_t getTurnType() const {return _properties.turnType;}
    /**
     * @brief Gibt die Zugangsberechtigung für eine Kante an.
     * @return Zugangsberechtigung einer Kante
     */
    boost::uint8_t getAccess() const {return _properties.access;}

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
     * @brief Legt die Zugangsberechitngung der Kante fest
     * @param access Die Zugangsberechtigung der Kante
     */
    void setAccess(const boost::uint8_t access) {_properties.access = access;}
    
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
