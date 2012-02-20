#ifndef ROUTINGEDGE_HPP
#define ROUTINGEDGE_HPP

#include <boost/cstdint.hpp>
#include <iostream>
#include "tests.hpp"

#define STREETTYPE_HIGHWAY_PRIMARY              0
#define STREETTYPE_HIGHWAY_SECONDARY            1
#define STREETTYPE_HIGHWAY_TERTIARY             2
#define STREETTYPE_HIGHWAY_TRACK                3
#define STREETTYPE_HIGHWAY_PATH                 4
#define STREETTYPE_HIGHWAY_CYCLEWAY             5
#define STREETTYPE_HIGHWAY_FOOTWAY              6
#define STREETTYPE_HIGHWAY_LIVINGSTREET         7
#define STREETTYPE_HIGHWAY_RESIDENTIAL          8
#define STREETTYPE_HIGHWAY_UNKNOWN              9
#define STREETTYPE_HIGHWAY_PEDESTRIAN           10
#define STREETTYPE_HIGHWAY_SERVICE              11
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
#define CYCLEWAYTYPE_TRACK_SHARED_BUSWAY        9 //Cyclewaytrack, combined cycle- and footway
#define CYCLEWAYTYPE_TRACK_SHARED_BUSWAY        9 //Cyclewaytrack, combined cycle- and footway
#define CYCLEWAYTYPE_TRACK_SHARED_BUSWAY        9 //Cyclewaytrack, combined cycle- and footway

#define CYCLEWAYTYPE_UNKNOWN                    (1<<BITLENGTH_CYCLEWAYTYPE)-1 //last bit, highest possible value, there is a cycleway along this edge


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
#define TURNTYPE_RIGHTCROSS		1
#define TURNTYPE_STRAIGHTCROSS          2
#define TURNTYPE_LEFTCROSS		3
#define TURNTYPE_UTURNCROSS		4


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
#define BITLENGTH_CYCLEWAYDESIGNATED    1ull

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
#define BITPOS_CYCLEWAYDESIGNATED       (BITLENGTH_STREETTYPE + BITLENGTH_CYCLEWAYTYPE + BITLENGTH_STREETSURFACETYPE + BITLENGTH_STREETSURFACEQUALITY + BITLENGTH_TURNTYPE + BITLENGTH_TRAFFICLIGHTS + BITLENGTH_TRAFFICCALMINGBUMPS + BITLENGTH_STOPSIGN + BITLENGTH_CYCLEBARRIER + BITLENGTH_STAIRS)

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
        bool isDesignated                   : BITLENGTH_CYCLEWAYDESIGNATED;
    } PropertyType;
private:
    PropertyType _properties;
    
    friend std::ostream & operator<<(std::ostream &os, const RoutingEdge& p);
public:
    /**
     * @brief Gibt die ID der Kante zurück.
     * @return Die ID der Kante.
     */
    virtual boost::uint64_t getID() const {return _id;}
    
    /**
     * @brief Setzt die ID der Kante auf den entsprechenden Wert.
     * @param id Die neue ID der Kante.
     * @return 
     */
    virtual void setID(const boost::uint64_t id) {this->_id = id;}
    
    /**
     * @brief Gibt die ID des Startknotens zurück.
     * @return Die ID des Startknotens.
     */
    virtual boost::uint64_t getStartNodeID() const {return _startNodeID;}
    
    /**
     * @brief Setzt die ID des Startknotens auf den entsprechenden Wert.
     * @param startNodeID Die neue ID des Startknotens.
     * @return 
     */
    virtual void setStartNodeID(const boost::uint64_t startNodeID) {this->_startNodeID = startNodeID;}
    
    /**
     * @brief Gibt die ID des Endknotens zurück.
     * @return Die ID des Endknotens.
     */
    virtual boost::uint64_t getEndNodeID() const {return _endNodeID;}
    
    /**
     * @brief Setzt die ID des Endknotens auf den entsprechenden Wert.
     * @param endNodeID Die neue ID des Endknotens.
     */
    virtual void setEndNodeID(const boost::uint64_t endNodeID) {this->_endNodeID = endNodeID;}
    
    /**
     * @brief Gibt die Eigenschaften der Kante als 64Bit-Wert zurück.
     * @return Die Eigenschaften der Kante als 64Bit-Wert.
     */
    virtual boost::uint64_t getProperties() const;
    
    /**
     * @brief Setzt die Eigenschaften der Kante auf den angegebenen 64Bit-Wert.
     * @param properties Die neuen Eigenschaften der Kante als 64Bit-Wert.
     */
    virtual void setProperties(const boost::uint64_t properties);
    
    /**
     * @brief Gibt an, ob an der entsprechenden Kante eine Ampel steht oder nicht.
     * @return Ob eine Ampel vorhanden ist
     */
    virtual bool hasTrafficLights() const;
    /**
     * @brief Gibt an, ob an der entsprechenden Kante Bremsschwellen vorhanden sind
     * @return Ob Bremsschwellen vorhanden sind
     */
    virtual bool hasTrafficCalmingBumps() const;
    /**
     * @brief Gibt an, ob an der entsprechenden Kante ein Stoppschild vorhanden ist
     * @return Ob ein Stoppschild vorhanden ist
     */
    virtual bool hasStopSign() const;
    /**
     * @brief Gibt an, ob an der entsprechenden Kante Treppen sind
     * @return Ob an der Kante Treppen sind
     * @todo Evtl. sind Treppen ein eigener Straßentyp?
     */
    virtual bool hasStairs() const;
    /**
     * @brief Gibt an, ob an der entsprechenden Kante ein Umlaufgitter o.Ä. vorhanden ist
     * @return Ob ein Umlaufgitter vorhanden ist
     */
    virtual bool hasCycleBarrier() const;
    /**
     * @brief Gibt an, ob die Nutzung eines vorhandenen Fahradweges verpflichtend ist.
     * @return Ob vorhandener Fahrredweg genutzt werden MUSS.
     */
    virtual bool isCyclewayDesignated() const;
    
    /**
     * @brief Legt fest, ob an der entsprechenden Kante eine Ampel ist oder nicht.
     * @param value 
     */
    virtual void setTrafficLights(const bool value);
    
    /**
     * @brief Legt fest, ob an der entsprechenden Kante Bremsschwellen sind oder nicht.
     * @param value 
     */
    virtual void setTrafficCalmingBumps(const bool value);
    
    /**
     * @brief Legt fest, ob an der entsprechenden Kante ein Stoppschild ist oder nicht.
     * @param value 
     */
    virtual void setStopSign(const bool value);
    
    /**
     * @brief Legt fest, ob an der entsprechenden Kante Treppen sind oder nicht.
     * @param value 
     */
    virtual void setStairs(const bool value);
    
    /**
     * @brief Legt fest, ob an der entsprechenden Kante Umlaufgitter sind oder nicht.
     * @param value 
     */
    virtual void setCycleBarrier(const bool value);

    /**
     * @brief Legt fest, ob die Nutzung eines vorhandenen Fahrradweges verpflichtend ist oder nicht.
     * @param value
     */
    virtual void setCyclewayDesignated(const bool value);
    
    /**
     * @brief Gibt den Typ der Straße an der Kante an.
     * @return Den Typ der Straße
     * @todo Evtl einen Enum für den Straßentyp machen
     */
    virtual boost::uint8_t getStreetType() const;
    /**
     * @brief Gibt den Typ des Radwegs an der Kante an.
     * @return Den Typ des Radwegs
     * @todo Evtl einen Enum für den Radwegtyp machen
     */
    virtual boost::uint8_t getCyclewayType() const;
    /**
     * @brief Gibt den Typ des Straßenbelags an
     * @return Den Typ des Straßenbelags
     * @todo Evtl einen Enum für den Belagstyp machen
     */
    virtual boost::uint8_t getStreetSurfaceType() const;
    /**
     * @brief Gibt die Qualität des Straßenbelags an.
     * @return Die Qualität des Straßenbelags der Kante
     */
    virtual boost::uint8_t getStreetSurfaceQuality() const;
    /**
     * @brief Gibt den Abbiegetyp der Kante an.
     * @return Den Abbiegetyp der Kante
     * @todo Evtl einen Enum für den Abbiegetyp machen. Geradeaus, links, rechts, umkehren. Evtl noch mehr?
     */
    virtual boost::uint8_t getTurnType() const;
    
    /**
     * @brief Setzt den Typ der Straße fest
     * @param streetType Der Typ der Straße
     */
    virtual void setStreetType(const boost::uint8_t streetType);
    /**
     * @brief Legt den Typ des Radwegs fest
     * @param cyclewayType Der Typ des Radwegs
     */
    virtual void setCyclewayType(const boost::uint8_t cyclewayType);
    /**
     * @brief Legt den Typ des Straßenbelags fest
     * @param streetsurfaceType Der Typ des STraßenbelags
     */
    virtual void setStreetSurfaceType(const boost::uint8_t streetSurfaceType);
    /**
     * @brief Legt die Qualität des Straßenbelags fest.
     * @param streetSurfaceQuality Die Qualität des Straßenbelags
     */
    virtual void setStreetSurfaceQuality(const boost::uint8_t streetSurfaceQuality);
    /**
     * @brief Legt den Abbiegetyp der Kante fest
     * @param turnType Der Abbiegetyp der Kante
     */
    virtual void setTurnType(const boost::uint8_t turnType);
    
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
