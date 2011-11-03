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
     * @return Die ID des Knotens
     */
    virtual boost::uint64_t getID() {return id;}
    
    /**
     * @brief Setzt die ID des Knotens auf den entsprechenden Wert.
     * @param id Die neue ID des Knotens.
     * @return 
     */
    virtual void setID(boost::uint64_t id) {this->id = id;}
};

#endif //ROUTINGNODE_HPP
