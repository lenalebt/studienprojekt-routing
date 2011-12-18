#ifndef OSMRELATION_HPP
#define OSMRELATION_HPP

#include <boost/cstdint.hpp>

/**
 * @brief Eine OSMRelation stellt eine Relation im OSM-Datenmodell im Speicher dar.
 * @ingroup dataprimitives
 * @todo die Routen-Relationen sind noch nicht implementiert
 * @remarks Bei der Implementierung darauf achten, dass, wo möglich, explizit festgelegt wird
 *      wie viel Platz Variablen benutzen. Beispiel: uint64_t ist ein unsigned int mit 64 bit Breite.
 *      Man sollte daruf achten, dass alle Zugriffe auf eine restriction nur die Werte 0 und 1 wählen.
 * @author Thorsten Scheller
 * @date 2011-11-01
 * @copyright GNU GPL v3
 */
class OSMRelation
{
private:
    // die vordefinierten Datentypen
    boost::uint64_t viaId;
    boost::uint64_t fromId;
    boost::uint64_t toId;
    // eine Art Zusammenfassung von Boolischen Variablen
    struct restriction{
        bool  noLeft:   1;
        bool  noRight:   1;
        bool  noStraight:   1;
        bool  noUTurn:   1;
    }thisRestriction;
public:
    /* @brief erstellt eine Relation mit IDs und restriction
     * @param viaId die ID des Nodes, über die die Relation führt
     * @param fromId die ID des Ways, von dem aus die Relation beginnt
     * @param toId die ID des Ways, an dem die Relation endet
     * @param noLeft die Variable die anzeigt, ob man nach links abbiegen darf (noLeft = 0) oder nicht (noLeft = 1)
     * @param noRight die Variable die anzeigt, ob man nach rechts abbiegen darf (noRight = 0) oder nicht (noRight = 1)
     * @param noStraight die Variable die anzeigt, ob man gerade aus weiter fahren darf (noStraight = 0) oder nicht (noStraight = 1)
     * @param noUTurn die Variable die anzeigt, ob man umkehren darf (noUTurn = 0) oder nicht (noUTurn = 1)
     */
    OSMRelation( boost::uint64_t viaId,  boost::uint64_t fromId,  boost::uint64_t toId, bool  noLeft, bool  noStraight, bool  noRight, bool  noUTurn):
    viaId(viaId), fromId(fromId), toId(toId) {
        this->thisRestriction.noLeft = noLeft;
        this->thisRestriction.noRight = noRight;
        this->thisRestriction.noStraight = noStraight;
        this->thisRestriction.noUTurn = noUTurn;
    };
    // ab hier: Get und Set -Funktionen
    /**
     * @brief Gibt die viaId der Relation zurück.
     * @return Die viaId der Relation.
     */
    boost::uint64_t getViaId() const {return viaId;}
    /**
     * @brief Setzt die viaId der Relation.
     * @param viaId Die viaId der Relation.
     */
    void setViaId(boost::uint64_t viaId) {this->viaId = viaId;}
    /**
     * @brief Gibt die fromId der Relation zurück.
     * @return Die fromId der Relation.
     */
    boost::uint64_t getFromId() const {return fromId;}
    /**
     * @brief Setzt die fromId der Relation.
     * @param fromId Die fromId der Relation.
     */
    void setFromId(boost::uint64_t fromId) {this->fromId = fromId;}
    /**
     * @brief Gibt die toId der Relation zurück.
     * @return Die toId der Relation.
     */
    boost::uint64_t getToId() const {return toId;}
    /**
     * @brief Setzt die toId der Relation.
     * @param toId Die toId der Relation.
     */
    void setToId(boost::uint64_t toId) {this->toId = toId;}
    /**
     * @brief Gibt die restriction.noLeft der Relation zurück.
     * @return Die restriction.noLeft der Relation.
     */
    bool  getLeft() const {return thisRestriction.noLeft ;}
    /**
     * @brief Setzt die restriction.noLeft der Relation.
     * @param noLeft der neue Wert der restriction.noLeft der Relation.
     */
    void setLeft(bool  noLeft) {this->thisRestriction.noLeft = noLeft;}
    /**
     * @brief Gibt die restriction.noRight der Relation zurück.
     * @return Die restriction.noRight der Relation.
     */
    bool  getRight() const {return thisRestriction.noRight ;}
    /**
     * @brief Setzt die restriction.noRight der Relation.
     * @param noRight der neue Wert der restriction.noRight der Relation.
     */
    void setRight(bool  noRight) {this->thisRestriction.noRight = noRight;}
    /**
     * @brief Gibt die restriction.noStraight der Relation zurück.
     * @return Die restriction.noStraight der Relation.
     */
    bool  getStraight() const {return thisRestriction.noStraight ;}
    /**
     * @brief Setzt die restriction.noStraight der Relation.
     * @param noStraight der neue Wert der restriction.noStraight der Relation.
     */
    void setStraight(bool  noStraight) {this->thisRestriction.noStraight = noStraight;}
    /**
     * @brief Gibt die restriction.noUTurn der Relation zurück.
     * @return Die restriction.noUTurn der Relation.
     */
    bool  getUTurn() const {return thisRestriction.noUTurn ;}
    /**
     * @brief Setzt die restriction.noStraight der Relation.
     * @param noUTurn der neue Wert der restriction.noUTurn der Relation.
     */
    void setUTurn(bool  noUTurn) {this->thisRestriction.noUTurn = noUTurn;}
};

/**
 * @todo: Implementieren, dieser Test ist noch leer.
 */
namespace biker_tests
{
    int testOSMRelation();
} 

#endif // OSMRELATION_HPP
