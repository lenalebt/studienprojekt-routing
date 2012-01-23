#ifndef OSMEDGE_HPP
#define OSMEDGE_HPP

#include "osmproperty.hpp"
#include "tests.hpp"
#include <QVector>
#include <boost/cstdint.hpp>
#include <iostream>

/**
 * @brief Eine Kante nach OSM-Datenmodell.
 * 
 * 
 * 
 * @date 2011-12-23
 * @copyright GNU GPL v3
 * @ingroup dataprimitives
 */
class OSMEdge
{
private:
    boost::uint64_t id;
    boost::uint64_t startNode;
    boost::uint64_t endNode;
    QVector<OSMProperty> properties;
    
public:
    /**
     * @brief Erstellt eine Edge mit angegebenen Standardeigenschaften (keine) und ohne zugehörigen Knoten.
     */
    OSMEdge(){};
    /**
     * @brief Erstellt eine Edge mit angegebener ID, Standardeigenschaften (keine) und ohne zugehörigen Knoten.
     * @param id Die ID des Weges zu dem die Edge gehört.
     */
    OSMEdge(boost::uint64_t id) : id(id), startNode(0), endNode(0) {};
    /**
     * @brief Erstellt eine Edge mit angegebener ID und Eigenschaften, ohne zugehörige Knoten.
     * @param id Die ID des Weges zu dem die Edge gehört.
     * @param propList Die zugehörigen Eigenschaften der Edge.
     */
    OSMEdge(boost::uint64_t id, QVector<OSMProperty> propList) : id(id), startNode(0), endNode(0), properties(propList) {};
    /**
     * @brief Erstellt eine Edge mit angegebener ID, Eigenschaften und zugehörigen Knoten.
     * @param id Die ID des Weges zu dem die Edge gehört.
     * @param startNode Startknoten der Edge.
     * @param endNode Endknoten der Edge.
     * @param propList Die zugehörigen Eigenschaften der Edge.
     */
    OSMEdge(boost::uint64_t id, boost::uint64_t startNode, boost::uint64_t endNode, QVector<OSMProperty> propList) : id(id), startNode(startNode), endNode(endNode), properties(propList) {};
    /**
     * @brief Gibt die ID des Weges zu dem die Edge gehört zurück.
     * @return Die ID des Weges zu dem die Edge gehört.
     */
    boost::uint64_t getID() const {return id;}
    /**
     * @brief Setzt die ID des Weges zu dem die Edge gehört.
     * @param id Die Die ID des Weges zu dem die Edge gehört.
     */
    void setID(const boost::uint64_t id) {this->id = id;}
    /**
     * @brief Setzt die Knoten der Edge.
     * @param startNode Startknoten der Edge.
     * @param endNode Endknoten der Edge.
     */
    void setNodes(const boost::uint64_t startNode, const boost::uint64_t endNode) {this->startNode = startNode; this->endNode = endNode;}
    /**
     * @brief Gibt den zugehörigen Startknoten zurück.
     * @return Startknoten.
     */
    boost::uint64_t getStartNode() const {return startNode;}
    /**
     * @brief Gibt den zugehörigen Endknoten zurück.
     * @return Endknoten.
     */
    boost::uint64_t getEndNode() const {return endNode;}
    /**
     * @brief Gibt in einer Liste die Zugehörigen Knoten in der Reichenfolge Startknoten, Endknoten zurück..
     * @return Liste der Knoten (Reihenfolde: Startknoten, Endknoten).
     */
    QVector<boost::uint64_t> getNodes() const {QVector<boost::uint64_t> nodes; return nodes << startNode << endNode;}
    /**
     * @brief Fügt eine Eigenschaft zur Liste der Eigenschaften hinzu.
     * @param prop Die Eigenschaft, die hinzugefügt werden soll.
     * @remarks Die Liste der Eigenschaften ist nicht notwendigerweise geordnet.
     */
    void addProperty(const OSMProperty prop) {properties << prop;}
    /**
     * @brief Gibt die Liste der zugehörigen Eigenschaften zurück.
     * @return Die Liste der zugehörigen Eigenschaften.
     * @remarks Die Liste der Eigenschaften ist nicht notwendigerweise geordnet.
     */
    QVector<OSMProperty> getProperties() const {return properties;}
    
};

/**
 * @brief Vergleicht 2 OSMEdges miteinander
 * @return Gibt an, ob die beiden OSMEdges gleich sind
 */
bool operator==(const OSMEdge& e1, const OSMEdge& e2);
/**
 * @brief Gibt eine OSMEdge auf einem Ausgabestrom aus.
 */
std::ostream& operator<<(std::ostream& os, const OSMEdge& e);

namespace biker_tests
{
    /**
     * @ingroup tests
     */
    int testOSMEdge();
}

#endif  //OSMEDGE_HPP
