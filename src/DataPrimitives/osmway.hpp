#ifndef OSMWAY_HPP
#define OSMWAY_HPP

#include "osmproperty.hpp"
#include <QList>

/**
 * @brief Ein OSMWay stellt einen Way im OSM-Datenmodell im Speicher dar.
 * 
 * Ein OSMWay wird beim Einlesen der OSM-Daten erstellt und wird temporär dafür
 * verwendet, den Routinggraphen zu erstellen.
 * 
 * @author Lena Brueder
 * @date 2011-11-01
 * @copyright GNU GPL v3
 */
class OSMWay
{
private:
    uint64_t id;
    QList<uint64_t> memberIDList;
    QList<OSMProperty> properties;
public:
    /**
     * @brief Erstellt einen Way mit angegebener ID, Standardeigenschaften (keine) und ohne zugehörige Knoten.
     * @param id Die ID des Ways.
     */
    OSMWay(uint64_t id) : id(id) {};
    /**
     * @brief Erstellt einen Way mit angegebener ID und Eigenschaften, ohne zugehörige Knoten.
     * @param id Die ID.
     * @param propList Die zugehörigen Eigenschaften des Ways.
     */
    OSMWay(uint64_t id, QList<OSMProperty> propList) : id(id), properties(propList) {};
    /**
     * @brief Erstellt einen Way mit angegebener ID, Eigenschaften und zugehörigen Knoten.
     * @param id Die ID.
     * @param memberIDList Die Liste der zugehörigen Knoten.
     * @param propList Die zugehörigen Eigenschaften des Ways.
     */
    OSMWay(uint64_t id, QList<uint64_t> memberIDList, QList<OSMProperty> propList) : id(id), memberIDList(memberIDList), properties(propList) {};
    /**
     * @brief Gibt die ID des Ways zurück.
     * @return Die ID.
     */
    uint64_t getID() const {return id;}
    /**
     * @brief Setzt die ID des Ways.
     * @param id Die ID des Ways.
     */
    void setID(uint64_t id) {this->id = id;}
    /**
     * @brief Gibt die Liste der zugehörigen Knoten zurück.
     * @return Die Liste der zugehörigen Knoten.
     * @remarks Die Liste der Knoten ist geordnet und hat Einfluss auf die Interpretation der Eigenschaften (wie z.B. Einbahnstraßen).
     */
    QList<uint64_t> getMemberList() const {return memberIDList;}
    /**
     * @brief Fügt einen Knoten zur Liste der zugehörigen Knoten hinzu.
     * @param nodeID Die ID des Knotens.
     * @remarks Die Liste der Knoten ist geordnet und hat Einfluss auf die Interpretation der Eigenschaften (wie z.B. Einbahnstraßen).
     */
    void addMember(uint64_t nodeID) {memberIDList << nodeID;}
    /**
     * @brief Fügt eine Eigenschaft zur Liste der Eigenschaften hinzu.
     * @param prop Die Eigenschaft, die hinzugefügt werden soll.
     * @remarks Die Liste der Eigenschaften ist nicht notwendigerweise geordnet.
     */
    void addProperty(OSMProperty prop) {properties << prop;}
    /**
     * @brief Gibt die Liste der zugehörigen Eigenschaften zurück.
     * @return Die Liste der zugehörigen Eigenschaften.
     * @remarks Die Liste der Eigenschaften ist nicht notwendigerweise geordnet.
     */
    QList<OSMProperty> getProperties() const {return properties;}
};

#endif // OSMWAY_HPP
