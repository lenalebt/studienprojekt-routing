#ifndef ROUTINGNODE_HPP
#define ROUTINGNODE_HPP

#include <boost/cstdint.hpp>
#include "gpsposition.hpp"
#include <iostream>

/**
 * @brief Beschreibt einen Knoten, der in der Datenbank abgelegt werden kann
 *      und fürs Routing verwendet wird.
 * 
 * 
 * 
 * @ingroup dataprimitives
 * @author Lena Brüder
 * @date 2011-11-01
 * @copyright GNU GPL v3
 * @todo Evtl erweitern?
 */
class RoutingNode : public GPSPosition
{
private:
    boost::uint64_t id;
public:
    /**
     * @brief Gibt die ID des Knotens zurück.
     * @return Die ID des Knotens.
     */
    boost::uint64_t getID() const {return id;}
    
    /**
     * @brief Setzt die ID des Knotens auf den entsprechenden Wert.
     * 
     * 
     * @param id Die neue ID des Knotens.
     */
    void setID(const boost::uint64_t id) {this->id = id;}
    
    /**
     * @brief Setzt die ID des Knotens auf den entsprechenden Wert und
     *      konvertiert ihn ins richige Format.
     * 
     * Die ID des Knotens wird folgendermaßen behandelt: Zuerst werden
     * nur die unteren 56bit des Ergebnisses behandelt (->Bitmaske).
     * Anschließend wird das Ergebnis um 8 Bit nach links verschoben:
     * Dadurch sind nun die folgenden 255 Werte frei zur Verwendung als ID
     * in einer Kante.
     * 
     * @param id Die neue ID des Knotens.
     */
    void setAndConvertID(const boost::uint64_t id)
    {
        this->id = convertIDToLongFormat(id);
    }
    
    /**
     * @brief Wandelt eine ID in das Format um, in dem Kanten KnotenID
     *  (für Start- und Endknoten) speichern.
     * 
     * @param id Die zu konvertierende ID
     * @return Die konvertierte ID
     */
    static inline boost::uint64_t convertIDToLongFormat(const boost::uint64_t id)
    {
        if (RoutingNode::isIDInLongFormat(id))
            return id;
        boost::uint64_t mask = 0x00FFFFFFFFFFFFFFllu;
        boost::uint64_t mark = 0x4000000000000000llu;
        return ((id & mask) << 8) | mark;
    }
    
    /**
     * @brief Wandelt eine ID in das Format um, in dem Knoten ihre ID
     *  speichern.
     * 
     * @param id Die zu konvertierende ID
     * @return Die konvertierte ID
     */
    static inline boost::uint64_t convertIDToShortFormat(const boost::uint64_t id)
    {
        if (!RoutingNode::isIDInLongFormat(id))
            return id;
        boost::uint64_t mark = 0x4000000000000000llu;
        return ((id & ~mark) >> 8);
    }
    
    bool isIDInLongFormat()
    {
        boost::uint64_t mark = 0x4000000000000000llu;
        return (id & mark);
    }
    static inline bool isIDInLongFormat(const boost::uint64_t id)
    {
        boost::uint64_t mark = 0x4000000000000000llu;
        return (id & mark);
    }
    
    RoutingNode(int id) : id(id) {}
    RoutingNode(int id, GPSPosition pos) : GPSPosition(pos), id(id) {}
    RoutingNode(int id, gps_float lat, gps_float lon) : GPSPosition(lat, lon), id(id) {}
    RoutingNode() : id(0) {}
    
    bool operator==(const RoutingNode& other)
    {
        return (this->id == other.id) &&
            (this->getLat() == other.getLat()) &&
            (this->getLon() == other.getLon());
    }
};

std::ostream& operator<<(std::ostream& os, const RoutingNode& node);

namespace biker_tests
{
    /**
     * @ingroup tests
     */
    int testRoutingNode();
}

#endif //ROUTINGNODE_HPP
