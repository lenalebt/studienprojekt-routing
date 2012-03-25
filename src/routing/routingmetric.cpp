#include "routingmetric.hpp" 
#include "altitudeprovider.hpp"
#include "srtmprovider.hpp"

RoutingMetric::~RoutingMetric()
{
    
}


double EuclideanRoutingMetric::rateEdge(const RoutingEdge &edge, const RoutingNode &startNode, const RoutingNode &endNode)
{
    return startNode.calcDistance(endNode);
}


double EuclideanRoutingMetric::timeEdge(const RoutingEdge &edge, const RoutingNode &startNode, const RoutingNode &endNode)
{
    //Rechne mit 5m/s Fahrgeschwindigkeit
    return this->rateEdge(edge, startNode, endNode) / 5.0;
}

double SimpleHeightRoutingMetric::rateEdge(const RoutingEdge &edge, const RoutingNode &startNode, const RoutingNode &endNode)
{
    return startNode.calcDistance(endNode) +
        fabs(_altitudeProvider->getAltitude(startNode) - _altitudeProvider->getAltitude(endNode)) * _detourPerHeightMeter;
}


double SimpleHeightRoutingMetric::timeEdge(const RoutingEdge &edge, const RoutingNode &startNode, const RoutingNode &endNode)
{
    //Rechne mit 5m/s Fahrgeschwindigkeit
    return this->rateEdge(edge, startNode, endNode) / 5.0;
}

double AdvancedHeightRoutingMetric::rateEdge(const RoutingEdge &edge, const RoutingNode &startNode, const RoutingNode &endNode)
{
    double distance = startNode.calcDistance(endNode);
    double heightdifference = _altitudeProvider->getAltitude(startNode) - _altitudeProvider->getAltitude(endNode);
    
    if (heightdifference < 0)
        heightdifference = 0;
    return distance + heightdifference * _detourPerHeightMeter;
    
    /*double inclination = heightdifference / distance;
    double punishment=1;
    
    std::cerr << "inclination: " << inclination << std::endl;
    
    if (inclination < 0)
        inclination = 0;
    else if (inclination < 0.01)
        punishment = 1.0;
    else if (inclination < 0.02)
        punishment = 1.05;
    else if (inclination < 0.03)
        punishment = 1.1;
    else if (inclination < 0.04)
        punishment = 1.15;
    else if (inclination < 0.05)
        punishment = 1.2;
    else if (inclination < 0.06)
        punishment = 1.25;
    else if (inclination < 0.07)
        punishment = 1.3;
    else if (inclination < 0.08)
        punishment = 1.35;
    else if (inclination < 0.09)
        punishment = 1.4;
    else if (inclination < 0.10)
        punishment = 1.45;
    else if (inclination < 0.11)
        punishment = 1.5;
    else if (inclination < 0.12)
        punishment = 1.55;
    else if (inclination < 0.13)
        punishment = 1.6;
    else if (inclination < 0.14)
        punishment = 1.65;
    else if (inclination >= 0.14)
        punishment = 1.7;
    
    punishment = pow(punishment, _extrapunishment);
    std::cerr << "punishment: " << punishment << std::endl;
    
    return distance + heightdifference * _detourPerHeightMeter * punishment;*/
}


double AdvancedHeightRoutingMetric::timeEdge(const RoutingEdge &edge, const RoutingNode &startNode, const RoutingNode &endNode)
{
    //Rechne mit 5m/s Fahrgeschwindigkeit
    return this->rateEdge(edge, startNode, endNode) / 5.0;
}

double PowerRoutingMetric::getPower(double speed, double inclination, double surfaceFactor, double haltungskorrekturfaktor, double weight)
{
    return calcRollingResistancePower(speed, surfaceFactor, weight) +
            calcAerodynamicResistancePower(speed, haltungskorrekturfaktor) +
            calcInclinationPower(speed, inclination, weight);
}
double PowerRoutingMetric::getSpeed(double power, double inclination, double surfaceFactor, double haltungskorrekturfaktor, double weight)
{
    double a=0;
    double b=30;
    double mid=(b-a)/2+a;
    //double pwra = getPower(a, inclination, surfaceFactor, haltungskorrekturfaktor, weight);
    //double pwrb = getPower(b, inclination, surfaceFactor, haltungskorrekturfaktor, weight);
    double pwrmid = getPower(mid, inclination, surfaceFactor, haltungskorrekturfaktor, weight);
    for (int i=0; i<20; i++)
    {
        if (pwrmid > power)
        {
            //pwrb = pwrmid;
            b=mid;
        }
        else
        {
            //pwra=pwrmid;
            a=mid;
        }
        mid=(b-a)/2+a;
        pwrmid = getPower(mid, inclination, surfaceFactor, haltungskorrekturfaktor, weight);
    }
    return mid;
}

void PowerRoutingMetric::init()
{
    /*
     * Abhängig von
     *  - Gewicht (fest)
     *  - Haltungskorrekturfaktor (fest)
     *  - Steigung (0% - +15%, evtl 0.5%-Schritte)
     *  - Oberflächenfaktor (0.1 - 3, 0.1-Schritte)
     *  - Geschwindigkeit (0-25m/s, 0.25m/s-Schritte) oder Leistung (0-1000W, 10W-Schritte)
     */
    /* //Wird noch nicht benutzt, das was folgt...
    powerarray = new double**[30];
    speedarray = new double**[30];
    for (int i = 0; i < 30; i++)
    {
        powerarray[i] = new double*[30];
        speedarray[i] = new double*[30];
        
        double inclination = 0.5 * i;
        for (int s = 0; s < 30; s++)
        {
            powerarray[i][s] = new double[100];
            speedarray[i][s] = new double[100];
        
            double surfaceFactor = 0.1 * s;
            for (int p = 0; p < 100; p++)
            {
                double power = 10 * p;
                double speed = 0.25 * p;
                
                powerarray[i][s][p] = getPower(speed, inclination, surfaceFactor, haltungskorrekturfaktor, weight);
                speedarray[i][s][p] = getSpeed(power, inclination, surfaceFactor, haltungskorrekturfaktor, weight);
            }
        }
    }*/
    
    maxSpeed = (maxSpeed < 0.0) ? (getSpeed(maxPower, 0.0, 0.5, haltungskorrekturfaktor, weight) * 1.5) : maxSpeed;
    
    std::cerr << "maxSpeed: " << maxSpeed << std::endl;
    std::cerr << "minSpeed: " << minSpeed << std::endl;
    std::cerr << "pushBikeSpeed: " << pushBikeSpeed << std::endl;
    std::cerr << "maxPower: " << maxPower << std::endl;
    std::cerr << "weight: " << weight << std::endl;
    
    /*for (int i=0; i<500; i+=20)
        std::cerr << "getSpeed("<<i<<", 0.0, 0.5, 0.5, 100)=" << getSpeed(i, 0.0, 0.5, 0.5, 100)*3.6 << std::endl;*/
    /*
    getSpeed(0, 0.0, 0.5, 0.5, 100)=5.14984e-05
    getSpeed(20, 0.0, 0.5, 0.5, 100)=10.8795
    getSpeed(40, 0.0, 0.5, 0.5, 100)=15.422
    getSpeed(60, 0.0, 0.5, 0.5, 100)=18.4742
    getSpeed(80, 0.0, 0.5, 0.5, 100)=20.8466
    getSpeed(100, 0.0, 0.5, 0.5, 100)=22.8195
    getSpeed(120, 0.0, 0.5, 0.5, 100)=24.5254
    getSpeed(140, 0.0, 0.5, 0.5, 100)=26.0387
    getSpeed(160, 0.0, 0.5, 0.5, 100)=27.4051
    getSpeed(180, 0.0, 0.5, 0.5, 100)=28.6555
    getSpeed(200, 0.0, 0.5, 0.5, 100)=29.8115
    getSpeed(220, 0.0, 0.5, 0.5, 100)=30.8889
    getSpeed(240, 0.0, 0.5, 0.5, 100)=31.8997
    getSpeed(260, 0.0, 0.5, 0.5, 100)=32.8534
    getSpeed(280, 0.0, 0.5, 0.5, 100)=33.7571
    getSpeed(300, 0.0, 0.5, 0.5, 100)=34.617
    getSpeed(320, 0.0, 0.5, 0.5, 100)=35.4379
    getSpeed(340, 0.0, 0.5, 0.5, 100)=36.224
    getSpeed(360, 0.0, 0.5, 0.5, 100)=36.9787
    getSpeed(380, 0.0, 0.5, 0.5, 100)=37.7049
    getSpeed(400, 0.0, 0.5, 0.5, 100)=38.4053
    getSpeed(420, 0.0, 0.5, 0.5, 100)=39.0819
    getSpeed(440, 0.0, 0.5, 0.5, 100)=39.7365
    getSpeed(460, 0.0, 0.5, 0.5, 100)=40.3711
    getSpeed(480, 0.0, 0.5, 0.5, 100)=40.9869
    */
}

namespace biker_tests
{
    int testRoutingMetrics()
    {
        RoutingNode startNode(1, 51.0, 7.0);
        RoutingNode endNode(2, 51.5,7.2);
        RoutingEdge edge(5, 1, 2);
        boost::shared_ptr<AltitudeProvider> srtmProvider(new SRTMProvider());
        boost::shared_ptr<AltitudeProvider> zeroProvider(new ZeroAltitudeProvider());
        EuclideanRoutingMetric erm;
        CHECK_EQ(erm.rateEdge(edge, startNode, endNode), startNode.calcDistance(endNode));
        
        SimpleHeightRoutingMetric shrm1(zeroProvider, 100.0);
        SimpleHeightRoutingMetric shrm2(srtmProvider, 100.0);
        CHECK_EQ(shrm1.rateEdge(edge, startNode, endNode), startNode.calcDistance(endNode));
        CHECK_EQ(shrm2.rateEdge(edge, startNode, endNode), 60313.476152647061099);
        
        PowerRoutingMetric prm(zeroProvider, 100, 200, 4, 0.5, 0.5, 1.5);
        //CHECK_EQ(prm.rateEdge(edge, startNode, endNode), startNode.calcDistance(endNode));
        //prm.test();
        //TODO
        return EXIT_SUCCESS;
    }
}
