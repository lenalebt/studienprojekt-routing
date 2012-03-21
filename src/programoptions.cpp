#include "programoptions.hpp"

ProgramOptions::ProgramOptions() :
    webserver_public_html_folder(""),
    webserver_port(8080),
    webserver_threadpool_size(20),
    webserver_startWebserver(true),
    webserver_apikey(""),
    
    threads_threadpool_size(20),
    
    tests_testName("all"),
    tests_starttest(false),
    
    osmFilename(""),
    parseOsmFile(false),
    simpleParseOsmFile(false),
    
    dbFilename(""),
    dbBackend("spatialite"),
    dbCacheSize(500000u),
    
    doRouting(false),
    routingStartPoint(),
    routingEndPoint(),
    routingStartPointString(""),
    routingEndPointString(""),
    routeOutputAsGPX(true)
{
    
}

boost::shared_ptr<ProgramOptions> ProgramOptions::getInstance()
{
    static boost::shared_ptr<ProgramOptions> instance;
    if (instance)
        return instance;
    else
    {
        instance.reset(new ProgramOptions());
        return instance;
    }
}
