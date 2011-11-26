#ifndef ROUTINGEDGE_HPP
#define ROUTINGNODE_HPP

#include <boost/cstdint.hpp>
#include <iostream>
#include "tests.hpp"

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
 * @todo Tests, Header evtl erweitern
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
    virtual boost::uint64_t getID() const {return _id;}
    
    /**
     * @brief Setzt die ID der Kante auf den entsprechenden Wert.
     * @param id Die neue ID der Kante.
     * @return 
     */
    virtual void setID(boost::uint64_t id) {this->_id = id;}
    
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
    virtual void setStartNodeID(boost::uint64_t startNodeID) {this->_startNodeID = startNodeID;}
    
    /**
     * @brief Gibt die ID des Endknotens zurück.
     * @return Die ID des Endknotens.
     */
    virtual boost::uint64_t getEndNodeID() const {return _endNodeID;}
    
    /**
     * @brief Setzt die ID des Endknotens auf den entsprechenden Wert.
     * @param endNodeID Die neue ID des Endknotens.
     */
    virtual void setEndNodeID(boost::uint64_t endNodeID) {this->_endNodeID = endNodeID;}
    
    /**
     * @brief Gibt die Eigenschaften der Kante als 64Bit-Wert zurück.
     * @return Die Eigenschaften der Kante als 64Bit-Wert.
     */
    virtual boost::uint64_t getProperties() const;
    
    /**
     * @brief Setzt die Eigenschaften der Kante auf den angegebenen 64Bit-Wert.
     * @param properties Die neuen Eigenschaften der Kante als 64Bit-Wert.
     */
    virtual void setProperties(boost::uint64_t properties);
    
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
     * @brief Legt fest, ob an der entsprechenden Kante eine Ampel ist oder nicht.
     * @param value 
     */
    virtual void setTrafficLights(bool value);
    
    /**
     * @brief Legt fest, ob an der entsprechenden Kante Bremsschwellen sind oder nicht.
     * @param value 
     */
    virtual void setTrafficCalmingBumps(bool value);
    
    /**
     * @brief Legt fest, ob an der entsprechenden Kante ein Stoppschild ist oder nicht.
     * @param value 
     */
    virtual void setStopSign(bool value);
    
    /**
     * @brief Legt fest, ob an der entsprechenden Kante Treppen sind oder nicht.
     * @param value 
     */
    virtual void setStairs(bool value);
    
    /**
     * @brief Legt fest, ob an der entsprechenden Kante Umlaufgitter sind oder nicht.
     * @param value 
     */
    virtual void setCycleBarrier(bool value);
    
    
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
    virtual void setStreetType(boost::uint8_t streetType);
    /**
     * @brief Legt den Typ des Radwegs fest
     * @param cyclewayType Der Typ des Radwegs
     */
    virtual void setCyclewayType(boost::uint8_t cyclewayType);
    /**
     * @brief Legt den Typ des Straßenbelags fest
     * @param streetsurfaceType Der Typ des STraßenbelags
     */
    virtual void setStreetSurfaceType(boost::uint8_t streetSurfaceType);
    /**
     * @brief Legt die Qualität des Straßenbelags fest.
     * @param streetSurfaceQuality Die Qualität des Straßenbelags
     */
    virtual void setStreetSurfaceQuality(boost::uint8_t streetSurfaceQuality);
    /**
     * @brief Legt den Abbiegetyp der Kante fest
     * @param turnType Der Abbiegetyp der Kante
     */
    virtual void setTurnType(boost::uint8_t turnType);
    
    /**
     * @brief Gibt zurück, auf welchem Level die Kante eingeordnet ist.
     * 
     * Abkürzungen haben einen Wert größer als Null. Null steht für
     * "keine Abkürzung", und ist somit der Standardwert. Über diese
     * Eigenschaft kann geprüft werden, ob eine Kante Abkürzung ist, oder
     * nicht.
     * 
     * @return Das Abkürzungslevel dieser Kante.
     * @todo Implementierung einer abgeleiteten Klasse, die das hier auch speichert.
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
     * @todo Implementierung einer abgeleiteten Klasse, die das hier auch speichert.
     */
    virtual void setRoutingLevel(boost::uint64_t level) {}
    
    RoutingEdge();
    RoutingEdge(boost::uint64_t id);
    
    /**
     * @brief Vergleicht zwei RoutingEdges.
     * 
     * @return <code>true</code>, wenn die RoutingEdges gleich sind, <code>false</code> sonst.
     * @todo 
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
    int testRoutingEdge();
}

#endif //ROUTINGNODE_HPP
