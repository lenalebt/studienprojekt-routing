#include "routingmetric.hpp" 
#include "altitudeprovider.hpp"
#include "srtmprovider.hpp"

RoutingMetric::~RoutingMetric()
{
    
}


double EuclidianRoutingMetric::rateEdge(const RoutingEdge &edge, const RoutingNode &startNode, const RoutingNode &endNode)
{
    return startNode.calcDistance(endNode);
}


double EuclidianRoutingMetric::timeEdge(const RoutingEdge &edge, const RoutingNode &startNode, const RoutingNode &endNode)
{
    //Rechne mit 5m/s Fahrgeschwindigkeit
    return this->rateEdge(edge, startNode, endNode) / 5;
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

double PowerRoutingMetric::getPower(float speed, float inclination, float surfaceFactor, float haltungskorrekturfaktor, float weight)
{
    return calcRollingResistancePower(speed, surfaceFactor, weight) +
            calcAerodynamicResistancePower(speed, haltungskorrekturfaktor) +
            calcInclinationPower(speed, inclination, weight);
}
double PowerRoutingMetric::getSpeed(float power, float inclination, float surfaceFactor, float haltungskorrekturfaktor, float weight)
{
    for (int i=0; i<500; i++)
    {
        double speed = 0.03*i;
        double pwr = getPower(speed, inclination, surfaceFactor, haltungskorrekturfaktor, weight);
        if (pwr >= power)
            return speed;
    }
    return 0.0;
}

void PowerRoutingMetric::test()
{
    std::cerr << "Test für PowerRoutingMetric" << std::endl;
    
    std::cerr << "getSpeed(250, 0.01, 1, 0.5, 100)=" << getSpeed(250, 0.01, 1, 0.5, 100) << std::endl;
    
    /*
    float***** powerArray;
    float***** speedArray;
    
    powerArray = new float****[50];
    speedArray = new float****[50];
    
    for (int i = 0; i < 50; i++)
    {
        powerArray[i] = new float***[30];
        speedArray[i] = new float***[30];
        
        float speed = 0.3 * i;
        float power = 15.0 * i;
        for (int j = 0; j < 30; j++)
        {
            powerArray[i][j] = new float**[20];
            speedArray[i][j] = new float**[20];
            
            float inclination = 0.5 * j;
            //std::cerr << inclination << std::endl;
            for (int k = 0; k < 20; k++)
            {
                powerArray[i][j][k] = new float*[4];
                speedArray[i][j][k] = new float*[4];
                
                float surfaceFactor = 0.1 * k;
                //std::cerr << surfaceFactor << std::endl;
                for (int l = 0; l < 4; l++)
                {
                    float haltungskorrekturfaktor;
                    switch (l)
                    {
                        case 0: haltungskorrekturfaktor = 0.5;
                                break;
                        case 1: haltungskorrekturfaktor = 0.4;
                                break;
                        case 2: haltungskorrekturfaktor = 0.3;
                                break;
                        case 3: haltungskorrekturfaktor = 0.25;
                                break;
                        default:haltungskorrekturfaktor = 0.5;
                                break;
                    }
                    for (int m = 0; m < 20; m++)
                    {
                        float weight = m * 10;
                        
                        powerArray[i][j][k][l] = new float[20];
                        speedArray[i][j][k][l] = new float[20];
                        
                        powerArray[i][j][k][l][m] = getPower(speed, inclination, surfaceFactor, haltungskorrekturfaktor, weight);
                        speedArray[i][j][k][l][m] = getSpeed(power, inclination, surfaceFactor, haltungskorrekturfaktor, weight);
                    }
                }
            }
        }
    }
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
        EuclidianRoutingMetric erm;
        CHECK_EQ(erm.rateEdge(edge, startNode, endNode), startNode.calcDistance(endNode));
        
        SimpleHeightRoutingMetric shrm1(zeroProvider, 100.0);
        SimpleHeightRoutingMetric shrm2(srtmProvider, 100.0);
        CHECK_EQ(shrm1.rateEdge(edge, startNode, endNode), startNode.calcDistance(endNode));
        CHECK_EQ(shrm2.rateEdge(edge, startNode, endNode), 60313.476152647061099);
        
        PowerRoutingMetric prm(zeroProvider, 100, 200, 4);
        //CHECK_EQ(prm.rateEdge(edge, startNode, endNode), startNode.calcDistance(endNode));
        prm.test();
        return EXIT_SUCCESS;
    }
}
