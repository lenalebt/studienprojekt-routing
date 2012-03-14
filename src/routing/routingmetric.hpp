#ifndef ROUTINGMETRIC_HPP
#define ROUTINGMETRIC_HPP

#include "routingnode.hpp"
#include "routingedge.hpp"
#include "altitudeprovider.hpp"
#include "potentialfunction.hpp"
#include <QVector>
#include <boost/shared_ptr.hpp>

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
    VIRTUAL,
    
    /**
     * @brief Gibt an, dass als grobes Maß Entfernungen verwendet werden.
     * 
     * Dabei ist festgelegt, dass der Wert einer Kante niemals den Wert der
     * Entfernung der beiden Endpunkte in Metern unterschreitet.
     */
    DISTANCE
};

/**
 * @brief Eine RoutingMetric ist eine Metrik zum Bewerten von Kanten des Graphen.
 * 
 * Eine RoutingMetric kann Kanten so bewerten, wie sie es für richtig hält - 
 * einzige Voraussetzung ist, dass sie nur positive Werte für die Kosten zurückgibt.
 * 
 * @ingroup routing
 * @author Thorsten Scheller
 * @date 2012-1-23
 * @copyright GNU GPL v3
 * @todo Fehlt: Beim Erzeugen einer Instanz soll immer gleich ein
 *      AltitudeProvider mit übergeben werden können. Fabrikmethoden
 *      verwenden?
 */
class RoutingMetric
{
protected:
    QVector<boost::shared_ptr<PotentialFunction> > _potFuncList;
    boost::shared_ptr<AltitudeProvider> _altitudeProvider;
public:
    RoutingMetric() : _potFuncList(), _altitudeProvider() {}
    RoutingMetric(boost::shared_ptr<AltitudeProvider> provider) : _potFuncList(), _altitudeProvider(provider) {}
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
    * in der Kante gespeichert ist, weil so der Zugriff auf deren
    * Längen- und Breitengrad einfacher und schneller ist.
    * @attention Die Funktion darf keine negativen Werte zurückgeben!
    * @return Die Bewertung der angegebenen Kante.
    */
    virtual double rateEdge(const RoutingEdge& edge, const RoutingNode& startNode, const RoutingNode& endNode)=0;
    /**
     * @brief Fügt eine Potentialfunktion in die Bewertung ein.
     * 
     * In der Standardimplementierung wird die PotentialFunction in einer
     * intern verwalteten Liste abgelegt.
     * 
     * @param potentialFunction Ein boost::shared_ptr auf eine beliebige Potentialfunktion.
     */
    virtual void addPotentialFunction(boost::shared_ptr<PotentialFunction> potentialFunction) 
    {
        _potFuncList << potentialFunction;
    }
    /**
     * @brief Gibt an, in welcher Einheit die Bewertung von rateEdge() angegeben wird.
     * 
     * Standardmäßig gibt diese Funktion DISTANCE zurück.
     * 
     * @return In welcher Einheit die Bewertung von rateEdge() angegeben wird.
     * @see MeasurementUnit
     */
    virtual MeasurementUnit getMeasurementUnit() 
    {
        return DISTANCE;
    }
    
    /**
    * @brief Gibt zurück, wie lang man benötigt um diese Kante zu befahren.
    *
    * Die Einheit ist Sekunden. Die Funktion wird verwendet, um dem Benutzer
    * eine Rückmeldung zu geben über die Zeit, die benötigt wird um eine
    * Route zu befahren.
    *
    * @remarks Wenn in rateEdge() Sekunden zurückgegeben werden, ist diese
    * Funktion identisch zu rateEdge().
    * @param edge Die Kante, die bewertet werden soll.
    * @param startNode Der Startknoten der Kante.
    * @param endNode Der Endknoten der Kante.
    * @remarks Es werden Start- und Endknoten angegeben, obwohl deren ID
    * in der Kante gespeichert ist, weil so der Zugriff auf deren
    * Längen- und Breitengrad einfacher und schneller ist.
    * @attention Die Funktion darf keine negativen Werte zurückgeben!
    * @return Die Zeit, de man benötigt um die angegebene Kante zu befahren.
    */
    virtual double timeEdge(const RoutingEdge& edge, const RoutingNode& startNode, const RoutingNode& endNode)=0;

    virtual ~RoutingMetric();
    
    /**
     * @brief Macht eine Vorhersage über die zu erwartenden Kosten, um eine bestimmte
     *      Entfernung zurückzulegen.
     * 
     * Die Schätzung muss in der Einheit erfolgen, die getMeasurementUnit()
     * zurückgibt - dadurch wird es ermöglicht, A* einzusetzen auch in den
     * Fällen, wo nicht die pure Entfernung als Metrik für den Algorithmus
     * verwendet wird.
     * 
     * Es ist eine Standardimplementierung vorhanden, die die Entfernung
     * von p1 zu p2 zurückgibt - kompatibel mit der Einheit DISTANCE.
     * 
     * @remarks Diese Funktion darf die Kosten niemals überschätzen.
     * @return Die geschätzten Kosten von p1 zu p2.
     */
    virtual double estimateDistance(const GPSPosition& p1, const GPSPosition& p2)
    {
        return p1.calcDistance(p2);
    }
}; 

class EuclidianRoutingMetric : public RoutingMetric
{
private:
    
public:
    EuclidianRoutingMetric() {}
    EuclidianRoutingMetric(boost::shared_ptr<AltitudeProvider> provider) : RoutingMetric(provider) {}
    double rateEdge(const RoutingEdge& edge, const RoutingNode& startNode, const RoutingNode& endNode);
    double timeEdge(const RoutingEdge& edge, const RoutingNode& startNode, const RoutingNode& endNode);
};

class SimpleHeightRoutingMetric : public RoutingMetric
{
private:
    float _detourPerHeightMeter;
public:
    SimpleHeightRoutingMetric() : _detourPerHeightMeter(50.0) {}
    SimpleHeightRoutingMetric(float detourPerHeightMeter) : _detourPerHeightMeter(detourPerHeightMeter) {}
    SimpleHeightRoutingMetric(boost::shared_ptr<AltitudeProvider> provider, float detourPerHeightMeter) : RoutingMetric(provider), _detourPerHeightMeter(detourPerHeightMeter) {}
    double rateEdge(const RoutingEdge& edge, const RoutingNode& startNode, const RoutingNode& endNode);
    double timeEdge(const RoutingEdge& edge, const RoutingNode& startNode, const RoutingNode& endNode);
};

class SimplePowerRoutingMetric : public RoutingMetric
{
private:
    double weight;
    double efficiency;
    /**
     * @brief Diese Funktion berrechnet die Leistung mittels Distanz und Zeit
     * @param distance die Entfernung in Metern
     * @param time die Zeit in Sekunden
     * @param hightDifference der Höhenunterschied in Metern
     * @param weight das Gesammtgewicht des Systems Fahrrad-Fahrer
     * @return die Leistung.
     */
    double calculateDistanceTime(double distance, double time, double hight, double weight)
    {
        // Am Steigungs-Dreieck gilt der Satz von Pythagoras: s^2 = x^2 + h^2
        double fullDistance = sqrt( pow(distance, 2) + pow(hight, 2));
        // Die Geschwindigkeit in m/s ist definiert als gefahrene Strecke pro Zeit
        double speed = fullDistance / time;
        //
        double hightDifference = hight/distance;
        // Die Leistung ist Arbeit pro Zeit: P = E / t & Die zu verrichtende Arbeit errechnet sich aus der Höhendifferenz und dem Gesamtgewicht von Fahrer und Fahrrad: E = G * 9.81 ms^-2 * h mit G = Gesamtgewicht von Fahrer und Fahrrad in kg
        double efficiency = weight * pow(9.81, -2.0) * hightDifference * speed / sqrt(1 + pow(hightDifference, 2.0));
        return efficiency;
    }
    /**
     * @brief Diese Funktion berrechnet die Leistung mittels der Geschwindigkeit
     * @param speed die Geschwindigkeit in Metern / Sekunde
     * @param hightDifference der Höhenunterschied in Metern / Distanz in Metern
     * @param weight das Gesammtgewicht des Systems Fahrrad-Fahrer
     * @return die Leistung.
     */
    double calculateSpeed(double speed, double hightDifference, double weight)
    {
        //
        double efficiency = weight * pow(9.81, -2.0) * hightDifference * speed / sqrt(1 + pow(hightDifference, 2.0));
        return efficiency;
    }
    /**
     * @brief Diese Funktion berrechnet die Leistung mittels Distanz und Zeit
     * @param efficency die Leistung
     * @param hightDifference der Höhenunterschied in Metern
     * @param weight das Gesammtgewicht des Systems Fahrrad-Fahrer
     * @return die Zeit.
     */
    double calculate(double efficiency, double hightDifference, double weight, double distance)
    {
        //
        double time = distance /(efficiency * sqrt(1 + pow(hightDifference, 2.0)) / (weight * pow(9.81, -2.0) * hightDifference));
        return time;
    }
public:
    SimplePowerRoutingMetric(boost::shared_ptr<AltitudeProvider> provider) :
        RoutingMetric(provider)
    {
        weight = 85.0;
        efficiency = 3.0 * weight;
    }
    SimplePowerRoutingMetric (boost::shared_ptr<AltitudeProvider> provider, double newWeight, double newEfficiency) :
        RoutingMetric(provider), weight(newWeight), efficiency(newEfficiency)
    {}

    double rateEdge(const RoutingEdge& edge, const RoutingNode& startNode, const RoutingNode& endNode)
    {
        double hightStart = _altitudeProvider->getAltitude(startNode);
        double hightEnd = _altitudeProvider->getAltitude(endNode);
        double hightDifference = hightStart-hightEnd;
        if (hightDifference < 0.0)
        {
            hightDifference = hightDifference * (-1.0);
        }
        double distance = startNode.calcDistance(endNode);
        double result = calculate(efficiency, hightDifference, weight, distance);
        return result;
    }
    /**
     * @todo IMPLEMENTIEREN!
     */
    double timeEdge(const RoutingEdge& edge, const RoutingNode& startNode, const RoutingNode& endNode) {return 0.0;}
 };

#endif //ROUTINGMETRIC_HPP
