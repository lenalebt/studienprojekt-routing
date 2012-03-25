#ifndef ROUTINGMETRIC_HPP
#define ROUTINGMETRIC_HPP

#include "routingnode.hpp"
#include "routingedge.hpp"
#include "altitudeprovider.hpp"
#include "potentialfunction.hpp"
#include <QVector>
#include <boost/shared_ptr.hpp>
#include <limits>

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
 * Wer eine eigene Routingmetrik implementieren möchte, muss von
 * RoutingMetric erben, und mindestens die Funktionen rateEdge() und timeEdge()
 * implementieren. Weitere Informationen finden sich dort.
 * 
 * @ingroup routing
 * @author Thorsten Scheller
 * @date 2012-1-23
 * @copyright GNU GPL v3
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
    * wird in der Funktion getMeasurementUnit() angegeben.
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
     * @remarks Potentialfunktionen werden noch nicht unterstützt in der aktuellen Version.
     * @param potentialFunction Ein boost::shared_ptr auf eine beliebige Potentialfunktion.
     */
    virtual void addPotentialFunction(boost::shared_ptr<PotentialFunction> potentialFunction) 
    {
        _potFuncList << potentialFunction;
    }
    /**
     * @brief Gibt an, in welcher Einheit die Bewertung von rateEdge() angegeben wird.
     * 
     * Diese Funktion bestimmt, welcher Routingalgorithmus standardmäßig
     * ausgewählt wird. Für DISTANCE wird eine A*-Version, für SECONDS wird
     * eine Dijkstra-Version gewählt.
     * 
     * Standardmäßig gibt diese Funktion DISTANCE zurück. Damit beziehen sich die Werte
     * der Metrik auf reale Entfernungen, und die Werte sind mindestens so groß wie
     * die euklidische Distanz der beiden Endpunkte.
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
    * Funktion im Idealfall identisch zu rateEdge(). Sie muss es allerdings nicht sein,
    * wenn die Bewertung der Route ungefähr Sekunden sind, sie aber durch andere Faktoren wie
    * "grüne Strecken" etc. verändert wurde.
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
        if (getMeasurementUnit() == DISTANCE)
            return p1.calcDistance(p2);
        else if (getMeasurementUnit() == SECONDS)
            return p1.calcDistance(p2) * 2;   //in m/s, schieben mit etwa 2km/h
        else
            return p1.calcDistance(p2);
    }
    
    virtual QString getParameterDetails() {return "none";}
}; 

/**
 * @brief Diese Klasse implementiert eine einfache Routingmetrik, die nur die
 *      euklidische Distanz der Endpunkte als Bewertung heranzieht.
 * 
 * 
 * 
 * @ingroup routing
 * @author Lena Brueder
 * @date 2012-03-25
 * @copyright GNU GPL v3
 */
class EuclideanRoutingMetric : public RoutingMetric
{
private:
    
public:
    EuclideanRoutingMetric() {}
    EuclideanRoutingMetric(boost::shared_ptr<AltitudeProvider> provider) : RoutingMetric(provider) {}
    double rateEdge(const RoutingEdge& edge, const RoutingNode& startNode, const RoutingNode& endNode);
    double timeEdge(const RoutingEdge& edge, const RoutingNode& startNode, const RoutingNode& endNode);
    QString getParameterDetails() {return "euclidean";}
};

/**
 * @brief Diese Klasse implementiert eine Routingmetrik, die nur die
 *      euklidische Distanz der Endpunkte und den Höhenunterschied dieser
 *      als Bewertung heranzieht.
 * 
 * Jeder Höhenmeter wird mit einem Umweg bestraft, wie er in
 * <code>detourPerHeightMeter</code> angegeben wird (Angaben in Metern).
 * <code>100.0</code> ist ein guter Praxiswert
 * 
 * @ingroup routing
 * @author Lena Brueder
 * @date 2012-03-25
 * @copyright GNU GPL v3
 */
class SimpleHeightRoutingMetric : public RoutingMetric
{
private:
    float _detourPerHeightMeter;
public:
    //SimpleHeightRoutingMetric() : _detourPerHeightMeter(50.0) {}
    //SimpleHeightRoutingMetric(float detourPerHeightMeter) : _detourPerHeightMeter(detourPerHeightMeter) {}
    SimpleHeightRoutingMetric(boost::shared_ptr<AltitudeProvider> provider, float detourPerHeightMeter) : RoutingMetric(provider), _detourPerHeightMeter(detourPerHeightMeter) {}
    double rateEdge(const RoutingEdge& edge, const RoutingNode& startNode, const RoutingNode& endNode);
    double timeEdge(const RoutingEdge& edge, const RoutingNode& startNode, const RoutingNode& endNode);
    QString getParameterDetails() {return QString("simpleheight,detour=%1").arg(_detourPerHeightMeter);}
};

class AdvancedHeightRoutingMetric : public RoutingMetric
{
private:
    float _extrapunishment;
    float _detourPerHeightMeter;
public:
    //SimpleHeightRoutingMetric() : _detourPerHeightMeter(50.0) {}
    //SimpleHeightRoutingMetric(float detourPerHeightMeter) : _detourPerHeightMeter(detourPerHeightMeter) {}
    AdvancedHeightRoutingMetric(boost::shared_ptr<AltitudeProvider> provider, float detourPerHeightMeter, float extrapunishment) : RoutingMetric(provider), _extrapunishment(extrapunishment), _detourPerHeightMeter(detourPerHeightMeter) {}
    double rateEdge(const RoutingEdge& edge, const RoutingNode& startNode, const RoutingNode& endNode);
    double timeEdge(const RoutingEdge& edge, const RoutingNode& startNode, const RoutingNode& endNode);
    QString getParameterDetails() {return QString("advancedheight,detour=%1,extrapunishment=%2").arg(_detourPerHeightMeter).arg(_extrapunishment);}
};

/**
 * @brief Diese Klasse berechnet die Metrik aufgrund der Leistung, die benötigt
 *      wird, um die Kante zu bewältigen.
 * 
 * Es wird der Untergrund und die Steigung
 * einbezogen. Der Straßentyp hat auch Einfluss.
 * Nähere Erklärungen sind auch unter \ref routingmetric_power
 * zu finden.
 * 
 * @ingroup routing
 * @author Lena Brueder
 * @date 2012-03-25
 * @copyright GNU GPL v3
 */
class PowerRoutingMetric : public RoutingMetric
{
private:
    double maxPower;     //maximale Gesamtleistung des Systems
    double weight;       //Gesamtgewicht des Systems
    double minSpeed;     //gewünschte Minimalgeschwindigkeit des Systems
    double haltungskorrekturfaktor;  //Bestimmt, wie aufrecht man sitzt auf dem Rad, realistische Werte:    Oberlenker: 0,5    Bremsgriff: 0.4    Unterlenker: 0.3    Triathlon: 0.25 
    double pushBikeSpeed;    //Geschwindigkeit, wenn das Rad geschoben wird
    double maxSpeed;     //Höchstgeschwindigkeit, errechnet sich aus der zur Verfügung stehenden Leistung
    
    double*** powerarray;
    double*** speedarray;
    
    inline double calcInclinationPower(double speed, double inclination, double weight)
    {
        return weight * sin(atan(inclination)) * 9.81 * speed;
        //return (weight * heightDifference * 9.81f) / time;
    }
    inline double calcAerodynamicResistancePower(double speed, double haltungskorrekturfaktor)
    {
        //0.5 * Anpassung * Höhen/Druckkorrektur(20°/150m))
        return 0.5f * 1.311f  * 0.9f * haltungskorrekturfaktor * (speed*speed*speed);
    }
    /**
     * @brief Berechnet die Rollwiderstandsleistung auf dem Boden.
     * @param speed Die Geschwindigkeit über dem Boden
     * @param surfaceFactor Der Faktor für den Bodenwiderstand. Standardwert: 1 (normaler Asphalt). Schotterweg: 1.8. Guter Schotterweg: 1.47. Rauer Asphalt: 1.15, glatter Asphalt: 0.82
     * @return Den Rollwiderstand
     */
    inline double calcRollingResistancePower(double speed, double surfaceFactor, double weight)
    {
        return 0.008f * surfaceFactor * weight * 9.81f * speed;
    }
    
    inline double min(double a, double b) {return (a<b) ? a : b;}
    inline double max(double a, double b) {return (a<b) ? b : a;}
    
    double getPower(double speed, double inclination, double surfaceFactor, double haltungskorrekturfaktor, double weight);
    double getSpeed(double power, double inclination, double surfaceFactor, double haltungskorrekturfaktor, double weight);
    void init();
    void getCalculationValues(const RoutingEdge& edge, const RoutingNode& startNode, const RoutingNode& endNode, double& heightDifference, double& distance, double& inclination, double& surfaceFactor, double& streetTypeFactor, double& timePunishment, double& speed, double& power)
    {
        heightDifference = _altitudeProvider->getAltitude(endNode) - _altitudeProvider->getAltitude(startNode);
        if (heightDifference < 0)
            heightDifference = 0;
        distance = startNode.calcDistance(endNode);
        inclination = (distance==0) ? 0.0 : heightDifference / distance;
        
        /*std::cerr << std::endl << edge << startNode << endNode << std::endl;
        
        std::cerr << "inclination: " << inclination << std::endl;
        std::cerr << "distance: " << distance << std::endl;
        std::cerr << "heightDifference: " << heightDifference << std::endl;*/
        
        //TODO: Faktor anpassen je nach Eigenschaften der Kante
        surfaceFactor = 1;
        streetTypeFactor = 1;
        timePunishment = 0;
        switch (edge.getAccess())
        {
            case ACCESS_DESTINATION:
            case ACCESS_PERMISSIVE:
            case ACCESS_YES:
            case ACCESS_UNKNOWN:
            case ACCESS_DESIGNATED:
            case ACCESS_COMPULSORY:
                switch (edge.getStreetType())
                {
                    case STREETTYPE_HIGHWAY_FORD:           streetTypeFactor = (edge.getStreetSurfaceType()!=STREETSURFACETYPE_UNKNOWN ? 1.0 : 1.0); break;
                    case STREETTYPE_HIGHWAY_JUNCTION:       streetTypeFactor = (edge.getStreetSurfaceType()!=STREETSURFACETYPE_UNKNOWN ? 1.0 : 1.0); break;
                    case STREETTYPE_HIGHWAY_LIVINGSTREET:   streetTypeFactor = (edge.getStreetSurfaceType()!=STREETSURFACETYPE_UNKNOWN ? 1.0 : 1.0); break;
                    case STREETTYPE_HIGHWAY_PATH:           if ((edge.getCyclewayType() != CYCLEWAYTYPE_NO_CYCLEWAY) && (edge.getCyclewayType() != CYCLEWAYTYPE_UNKNOWN))
                                                            {
                                                                streetTypeFactor = (edge.getStreetSurfaceType()!=STREETSURFACETYPE_UNKNOWN ? 1.0 : 1.0); break;
                                                            }
                                                            else
                                                            {
                                                                streetTypeFactor = (edge.getStreetSurfaceType()!=STREETSURFACETYPE_UNKNOWN ? 1.5 : 2.0); break;
                                                            }
                    case STREETTYPE_HIGHWAY_PEDESTRIAN:     speed = pushBikeSpeed; return; break;
                    case STREETTYPE_HIGHWAY_PRIMARY:        streetTypeFactor = (edge.getStreetSurfaceType()!=STREETSURFACETYPE_UNKNOWN ? 1.2 : 1.2); break;
                    case STREETTYPE_HIGHWAY_RESIDENTIAL:    streetTypeFactor = (edge.getStreetSurfaceType()!=STREETSURFACETYPE_UNKNOWN ? 1.0 : 1.0); break;
                    case STREETTYPE_HIGHWAY_SECONDARY:      streetTypeFactor = (edge.getStreetSurfaceType()!=STREETSURFACETYPE_UNKNOWN ? 1.15 : 1.15); break;
                    case STREETTYPE_HIGHWAY_SERVICE:        streetTypeFactor = (edge.getStreetSurfaceType()!=STREETSURFACETYPE_UNKNOWN ? 1.0 : 1.0); break;
                    case STREETTYPE_HIGHWAY_UNCLASSIFIED:
                    case STREETTYPE_HIGHWAY_TERTIARY:       streetTypeFactor = (edge.getStreetSurfaceType()!=STREETSURFACETYPE_UNKNOWN ? 1.1 : 1.1); break;
                    case STREETTYPE_HIGHWAY_TRACK:          streetTypeFactor = (edge.getStreetSurfaceType()!=STREETSURFACETYPE_UNKNOWN ? 1.5 : 2.0); break;
                    case STREETTYPE_UNKNOWN:
                    default:                                streetTypeFactor = (edge.getStreetSurfaceType()!=STREETSURFACETYPE_UNKNOWN ? 1.8 : 2.5); break;
                }
                break;
            case ACCESS_FOOT_ONLY:          speed = pushBikeSpeed; return; break;
            default:                        streetTypeFactor = 100.0;
                                            break;
        }
        switch (edge.getCyclewayType())
        {
            case CYCLEWAYTYPE_LANE:                         streetTypeFactor *= 1.0; break;
            case CYCLEWAYTYPE_LANE_OP:                      streetTypeFactor *= 1.0; break;
            case CYCLEWAYTYPE_LANE_SEGREGAETD:              streetTypeFactor *= 1.0; break;
            case CYCLEWAYTYPE_LANE_SEGREGAETD_OP:           streetTypeFactor *= 1.0; break;
            case CYCLEWAYTYPE_LANE_SHARED_BUSWAY:           streetTypeFactor *= 1.0; break;
            case CYCLEWAYTYPE_LANE_SHARED_BUSWAY_OP:        streetTypeFactor *= 1.0; break;
            case CYCLEWAYTYPE_TRACK:                        streetTypeFactor *= 1.0; break;
            case CYCLEWAYTYPE_TRACK_SEGREGATED:             streetTypeFactor *= 1.0; break;
            case CYCLEWAYTYPE_TRACK_SHARED_BUSWAY:          streetTypeFactor *= 1.0; break;
            case CYCLEWAYTYPE_MTB_0:                        streetTypeFactor *= 1.0; break;
            case CYCLEWAYTYPE_MTB_1:                        streetTypeFactor *= 1.0; break;
            case CYCLEWAYTYPE_MTB_2:                        streetTypeFactor *= 1.0; break;
            case CYCLEWAYTYPE_MTB_3:                        streetTypeFactor *= 1.0; break;
            case CYCLEWAYTYPE_MTB_HIGH:                     streetTypeFactor *= 1.0; break;
            case CYCLEWAYTYPE_NO_CYCLEWAY:
            case CYCLEWAYTYPE_UNKNOWN:
            default:                                        streetTypeFactor *= 1.5; break;
        }
        switch (edge.getStreetSurfaceQuality())
        {
            case STREETSURFACEQUALITY_EXCELLENT:     surfaceFactor *= 0.82; break;
            case STREETSURFACEQUALITY_GOOD:          surfaceFactor *= 1.0; break;
            case STREETSURFACEQUALITY_INTERMEDIATE:  surfaceFactor *= 1.15; break;
            case STREETSURFACEQUALITY_BAD:           surfaceFactor *= 1.4; break;
            case STREETSURFACEQUALITY_VERYBAD:       surfaceFactor *= 1.8; break;
            case STREETSURFACEQUALITY_HORRIBLE:      surfaceFactor *= 2.2; break;
            case STREETSURFACEQUALITY_VERYHORRIBLE:  surfaceFactor *= 2.5; break;
            case STREETSURFACEQUALITY_IMPASSABLE:    surfaceFactor *= 5.0; break;
            case STREETSURFACEQUALITY_UNKNOWN:
            default:                                 surfaceFactor *= 1.0; break;
        }
        switch (edge.getStreetSurfaceType())
        {
            case STREETSURFACETYPE_ASPHALT:         surfaceFactor *= 1.0; break;
            case STREETSURFACETYPE_COBBLESTONE:     surfaceFactor *= 1.5; break;
            case STREETSURFACETYPE_COMPACTED:       surfaceFactor *= 1.4; break;
            case STREETSURFACETYPE_CONCRETE:        surfaceFactor *= 1.4; break;
            case STREETSURFACETYPE_FINEGRAVEL:      surfaceFactor *= 1.4; break;
            case STREETSURFACETYPE_GRASS:           surfaceFactor *= 2.0; break;
            case STREETSURFACETYPE_GRASSPAVER:      surfaceFactor *= 1.7; break;
            case STREETSURFACETYPE_GRAVEL:          surfaceFactor *= 1.0; break;
            case STREETSURFACETYPE_GROUND:          surfaceFactor *= 1.0; break;
            case STREETSURFACETYPE_METAL:           surfaceFactor *= 2.0; break;
            case STREETSURFACETYPE_PAVED:           surfaceFactor *= 1.2; break;
            case STREETSURFACETYPE_PAVING_STONES:   surfaceFactor *= 1.3; break;
            case STREETSURFACETYPE_SETT:            surfaceFactor *= 1.3; break;
            case STREETSURFACETYPE_TARTAN:          surfaceFactor *= 1.3; break;
            case STREETSURFACETYPE_UNPAVED:         surfaceFactor *= 2.0; break;
            case STREETSURFACETYPE_UNKNOWN:
            default:                                surfaceFactor *= 1.3; break;
            
        }
        switch (edge.getTurnType())
        {
            case TURNTYPE_LEFTCROSS:        timePunishment += 5.0; break;
            case TURNTYPE_RIGHTCROSS:       timePunishment += 2.0; break;
            case TURNTYPE_STRAIGHTCROSS:    timePunishment += 0.0; break;
            case TURNTYPE_UTURNCROSS:       timePunishment += 7.0; break;
            case TURNTYPE_STRAIGHT:
            default:                        timePunishment += 0.0; break;
        }
        if (edge.hasStairs())
        {
            //Bestrafung für eine Treppe: Länge * 2 in Sekunden + 5 Sekunden
            timePunishment += distance * 2.0 + 5.0;
        }
        if (edge.hasCycleBarrier())
        {
            timePunishment += 10.0;
        }
        if (edge.hasTrafficCalmingBumps())
        {
            timePunishment += 8.0;
        }
        if (edge.hasStopSign())
        {
            timePunishment += 8.0;
        }
        if (edge.hasTrafficLights())
        {
            timePunishment += 10.0;
        }
        
        power = getPower(minSpeed, inclination, surfaceFactor, haltungskorrekturfaktor, weight);
        //std::cerr << "power: " << power << std::endl;
        
        if (power > maxPower)
        {
            //okay, zu viel Leistung: Schiiieben.
            speed = pushBikeSpeed;
        }
        else
        {
            speed = getSpeed(maxPower, inclination, surfaceFactor, haltungskorrekturfaktor, weight);
            
            //speed = minSpeed;
        }
        if (speed > maxSpeed)
            speed = maxSpeed;
        /*std::cerr << "speed: " << speed << std::endl;*/
    }
    
public:
    PowerRoutingMetric(boost::shared_ptr<AltitudeProvider> provider)
        : RoutingMetric(provider), maxPower(350.0), weight(100.0), minSpeed(4.0), haltungskorrekturfaktor(0.5), pushBikeSpeed(0.5), maxSpeed(-1.0)
    {
        init();
    }
    PowerRoutingMetric(boost::shared_ptr<AltitudeProvider> provider, double weight, double maxPower, double minSpeed, double pushBikeSpeed, double haltungskorrekturfaktor, double maxSpeed = -1.0)
        : RoutingMetric(provider), maxPower(maxPower), weight(weight), minSpeed(minSpeed), haltungskorrekturfaktor(haltungskorrekturfaktor),
            pushBikeSpeed(pushBikeSpeed), maxSpeed(maxSpeed)
    {
        init();
    }
    double rateEdge(const RoutingEdge& edge, const RoutingNode& startNode, const RoutingNode& endNode)
    {
        double heightDifference;
        double distance;
        double inclination;
        double surfaceFactor;
        double streetTypeFactor;
        double timePunishment;
        double speed;
        double power;
        getCalculationValues(edge, startNode, endNode, heightDifference, distance, inclination, surfaceFactor, streetTypeFactor, timePunishment, speed, power);

        return streetTypeFactor * (distance / speed) + timePunishment;
    }
    double timeEdge(const RoutingEdge& edge, const RoutingNode& startNode, const RoutingNode& endNode)
    {
        double heightDifference;
        double distance;
        double inclination;
        double surfaceFactor;
        double streetTypeFactor;
        double timePunishment;
        double speed;
        double power;
        getCalculationValues(edge, startNode, endNode, heightDifference, distance, inclination, surfaceFactor, streetTypeFactor, timePunishment, speed, power);
        
        return (distance/speed);
    }
    double estimateDistance(const GPSPosition& p1, const GPSPosition& p2)
    {
        return (p1.calcDistance(p2) / maxSpeed);
    }
    MeasurementUnit getMeasurementUnit() {return SECONDS;}
    QString getParameterDetails() {return QString("power,maxpower=%1,weight=%2,minspeed=%3,haltungskorrekturfaktor=%4,pushbikespeed=%5,maxspeed=%6").arg(maxPower).arg(weight).arg(minSpeed).arg(haltungskorrekturfaktor).arg(pushBikeSpeed).arg(maxSpeed);}
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

namespace biker_tests
{
    int testRoutingMetrics();
}
#endif //ROUTINGMETRIC_HPP
