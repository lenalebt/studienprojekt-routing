#ifndef ROUTINGNODE_HPP
#define ROUTINGNODE_HPP

#include <boost/cstdint.hpp>
#include "gpsposition.hpp"

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
    virtual boost::uint64_t getID() {return id;}
    
    /**
     * @brief Setzt die ID des Knotens auf den entsprechenden Wert.
     * 
     * 
     * @param id Die neue ID des Knotens.
     */
    virtual void setID(boost::uint64_t id) {this->id = id;}
    
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
    virtual void setAndConvertID(boost::uint64_t id)
    {
        boost::uint64_t mask = 0xFFFFFFFFFFFFFFl;
        this->id = ((id & mask) << 8);
    }
};

#endif //ROUTINGNODE_HPP
