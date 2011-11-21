#ifndef OSMRELATION_HPP
#define OSMRELATION_HPP

#include <boost/cstdint.hpp>

/**
 * @brief Eine OSMRelation stellt eine Relation im OSM-Datenmodell im Speicher dar.
 *
 *
 * @ingroup dataprimitives
 * @todo die Routen-Relationen sind noch nicht implementiert
 * @remarks Bei der Implementierung darauf achten, dass, wo möglich, explizit festgelegt wird
 *      wie viel Platz Variablen benutzen. Beispiel: uint64_t ist ein unsigned int mit 64 bit Breite.
 *      Man sollte daruf achten, dass alle Zugriffe auf eine restriction nur die Werte 0 und 1 wählen.
 * @author Lena Brueder
 * @date 2011-11-01
 * @copyright GNU GPL v3
 */
class OSMRelation
{
private:
    // die vordefinierten Datentypen
    boost::uint64_t via_id;
    boost::uint64_t from_id;
    boost::uint64_t to_id;
    // eine Art Zusammenfassung von Boolischen Variablen
    struct restriction{
        unsigned int no_left:   1;
        unsigned int no_right:   1;
        unsigned int no_straight:   1;
        unsigned int no_u_turn:   1;
    }this_restriction;
public:
    /* @brief erstellt eine Relation mit IDs und restriction
     * @param via_id die ID des Nodes, über die die Relation führt
     * @param from_id die ID des Ways, von dem aus die Relation beginnt
     * @param to_id die ID des Ways, an dem die Relation endet
     * @param no_left die Variable die anzeigt, ob man nach links abbiegen darf (no_left = 0) oder nicht (no_left = 1)
     * @param no_right die Variable die anzeigt, ob man nach rechts abbiegen darf (no_right = 0) oder nicht (no_right = 1)
     * @param no_straight die Variable die anzeigt, ob man gerade aus weiter fahren darf (no_straight = 0) oder nicht (no_straight = 1)
     * @param no_u_turn die Variable die anzeigt, ob man umkehren darf (no_u_turn = 0) oder nicht (no_u_turn = 1)
     */
    OSMRelation( boost::uint64_t via_id,  boost::uint64_t from_id,  boost::uint64_t to_id, unsigned int no_left, unsigned int no_straight, unsigned int no_right, unsigned int no_u_turn):
    via_id(via_id), from_id(from_id), to_id(to_id) {
    	this->this_restriction.no_left = no_left;
    	this->this_restriction.no_right = no_right;
    	this->this_restriction.no_straight = no_straight;
    	this->this_restriction.no_u_turn = no_u_turn;
    };
    // ab hier: Get und Set -Funktionen
    /**
     * @brief Gibt die via_ID der Relation zurück.
     * @return Die via_ID der Relation.
     */
    boost::uint64_t getvia_ID() const {return via_id;}
    /**
     * @brief Setzt die via_ID der Relation.
     * @param via_id Die via_ID der Relation.
     */
    void setvia_ID(boost::uint64_t via_id) {this->via_id = via_id;}
    /**
     * @brief Gibt die from_ID der Relation zurück.
     * @return Die from_ID der Relation.
     */
    boost::uint64_t getfrom_ID() const {return from_id;}
    /**
     * @brief Setzt die from_ID der Relation.
     * @param from_id Die from_ID der Relation.
     */
    void setfrom_ID(boost::uint64_t from_id) {this->from_id = from_id;}
    /**
     * @brief Gibt die to_ID der Relation zurück.
     * @return Die to_ID der Relation.
     */
    boost::uint64_t getto_ID() const {return to_id;}
    /**
     * @brief Setzt die to_ID der Relation.
     * @param to_id Die to_ID der Relation.
     */
    void setto_ID(boost::uint64_t to_id) {this->to_id = to_id;}
    /**
     * @brief Gibt die restriction.no_left der Relation zurück.
     * @return Die restriction.no_left der Relation.
     */
    unsigned int get_left() const {return this_restriction.no_left ;}
    /**
     * @brief Setzt die restriction.no_left der Relation.
     * @param no_left der neue Wert der restriction.no_left der Relation.
     */
    void set_left(unsigned int no_left) {this->this_restriction.no_left = no_left;}
    /**
     * @brief Gibt die restriction.no_right der Relation zurück.
     * @return Die restriction.no_right der Relation.
     */
    unsigned int get_right() const {return this_restriction.no_right ;}
    /**
     * @brief Setzt die restriction.no_right der Relation.
     * @param no_right der neue Wert der restriction.no_right der Relation.
     */
    void set_right(unsigned int no_right) {this->this_restriction.no_right = no_right;}
    /**
     * @brief Gibt die restriction.no_straight der Relation zurück.
     * @return Die restriction.no_straight der Relation.
     */
    unsigned int get_straight() const {return this_restriction.no_straight ;}
    /**
     * @brief Setzt die restriction.no_straight der Relation.
     * @param no_straight der neue Wert der restriction.no_straight der Relation.
     */
    void set_straight(unsigned int no_straight) {this->this_restriction.no_straight = no_straight;}
    /**
     * @brief Gibt die restriction.no_u_turn der Relation zurück.
     * @return Die restriction.no_u_turn der Relation.
     */
    unsigned int get_u_turn() const {return this_restriction.no_u_turn ;}
    /**
     * @brief Setzt die restriction.no_straight der Relation.
     * @param no_u_turn der neue Wert der restriction.no_u_turn der Relation.
     */
    void set_u_turn(unsigned int no_u_turn) {this->this_restriction.no_u_turn = no_u_turn;}
};

#endif // OSMRELATION_HPP
