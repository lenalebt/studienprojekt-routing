#include "edgecategorizer.hpp"

EdgeCategorizer::EdgeCategorizer(BlockingQueue<boost::shared_ptr<OSMEdge> >* inQueue, BlockingQueue<boost::shared_ptr<RoutingEdge> >* outQueue) :
    _edgeQueue(inQueue), _wayQueue(0), _outQueue(outQueue)
{
    
}
EdgeCategorizer::EdgeCategorizer(BlockingQueue<boost::shared_ptr<OSMWay> >* inQueue, BlockingQueue<boost::shared_ptr<RoutingEdge> >* outQueue) :
    _edgeQueue(0), _wayQueue(inQueue), _outQueue(outQueue)
{
    
}

boost::shared_ptr<RoutingEdge> EdgeCategorizer::categorizeEdge(const OSMEdge &osmEdge)
{
    bool hasTrafficLights = false;
    bool hasTrafficCalmingBumps = false;
    //bool hasStopSign = false;
    bool hasStairs = false;
    bool hasCycleBarrier = false;
    bool isDesignated = false;
    boost::uint8_t streetType = STREETTYPE_UNKNOWN;
    boost::uint8_t cyclewayType = CYCLEWAYTYPE_UNKNOWN;
    boost::uint8_t streetSurfaceType = STREETSURFACETYPE_UNKNOWN;
    boost::uint8_t streetSurfaceQuality = STREETSURFACEQUALITY_UNKNOWN;    
    boost::uint8_t access = ACCESS_YES;
    //boost::uint8_t turnType = TURNTYPE_STRAIGHT; // TURNTYPE won't be categorized in this function
    
    static boost::uint64_t edgeID=0;
    
    boost::shared_ptr<RoutingEdge> routingEdge = boost::shared_ptr<RoutingEdge>(new RoutingEdge(edgeID++, RoutingNode::convertIDToLongFormat(osmEdge.getStartNode()), RoutingNode::convertIDToLongFormat(osmEdge.getEndNode())));

    //Flags
    bool isForward = osmEdge.getForward();
    bool isArea = false;
    bool isHighway = false;
    bool isOfficial = false;
    bool hasDesignation = false;


    QVector<OSMProperty> properties = osmEdge.getProperties();
    for (QVector<OSMProperty>::const_iterator it = properties.constBegin(); it != properties.constEnd(); it++)
    {
        QString osmKey = it->getKey();
        QString osmValue = it->getValue();

        //DONE:
        // Key: smoothness, surface, tracktype(only grade1), mtb:scale,
        //TODO:
        // highway, stop, traffic_signal, cycleway, oneway, bicycle, compulsory und designation zusammenstellen

        if(osmValue == "impassalbe"){
            streetSurfaceQuality = STREETSURFACEQUALITY_IMPASSABLE;
            access = ACCESS_NOT_USABLE_FOR_BIKES;
            break; //if the edge can't be passed by bike, there is no need for further categorization
        }

        else if(osmKey == "ice_road" || osmKey == "motorroad" || osmKey == "winterroad"){
            if(osmValue == "yes"){
                access = ACCESS_NOT_USABLE_FOR_BIKES;
                break; //if the edge can't be passed by bike, there is no need for further categorization
            }
        }

        else if(osmKey == "mtb:scale"){
            if(osmValue == "0"){
                cyclewayType = CYCLEWAYTYPE_MTB_0;
            }
            else if(osmValue == "1"){
                cyclewayType = CYCLEWAYTYPE_MTB_1;
            }
            else if(osmValue == "2"){
                cyclewayType = CYCLEWAYTYPE_MTB_2;
            }
            else if(osmValue == "3"){
                cyclewayType = CYCLEWAYTYPE_MTB_3;
            }
            else{
                cyclewayType = CYCLEWAYTYPE_MTB_HIGH;
            }
        }

        else if(osmKey == "highway"){
            isHighway = true;

            if(osmValue == "bus_guideway" ||
                    osmValue == "construction" ||
                    osmValue == "motorway" ||
                    osmValue == "motorway_link" ||
                    osmValue == "proposed" ||
                    osmValue == "raceway"){
                access = ACCESS_NOT_USABLE_FOR_BIKES;
                break; //if the edge can't be passed by bike, there is no need for further categorization
            }
            else if(osmValue == "pedestrian"){
                streetType = STREETTYPE_HIGHWAY_PEDESTRIAN;

            }
            else if(osmValue == "primary" || osmValue == "primary_link" || osmValue == "trunk" || osmValue == "trunk_link"){
                streetType = STREETTYPE_HIGHWAY_PRIMARY;

            }
            else if(osmValue == "secondary" || osmValue == "secondary_link"){
                streetType = STREETTYPE_HIGHWAY_SECONDARY;

            }
            else if(osmValue == "tertiary" || osmValue == "tertiary_link"){
                streetType = STREETTYPE_HIGHWAY_TERTIARY;

            }
            else if(osmValue == "track" || osmValue == "byway"){
                streetType = STREETTYPE_HIGHWAY_TRACK;

            }
            else if(osmValue == "path"){
                streetType = STREETTYPE_HIGHWAY_PATH;

            }
            else if(osmValue == "living_street"){
                streetType = STREETTYPE_HIGHWAY_LIVINGSTREET;

            }
            else if(osmValue == "residential"){
                streetType = STREETTYPE_HIGHWAY_RESIDENTIAL;

            }
            else if(osmValue == "junction"){
                streetType = STREETTYPE_HIGHWAY_JUNCTION;

            }
            else if(osmValue == "service"){
                streetType = STREETTYPE_HIGHWAY_SERVICE;

            }
            else if(osmValue == "ford"){
                streetType = STREETTYPE_HIGHWAY_FORD;

            }
            else{
                streetType = STREETTYPE_UNKNOWN;

            }

            if(osmValue == "steps"){
                hasStairs = true;

            }
        }

        else if(osmKey == "access"){
            if(osmValue == "no"){
                access = ACCESS_NOT_USABLE_FOR_BIKES;
                break; //if the edge can't be passed by bike, there is no need for further categorization
            }
            else if(osmValue == "private"){
                access = ACCESS_PRIVATE;
            }
            else if(osmValue == "delivery"){
                access = ACCESS_DELIVERY;
            }
            else if(osmValue == "destination"){
                access = ACCESS_DESTINATION;
            }
            else if(osmValue == "customer"){
                access = ACCESS_CUSTOMER;
            }
            else if(osmValue == "agricultural"){
                access = ACCESS_AGRICULTURAL;
            }
            else if(osmValue == "forestry"){
                access = ACCESS_FORESTRY;
            }
            else if(osmValue == "permissive"){
                access = ACCESS_PERMISSIVE;
            }
            else if(osmValue == "designated"){
                hasDesignation = true;
            }
            else if(osmValue == "official"){
                isOfficial = true;
            }
        }

        else if(osmKey == "smoothness"){
            if(osmValue == "exellent"){
                    streetSurfaceQuality = STREETSURFACEQUALITY_EXCELLENT;
            }
            else if(osmValue == "good"){
                    streetSurfaceQuality = STREETSURFACEQUALITY_GOOD;
            }
            else if(osmValue == "intermediate"){
                    streetSurfaceQuality = STREETSURFACEQUALITY_INTERMEDIATE;
            }
            else if(osmValue == "bad"){
                    streetSurfaceQuality = STREETSURFACEQUALITY_BAD;
            }
            else if(osmValue == "very_bad"){
                    streetSurfaceQuality = STREETSURFACEQUALITY_VERYBAD;
            }
            else if(osmValue == "horrible"){
                    streetSurfaceQuality = STREETSURFACEQUALITY_HORRIBLE;
            }
            else if(osmValue == "very_horrible"){
                    streetSurfaceQuality = STREETSURFACEQUALITY_VERYHORRIBLE;
            }
        }

        else if(osmKey == "surface" || osmKey.contains("tracktype")){
            if(osmValue == "paved" || osmValue == "grade:1"){
                streetSurfaceType = STREETSURFACETYPE_PAVED;
            }
            else if(osmValue == "asphalt"){
                    streetSurfaceType = STREETSURFACETYPE_ASPHALT;
            }
            else if(osmValue == "sett"){
                    streetSurfaceType = STREETSURFACETYPE_SETT;
            }
            else if(osmValue == "paving_stones"){
                    streetSurfaceType = STREETSURFACETYPE_PAVING_STONES;
            }
            else if(osmValue == "tartarn"){
                    streetSurfaceType = STREETSURFACETYPE_TARTAN;
            }
            else if(osmValue.contains("concrete")){
                    streetSurfaceType = STREETSURFACETYPE_CONCRETE;
            }
            else if(osmValue == "cobblestone"){
                    streetSurfaceType = STREETSURFACETYPE_COBBLESTONE;
            }
            else if(osmValue == "compacted"){
                    streetSurfaceType = STREETSURFACETYPE_COMPACTED;
            }
            else if(osmValue == "fine_gravel"){
                    streetSurfaceType = STREETSURFACETYPE_FINEGRAVEL;
            }
            else if(osmValue == "grass_paver"){
                    streetSurfaceType = STREETSURFACETYPE_GRASSPAVER;
            }
            else if(osmValue == "gravel" || "pebblestone"){
                    streetSurfaceType = STREETSURFACETYPE_GRAVEL;
            }
            else if(osmValue == "ground" || osmValue == "dirt" || osmValue == "mud" || osmValue == "earth" || osmValue == "clay" || osmValue == "sand"){
                    streetSurfaceType = STREETSURFACETYPE_GROUND;
            }
            else if(osmValue == "grass" || osmValue == "artificial_turf"){
                    streetSurfaceType = STREETSURFACETYPE_GRASS;
            }
            else if(osmValue == "metal"){
                    streetSurfaceType = STREETSURFACETYPE_METAL;
            }
            else{
                    streetSurfaceType = STREETSURFACETYPE_UNPAVED;
            }
         }

    }// END: osmEdge properties iterator

    //Flagauswertung//////////////////////////////////////
    if(isArea && !isHighway){
        access = ACCESS_NOT_USABLE_FOR_BIKES;
    }
    //Flagauswertung//END/////////////////////////////////

    routingEdge->setTrafficLights(hasTrafficLights);
    routingEdge->setTrafficCalmingBumps(hasTrafficCalmingBumps);
    //routingEdge->setStopSign(hasStopSign);
    routingEdge->setStairs(hasStairs);
    routingEdge->setCycleBarrier(hasCycleBarrier);
    routingEdge->setAccess(access);
    routingEdge->setStreetType(streetType);
    routingEdge->setCyclewayType(cyclewayType);
    routingEdge->setStreetSurfaceType(streetSurfaceType);
    routingEdge->setStreetSurfaceQuality(streetSurfaceQuality);
    //routingEdge->setTurnType(turnType);

    return routingEdge;
}

bool EdgeCategorizer::startCategorizerLoop()
{
    //std::cerr << "categorizer loop..." << std::endl;
    if (_edgeQueue != 0)
        return categorizeEdges();
    else if (_wayQueue != 0)
        return categorizeWays();
    else
        return false;
}

bool EdgeCategorizer::categorizeEdges()
{
    boost::shared_ptr<OSMEdge> osmEdge;
    while (_edgeQueue->dequeue(osmEdge))
    {
        boost::shared_ptr<RoutingEdge> edge = categorizeEdge(*osmEdge);
        if (edge->getAccess() != ACCESS_NOT_USABLE_FOR_BIKES)
            _outQueue->enqueue(edge);
    }
    _outQueue->destroyQueue();
    return false;
}
bool EdgeCategorizer::categorizeWays()
{
    boost::shared_ptr<OSMWay> osmWay;
    //std::cerr << "categorize ways..." << std::endl;
    while (_wayQueue->dequeue(osmWay))
    {
        //std::cerr << "dequeue way..." << std::endl;
        QVector<OSMEdge> edgeList = osmWay->getEdgeList();
        for(int i = 0; i < edgeList.size(); i++)
        {
            boost::shared_ptr<RoutingEdge> edge = categorizeEdge(edgeList[i]);
            if (edge->getAccess() != ACCESS_NOT_USABLE_FOR_BIKES)
                _outQueue->enqueue(edge);
            /*else
                std::cerr << "edge not usable for bikes:" << edge->getID() << std::endl;*/
        }
    }
    _outQueue->destroyQueue();
    return false;
}
