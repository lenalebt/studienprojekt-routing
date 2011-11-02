#ifndef POTENTIALFUNCTION_HPP
#define POTENTIALFUNCTION_HPP

/**
 * @brief Eine PotentialFunction stellt eine Potentialfunktion dar.
 * 
 * Eine Potentialfunktion ist eine Funktion, die einem Punkt der Ebene
 * einen Wert zuweist. In diesem Fall beschreibt dieser Wert, wie stark
 * ein bestimmter Punkt gemieden werden soll. Da die Potentialfunktion nur
 * positive Werte für eine Bewertung zurückgeben darf werden unbekannte Punkte
 * immer bestraft, und schöne/gute Punkte einfach mit weniger Bestrafung
 * versehen.
 * 
 * @author Lena Brueder
 * @date 2011-11-02
 * @copyright GNU GPL v3
 * @todo Implementierung und Definition von Funktionen fehlt. Evtl Werte normieren? (immer auf 100?)
 * @todo Evtl ein operator() für den Aufruf von ratePosition verwenden?
 */
class PotentialFunction
{
private:
    
public:
    /**
     * @brief Bewertet eine Position.
     * 
     * Dabei wird die Position über ihren Längen- und Breitengrad angegeben.
     * 
     * @param lon Der Längengrad.
     * @param lat Der Breitengrad.
     * @attention Diese Funktion darf nur positive Werte zurückgeben!
     * @return Die Bewertung des Punktes.
     */
    virtual double ratePosition(const double lon, const double lat)=0;
    
    /**
     * @brief Bewertet eine Position.
     * 
     * Dabei wird die Position über ihre GPSPosition angegeben.
     * 
     * @param pos Die Position des Punktes.
     * @attention Diese Funktion darf nur positive Werte zurückgeben!
     * @return Die Bewertung des Punktes.
     */
    virtual double ratePosition(const GPSPosition& pos)=0;
}; 

#endif //POTENTIALFUNCTION_HPP
