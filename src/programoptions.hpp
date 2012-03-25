#ifndef PROGRAMOPTIONS_HPP
#define PROGRAMOPTIONS_HPP

#include <string>
#include "gpsposition.hpp"
#include <boost/shared_ptr.hpp>

class ProgramOptions
{
public:
    //Webserver-spezifischer kram
    std::string webserver_public_html_folder;
    unsigned int webserver_port;
    unsigned int webserver_threadpool_size;
    bool webserver_startWebserver;
    std::string webserver_apikey;
    bool webserver_no_serve_files;
    
    //Threads
    unsigned int threads_threadpool_size;
    
    //Tests
    std::string tests_testName;
    bool tests_starttest;
    
    //Datenvorverarbeitung
    std::string osmFilename;
    bool parseOsmFile;
    bool simpleParseOsmFile;
    
    //Datenbank
    std::string dbFilename;
    std::string dbBackend;
    unsigned int dbCacheSize;
    
    //Routenberechnung
    bool doRouting;
    GPSPosition routingStartPoint;
    GPSPosition routingEndPoint;
    std::string routingStartPointString;
    std::string routingEndPointString;
    bool routeOutputAsGPX;
    
    ProgramOptions();
    
    static boost::shared_ptr<ProgramOptions> getInstance();
};

#endif //PROGRAMOPTIONS_HPP
