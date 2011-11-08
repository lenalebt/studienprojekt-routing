#ifndef ROUTINGMETRIC_HPP
#define ROUTINGMETRIC_HPP

#include "routingnode.hpp"
#include "routingedge.hpp"
#include "altitudeprovider.hpp"
#include <QList>

/**
 * @brief Gibt an, in welcher Einheit eine RoutingMetric die übergebenen
 *      Kanten bewertet.
 * @ingroup routing
 */
enum MeasurementUnit
{
    /**
     * @brief Gibt an, dass Sekunden als Maßeinheit verwendet werden.
     */
    SECONDS,
    
    /**
     * @brief Gibt an, dass keine festgelegte Maßeinheit verwendet wird.
     */
    VIRTUAL
};

/**
 * @brief Eine RoutingMetric ist eine Metrik zum Bewerten von Kanten des Graphen.
 * 
 * Eine RoutingMetric kann Kanten so bewerten, wie sie es für richtig hält - 
 * einzige Voraussetzung ist, dass sie nur positive Werte für die Kosten zurückgibt.
 * 
 * @ingroup routing
 * @author Lena Brueder
 * @date 2011-11-02
 * @copyright GNU GPL v3
 * @todo Fehlt: Beim Erzeugen einer Instanz soll immer gleich ein
 *      AltitudeProvider mit übergeben werden können. Fabrikmethoden
 *      verwenden?
 */
class RoutingMetric
{
protected:
    QList<PotentialFunction> potFuncList;
    AltitudeProvider* altitudeProvider;
public:
    /**
     * @brief Bewertet eine Kante nach bestimmten Kriterien.
     * 
     * Die Kriterien, nach denen bewertet wird, hängen von der Implementierung der
     * Klasse ab. Die Funktion kann entweder Zeiten in Sekunden, oder eine
     * fiktive Bewertungseinheit zurückgeben. Welche Einheit der zurückgegebene Wert hat
     * wird in der Funktion
     * 
     * @param edge Die Kante, die bewertet werden soll.
     * @param startNode Der Startknoten der Kante.
     * @param endNode Der Endknoten der Kante.
     * @remarks Es werden Start- und Endknoten angegeben, obwohl deren ID
     *      in der Kante gespeichert ist, weil so der Zugriff auf deren
     *      Längen- und Breitengrad einfacher und schneller ist.
     * @attention Die Funktion darf keine negativen Werte zurückgeben!
     * @return Die Bewertung der angegebenen Kante.
     */
    virtual double rateEdge(const RoutingEdge& edge, const RoutingNode& startNode, const RoutingNode& endNode)=0;
    /**
     * @brief Fügt eine Potentialfunktion in die Bewertung ein.
     * 
     * In der Standardimplementierung wird die PotentialFunction in einer
     * intern verwaltete Liste abgelegt.
     * 
     * @param potentialFunction Eine beliebige Potentialfunktion.
     */
    virtual void addPotentialFunction(const PotentialFunction& potentialFunction) {potFuncList << potentialFunction;}
    /**
     * @brief Gibt an, in welcher Einheit die Bewertung von rateEdge() angegeben wird.
     * 
     * Standardmäßig gibt diese Funktion VIRTUAL zurück.
     * 
     * @return In welcher Einheit die Bewertung von rateEdge() angegeben wird.
     * @see MeasurementUnit
     */
    virtual MeasurementUnit getMeasurementUnit() {return VIRTUAL;}
    
    /**
     * @brief Gibt zurück, wie lang man benötigt um diese Kante zu befahren.
     * 
     * Die Einheit ist Sekunden. Die Funktion wird verwendet, um dem Benutzer
     * eine Rückmeldung zu geben über die Zeit, die benötigt wird um eine
     * Route zu befahren.
     * 
     * @remarks Wenn in rateEdge() Sekunden zurückgegeben werden, ist diese
     *      Funktion identisch zu rateEdge().
     * @param edge Die Kante, die bewertet werden soll.
     * @param startNode Der Startknoten der Kante.
     * @param endNode Der Endknoten der Kante.
     * @remarks Es werden Start- und Endknoten angegeben, obwohl deren ID
     *      in der Kante gespeichert ist, weil so der Zugriff auf deren
     *      Längen- und Breitengrad einfacher und schneller ist.
     * @attention Die Funktion darf keine negativen Werte zurückgeben!
     * @return Die Zeit, de man benötigt um die angegebene Kante zu befahren.
     */
    virtual double timeEdge(const RoutingEdge& edge, const RoutingNode& startNode, const RoutingNode& endNode)=0;
    
    virtual ~RoutingMetric();
}; 

#endif //ROUTINGMETRIC_HPP
