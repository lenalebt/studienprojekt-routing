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
    double inclination = heightdifference / distance;
    double punishment=1;
    
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
    
    return distance + heightdifference * _detourPerHeightMeter * punishment;
}


double AdvancedHeightRoutingMetric::timeEdge(const RoutingEdge &edge, const RoutingNode &startNode, const RoutingNode &endNode)
{
    //Rechne mit 5m/s Fahrgeschwindigkeit
    return this->rateEdge(edge, startNode, endNode) / 5.0;
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
        CHECK_EQ(prm.rateEdge(edge, startNode, endNode), startNode.calcDistance(endNode));
        return EXIT_SUCCESS;
    }
}
