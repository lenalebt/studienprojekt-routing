#ifndef OSMEDGE_HPP
#define OSMEDGE_HPP

#include "osmproperty.hpp"
#include "tests.hpp"
#include <QVector>
#include "QVectorIterator"
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
    bool forward;
    boost::uint64_t startNodeID;
    boost::uint64_t endNodeID;
    QVector<OSMProperty> properties;

    static QVector<OSMProperty> onewayProperties();
    static QVector<OSMProperty> noOnewayProperties();
    static QVector<OSMProperty> reverseOnewayProperties();
    static QVector<OSMProperty> bikeOnewayProperties();
    static QVector<OSMProperty> bikeReverseOnewayProperties();
    static QVector<OSMProperty> noBikeOnewayProperties();
    
public:
    /**
     * @brief Erstellt eine Edge mit angegebenen Standardeigenschaften (keine) und ohne zugehörigen Knoten.
     */
    OSMEdge(){};
    /**
     * @brief Erstellt eine Edge mit angegebener ID, Standardeigenschaften (keine) und ohne zugehörigen Knoten. Richtugn ist per default vorwärts (true).
     * @param id Die ID des Weges zu dem die Edge gehört.
     */
    OSMEdge(boost::uint64_t id) : id(id), forward(true), startNodeID(0), endNodeID(0), properties() {};
    /**
     * @brief Erstellt eine Edge mit angegebener ID, Standardeigenschaften (keine) und ohne zugehörigen Knoten.
     * @param id Die ID des Weges zu dem die Edge gehört.
     * @param forward Die Richtung der Kante in Bezug auf den zugehörigen Weg.
     */
    OSMEdge(boost::uint64_t id, bool forward) : id(id), forward(forward), startNodeID(0), endNodeID(0), properties() {};
    /**
     * @brief Erstellt eine Edge mit angegebener ID und Eigenschaften, ohne zugehörige Knoten.
     * @param id Die ID des Weges zu dem die Edge gehört.
     * @param propList Die zugehörigen Eigenschaften der Edge.
     */
    OSMEdge(boost::uint64_t id, QVector<OSMProperty> propList) : id(id), forward(true), startNodeID(0), endNodeID(0), properties(propList) {};
    /**
     * @brief Erstellt eine Edge mit angegebener ID, Eigenschaften und zugehörigen Knoten.
     * @param id Die ID des Weges zu dem die Edge gehört.
     * @param startNode Startknoten der Edge.
     * @param endNode Endknoten der Edge.
     * @param propList Die zugehörigen Eigenschaften der Edge.
     */
    OSMEdge(boost::uint64_t id, boost::uint64_t startNode, boost::uint64_t endNode, QVector<OSMProperty> propList) : id(id), forward(true), startNodeID(startNode), endNodeID(endNode), properties(propList) {};
    /**
     * @brief Erstellt eine Edge mit angegebener ID, Eigenschaften und zugehörigen Knoten.
     * @param id Die ID des Weges zu dem die Edge gehört.
     * @param forward Die Richtung der Kante in Bezug auf den zugehörigen Weg.
     * @param startNode Startknoten der Edge.
     * @param endNode Endknoten der Edge.
     */
    OSMEdge(boost::uint64_t id, bool forward, boost::uint64_t startNode, boost::uint64_t endNode) : id(id), forward(forward), startNodeID(startNode), endNodeID(endNode), properties() {};
    /**
     * @brief Erstellt eine Edge mit angegebener ID, Eigenschaften und zugehörigen Knoten.
     * @param id Die ID des Weges zu dem die Edge gehört.
     * @param forward Die Richtung der Kante in Bezug auf den zugehörigen Weg.
     * @param startNode Startknoten der Edge.
     * @param endNode Endknoten der Edge.
     * @param propList Die zugehörigen Eigenschaften der Edge.
     */
    OSMEdge(boost::uint64_t id, bool forward, boost::uint64_t startNode, boost::uint64_t endNode, QVector<OSMProperty> propList) : id(id), forward(forward), startNodeID(startNode), endNodeID(endNode), properties(propList) {};
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
     * @brief Gibt die Richtung der Edge in Bezug auf den zugehörigen Weg.
     * @return Die Richtung der Edge in Bezug auf den zugehörigen Weg.
     */
    bool getForward() const {return forward;}
    /**
     * @brief Setzt die Richtung der Edge in Bezug auf den zugehörigen Weg.
     * @param forward Die Richtung der Edge in Bezug auf den zugehörigen Weg..
     */
    void setForward(const bool forward) {this->forward = forward;}
    /**
     * @brief Setzt die Knoten der Edge.
     * @param startNode Startknoten der Edge.
     * @param endNode Endknoten der Edge.
     */
    void setNodes(const boost::uint64_t startNode, const boost::uint64_t endNode) {this->startNodeID = startNode; this->endNodeID = endNode;}
    
    /**
     * @brief Setzt den Startknoten der OSMEdge auf einen Wert.
     * @param startNodeID Die ID des Startknotens
     */
    void setStartNodeID(boost::uint64_t startNodeID) {this->startNodeID = startNodeID;}
    /**
     * @brief Setzt den Endknoten der OSMEdge auf einen Wert.
     * @param startNodeID Die ID des Endknotens
     */
    void setEndNodeID(boost::uint64_t endNodeID) {this->endNodeID = endNodeID;}
    
    /**
     * @brief Gibt die ID des zugehörigen Startknoten zurück.
     * @return Startknoten.
     */
    boost::uint64_t getStartNodeID() const {return startNodeID;}
    /**
     * @brief Gibt die ID des zugehörigen Endknoten zurück.
     * @return Endknoten.
     */
    boost::uint64_t getEndNodeID() const {return endNodeID;}
    /**
     * @brief Gibt in einer Liste die Zugehörigen Knoten in der Reichenfolge Startknoten, Endknoten zurück..
     * @return Liste der Knoten (Reihenfolde: Startknoten, Endknoten).
     */
    QVector<boost::uint64_t> getNodes() const {QVector<boost::uint64_t> nodes; return nodes << startNodeID << endNodeID;}
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

    /**
     * @brief Gibt zurück, ob es sich um eine Kante mit Einbahneigenschaften handelt.
     *
     * Überprüft beispielsweise, ob die Kante Eigenschaften hat wie "oneway=yes"
     * oder "highway=motorway", die beinhalten dass es sich um eine Einbahnstraße
     * handelt.
     *
     * @remarks Dazu zählen auch Autobahnen!
     * @return <code>1</code>, wenn es sich um eine Einbahnstraße handelt, <code>0</code>
     *      wenn nicht. <code>-1</code>, wenn die Einbahnstraße in umgekehrter Richtung
     *      gültig ist.
     * @todo Evtl. weitere Wegtypen hinzufügen.
     */
    int isOneway();

    /**
     * @brief Gibt an, ob die Straße für Fahrräder eine Einbahnstraße ist,
     *      oder nicht.
     *
     * Einige Straßen sind zwar Einbahnstraßen, jedoch für Fahrräder in der
     * entgegengesetzten Richtung freigegeben.
     *
     * @return <code>1</code>, wenn es sich um eine Einbahnstraße für Fahrräder
     *      handelt, <code>0</code>
     *      wenn nicht. <code>-1</code>, wenn die Einbahnstraße in umgekehrter Richtung
     *      gültig ist.
     * @todo
     */
    int isOneWayForBikes();
    
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
