#ifndef OSMWAY_HPP
#define OSMWAY_HPP

#include "osmproperty.hpp"
#include "osmedge.hpp"
#include <QVector>
#include <boost/cstdint.hpp>
#include "tests.hpp"

/**
 * @brief Ein OSMWay stellt einen Way im OSM-Datenmodell im Speicher dar.
 * 
 * Ein OSMWay wird beim Einlesen der OSM-Daten erstellt und wird temporär dafür
 * verwendet, den Routinggraphen zu erstellen.
 * 
 * @ingroup dataprimitives
 * @author Lena Brueder
 * @date 2011-11-01
 * @copyright GNU GPL v3
 */
class OSMWay
{
private:
    boost::uint64_t id;
    QVector<boost::uint64_t> memberIDList;
    QVector<OSMProperty> properties;
    /*
    static QVector<OSMProperty> onewayProperties;
    static QVector<OSMProperty> reverseOnewayProperties;
    static QVector<OSMProperty> noOnewayProperties;
    */
    static QVector<OSMProperty> onewayProperties();
    static QVector<OSMProperty> noOnewayProperties();
    static QVector<OSMProperty> reverseOnewayProperties();
    static QVector<OSMProperty> bikeOnewayProperties();
    static QVector<OSMProperty> bikeReverseOnewayProperties();
    static QVector<OSMProperty> noBikeOnewayProperties();
public:
    
    /**
     * @brief Erstellt einen Way mit Standardeigenschaften (keine) und ohne zugehörige Knoten.
     */
    OSMWay() : id(0) {};
    /**
     * @brief Erstellt einen Way mit angegebener ID, Standardeigenschaften (keine) und ohne zugehörige Knoten.
     * @param id Die ID des Ways.
     */
    OSMWay(boost::uint64_t id) : id(id) {};
    /**
     * @brief Erstellt einen Way mit angegebener ID und Eigenschaften, ohne zugehörige Knoten.
     * @param id Die ID.
     * @param propList Die zugehörigen Eigenschaften des Ways.
     */
    OSMWay(boost::uint64_t id, QVector<OSMProperty> propList) : id(id), properties(propList) {};
    /**
     * @brief Erstellt einen Way mit angegebener ID, Eigenschaften und zugehörigen Knoten.
     * @param id Die ID.
     * @param memberIDList Die Liste der zugehörigen Knoten.
     * @param propList Die zugehörigen Eigenschaften des Ways.
     */
    OSMWay(boost::uint64_t id, QVector<boost::uint64_t> memberIDList, QVector<OSMProperty> propList) : id(id), memberIDList(memberIDList), properties(propList) {};
    /**
     * @brief Gibt die ID des Ways zurück.
     * @return Die ID.
     */
    boost::uint64_t getID() const {return id;}
    /**
     * @brief Setzt die ID des Ways.
     * @param id Die ID des Ways.
     */
    void setID(const boost::uint64_t id) {this->id = id;}
    /**
     * @brief Gibt die Liste der zugehörigen Knoten zurück.
     * @return Die Liste der zugehörigen Knoten.
     * @remarks Die Liste der Knoten ist geordnet und hat Einfluss auf die Interpretation der Eigenschaften (wie z.B. Einbahnstraßen).
     */
    QVector<boost::uint64_t> getMemberList() const {return memberIDList;}
    /**
     * @brief Fügt einen Knoten zur Liste der zugehörigen Knoten hinzu.
     * @param nodeID Die ID des Knotens.
     * @remarks Die Liste der Knoten ist geordnet und hat Einfluss auf die Interpretation der Eigenschaften (wie z.B. Einbahnstraßen).
     */
    void addMember(const boost::uint64_t nodeID) {memberIDList << nodeID;}
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
     * @brief Erstellt aus dem Weg die zugekörigen Kanten als OSMEdge.
     * @return Die Liste aller Kanten des Weges.
     */
    QVector<OSMEdge> getEdgeList();
    
    /**
     * @brief Gibt zurück, ob es sich um einen Weg mit Einbahneigenschaften handelt.
     * 
     * Überprüft beispielsweise, ob der Weg Eigenschaften hat wie "oneway=yes"
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
    int isOnewayForBikes();
};

namespace biker_tests
{
    /**
     * @ingroup tests
     */
    int testOSMWay();
} 

#endif // OSMWAY_HPP
