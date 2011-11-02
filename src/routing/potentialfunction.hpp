#ifndef POTENTIALFUNCTION_HPP
#define POTENTIALFUNCTION_HPP

/**
 * @brief Eine PotentialFunction stellt eine Potentialfunktion dar.
 * 
 * Eine Potentialfunktion ist eine Funktion, die einem Punkt der Ebene
 * einen Wert zuweist. In diesem Fall beschreibt dieser Wert, wie stark
 * ein bestimmter Punkt gemieden werden soll.
 * 
 * @author Lena Brueder
 * @date 2011-11-02
 * @copyright GNU GPL v3
 * @todo Implementierung und Definition von Funktionen fehlt. Evtl Werte normieren? (immer auf 100?)
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
     * @return Die Bewertung des Punktes.
     */
    virtual double ratePosition(const double lon, const double lat)=0;
    
    /**
     * @brief Bewertet eine Position.
     * 
     * Dabei wird die Position über ihre GPSPosition angegeben.
     * 
     * @param pos Die Position des Punktes.
     * @return Die Bewertung des Punktes.
     */
    virtual double ratePosition(const GPSPosition& pos)=0;
}; 

#endif //POTENTIALFUNCTION_HPP
