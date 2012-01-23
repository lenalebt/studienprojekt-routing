#ifndef OSMNODE_HPP
#define OSMNODE_HPP

#include "gpsposition.hpp"
#include "osmproperty.hpp"
#include <QVector>
#include <iostream>
#include <boost/cstdint.hpp>
#include "tests.hpp"

/**
 * @brief Eine OSMNode stellt einen Knoten nach dem OSM-Datenmodell im Speicher dar.
 * 
 * Eine OSMNode wird beim Einlesen der OSM-Daten erstellt und wird temporär dafür
 * verwendet, den Routinggraphen zu erstellen.
 * 
 * @ingroup dataprimitives
 * @author Lena Brueder
 * @date 2011-11-01
 * @copyright GNU GPL v3
 */
class OSMNode : public GPSPosition
{
private:
    boost::uint64_t id;
    QVector<OSMProperty> properties;

public:
    /**
     * @brief Erstellt eine OSMNode mit Standardwerten für die Position und <code>id==0</code>.
     */
    OSMNode() : GPSPosition(), id(0), properties(QVector<OSMProperty>()) {}
    /**
     * @brief Erstellt eine OSMNode mit den entsprechenden übergebenen Werten und leerer Eigenschaftenliste.
     * @param id Die ID des Knotens
     * @param pos Die GPSPosition des Knotens
     */
    OSMNode(boost::uint64_t id, const GPSPosition& pos) : GPSPosition(pos), id(id), properties(QVector<OSMProperty>()) {}
    /**
     * @brief Erstellt eine OSMNode mit den entsprechenden übergebenen Werten.
     * @param id Die ID des Knotens
     * @param pos Die GPSPosition des Knotens
     * @param props Die Eigenschaftenliste des Knotens
     */
    OSMNode(boost::uint64_t id, const GPSPosition& pos, QVector<OSMProperty> props) : GPSPosition(pos), id(id), properties(props) {}
    ~OSMNode() {}
    /**
     * @brief Gibt die Eigenschaftenliste des Knotens zurück.
     * @return die Eigenschaftenliste
     */
    QVector<OSMProperty> getProperties() const {return properties;}
    /**
     * @brief Fügt eine OSMProperty der Eigenschaftenliste hinzu.
     * @param prop Die Eigenschaft
     */
    void addProperty(const OSMProperty& prop) {properties << prop;}
    /**
     * @brief Gibt die ID des Knotens zurück.
     * @return Die ID.
     */
    boost::uint64_t getID() const {return id;}
    /**
     * @brief Setzt die ID des Knotens.
     * @param id Die ID des Knotens.
     */
    void setID(const boost::uint64_t id) {this->id = id;}
};

/**
 * @brief Vergleicht 2 OSMNodes miteinander
 * @return Ob die beiden OSMNodes gleich sind
 */
bool operator==(const OSMNode& p1, const OSMNode& p2);
/**
 * @brief Gibt eine OSMNode auf einem Ausgabestrom aus.
 */
std::ostream& operator<<(std::ostream& os, const OSMNode& p);

namespace biker_tests
{
    /**
     * @ingroup tests
     */
    int testOSMNode();
}

#endif // OSMNODE_HPP
