#include "tests.hpp"
#include <iostream>
#include <iomanip>
#include <boost/algorithm/string.hpp> 

#include "routingnode.hpp"
#include "routingedge.hpp"
#include "osmturnrestriction.hpp"
#include "osmnode.hpp"
#include "osmway.hpp"
#include "osmproperty.hpp"
#include "osmedge.hpp"
#include "gpsposition.hpp"
#include "gpsroute.hpp"
#include "spatialitedatabase.hpp"
#include "temporarydatabase.hpp"
#include "osmparser.hpp"
#include "blockingqueue.hpp"
#include "closedlist.hpp"
#include "heap.hpp"
#include "datapreprocessing.hpp"


//für EXIT_SUCCESS und EXIT_FAILURE
#include <boost/program_options.hpp>


/**
 * @file
 * @ingroup tests
 * @todo i18n/l10n
 * @attention Diese Testfunktionen können das Programm komplett in einen anderen
 *      Zustand überführen als es vor dem Aufruf der Funktion hatte
 */

#define TEST_PASSED_MSG_WIDTH 80

using namespace std;

namespace biker_tests
{
    std::string basename(std::string filename)
    {
        //Suche letzten Pfadtrenner
        size_t pos = filename.find_last_of("/\\:");
        //Setze die Position hinter den Pfadtrenner - wenn es keinen gibt, nach ganz vorn.
        pos = (pos!=std::string::npos) ? pos+1 : 0;
        //schneide den Dateinamen aus.
        return filename.substr(pos, filename.length() - pos - (filename.find_last_of("\"") != std::string::npos));
    }
    
    template<typename S, typename T>
    bool check_equality(std::string message, S a, T b)
    {
        std::cout << std::left << std::setw(TEST_PASSED_MSG_WIDTH) << message << " - " << std::flush;
        if (a==b)
        {
            std::cout << "passed!" << std::endl;
            return true;
        }
        else
        {
            std::cout << "failed!" << std::endl;
            std::cout << "\tValue A: " << std::fixed << std::setprecision(15) << a << std::endl;
            std::cout << "\tValue B: " << std::fixed << std::setprecision(15) << b << std::endl;
            return false;
        }
    }
    
    template<> bool check_equality(std::string message, boost::uint8_t a, boost::uint8_t b)
    {
        return check_equality<boost::int32_t, boost::int32_t>(message, a, b);
    }
    template<> bool check_equality(std::string message, std::string a, QString b)
    {
        return check_equality<std::string, std::string>(message, a, b.toStdString());
    }
    template<> bool check_equality(std::string message, QString a, std::string b)
    {
        return check_equality<std::string, std::string>(message, a.toStdString(), b);
    }
    template<> bool check_equality(std::string message, double a, double b)
    {
        std::cout << std::left << std::setw(TEST_PASSED_MSG_WIDTH) << message << " - " << std::flush;
        if (fabs(a - b) < DOUBLE_EQUALITY_BARRIER)
        {
            std::cout << "passed!" << std::endl;
            return true;
        }
        else
        {
            std::cout << "failed!" << std::endl;
            std::cout << "\tValue A: " << std::fixed << std::setprecision(15) << a << std::endl;
            std::cout << "\tValue B: " << std::fixed << std::setprecision(15) << b << std::endl;
            return false;
        }
    }
    template<> bool check_equality(std::string message, float a, float b)
    {
        std::cout << std::left << std::setw(TEST_PASSED_MSG_WIDTH) << message << " - " << std::flush;
        if (fabs(a - b) < FLOAT_EQUALITY_BARRIER)
        {
            std::cout << "passed!" << std::endl;
            return true;
        }
        else
        {
            std::cout << "failed!" << std::endl;
            std::cout << "\tValue A: " << std::fixed << std::setprecision(15) << a << std::endl;
            std::cout << "\tValue B: " << std::fixed << std::setprecision(15) << b << std::endl;
            return false;
        }
    }
    template<> bool check_equality(std::string message, float a, double b)
    {
        return check_equality<float, float>(message, a, b);
    }
    template<> bool check_equality(std::string message, double a, float b)
    {
        return check_equality<double, double>(message, a, b);
    }
    
    
    /* Diese Template-Ausprägungen müssen gemacht werden, wenn man einen neuen Typ
     * braucht, für den man Tests machen möchte. Der Typ muss einen operator<<
     * bereitstellen für einen Ausgabestrom (Beispieldefinition siehe
     * src/dataprimitives/routingedge.{h,c}pp), sowie einen operator==
     * (Beispiel ebenda). Ansonsten nur eine Zeile wie sie hier folgen.
     */
    template bool check_equality<bool, bool>(std::string message, bool a, bool b);
    template bool check_equality(std::string message, RoutingEdge a, RoutingEdge b);
    //template bool check_equality(std::string message, RoutingNode a, RoutingNode b);
    template bool check_equality(std::string message, std::string a, std::string b);
    template bool check_equality(std::string message, QString a, QString b);
    template bool check_equality(std::string message, boost::uint16_t a, boost::uint16_t b);
    template bool check_equality(std::string message, boost::int16_t a,  boost::int16_t b);
    template bool check_equality(std::string message, boost::uint32_t a, boost::uint32_t b);
    template bool check_equality(std::string message, boost::uint64_t a, boost::uint64_t b);
    template bool check_equality(std::string message, boost::int64_t a,  boost::int64_t b);
    template bool check_equality(std::string message, boost::int32_t a,  boost::int32_t b);
    template bool check_equality(std::string message, boost::int64_t a,  boost::int32_t b);
    template bool check_equality(std::string message, boost::int32_t a,  boost::int64_t b);
    template bool check_equality(std::string message, boost::uint64_t a, boost::uint32_t b);
    template bool check_equality(std::string message, boost::uint32_t a, boost::uint64_t b);
    template bool check_equality(std::string message, unsigned long a,   unsigned long long b);
    template bool check_equality(std::string message, OSMProperty a,     OSMProperty b);
    template bool check_equality(std::string message, OSMNode a,         OSMNode b);
    template bool check_equality(std::string message, OSMTurnRestriction a, OSMTurnRestriction b);
    
    std::string uint64_t2string(boost::uint64_t integer)
    {
        std::string retVal("");
        //Anmerkung: (unsigned0 - 1) ist groß! Hoffentlich überall...
        for (boost::uint64_t i=63; i<64; i--)
        {
            retVal += ((integer & (1ull<<i)) > 0) ? "1" : "0";
        }
        return retVal;
    }

    int testProgram(std::string testName)
    {
        cout << "starting program tests..." << endl << flush;
        
        boost::algorithm::to_lower(testName);
        cout << "requested test: " << testName << endl;
        
        if (testName == "routingedge")
            return biker_tests::testRoutingEdge();
        else if (testName == "uint64_t2string")
            return biker_tests::test_uint64_t2string();
        else if (testName == "routingnode")
            return biker_tests::testRoutingNode();
        else if (testName == "basename")
            return biker_tests::testBasename();
        else if (testName == "spatialitedatabaseconnection")
            return biker_tests::testSpatialiteDatabaseConnection();
        else if (testName == "temporaryosmdatabaseconnection")
            return biker_tests::testTemporaryOSMDatabaseConnection();
        else if (testName == "gpsposition")
            return biker_tests::testGPSPosition();
        else if (testName == "osmnode")
            return biker_tests::testOSMNode();
        else if (testName == "osmway")
            return biker_tests::testOSMWay();
        else if (testName == "osmedge")
            return biker_tests::testOSMEdge();
        else if (testName == "osmproperty")
            return biker_tests::testOSMProperty();
        else if (testName == "osmturnrestriction")
            return biker_tests::testOSMTurnRestriction();
        else if (testName == "osmparser")
            return biker_tests::testOSMParser();
        else if (testName == "blockingqueue")
            return biker_tests::testBlockingQueue();
        else if (testName == "binaryheap")
            return biker_tests::testBinaryHeap();
        else if (testName == "hashclosedlist")
            return biker_tests::testHashClosedList();
        else if (testName == "gpsroute")
            return biker_tests::testGPSRoute();
        else if(testName == "datapreprocessing")
            return biker_tests::testDataPreprocessing();
        
        //Anpassen, falls Fehler auftraten!
        std::cout << "error: did not find test \"" << testName << "\"." << std::endl;
        return EXIT_FAILURE;
    }
    
    int test_uint64_t2string()
    {
        CHECK_EQ(uint64_t2string(1), "0000000000000000000000000000000000000000000000000000000000000001");
        CHECK_EQ(uint64_t2string(5), "0000000000000000000000000000000000000000000000000000000000000101");
        CHECK_EQ(uint64_t2string(4096), "0000000000000000000000000000000000000000000000000001000000000000");
        CHECK_EQ(uint64_t2string(827659816459016745ull), "0000101101111100011100000011011111001010111001111001101000101001");
        
        return EXIT_SUCCESS;
    }
    
    int testBasename()
    {
        CHECK_EQ_TYPE(biker_tests::basename("test.cpp"), "test.cpp", std::string);
        CHECK_EQ_TYPE(biker_tests::basename("/test.cpp"), "test.cpp", std::string);
        CHECK_EQ_TYPE(biker_tests::basename("/home/lala/test.cpp"), "test.cpp", std::string);
        CHECK_EQ_TYPE(biker_tests::basename("\"/home/lala/test.cpp\""), "test.cpp", std::string);
        CHECK_EQ_TYPE(biker_tests::basename("C:\\home\\lala\\test.cpp"), "test.cpp", std::string);
        CHECK_EQ_TYPE(biker_tests::basename("C:\\home\\lala/test.cpp"), "test.cpp", std::string);
        CHECK_EQ_TYPE(biker_tests::basename("\"C:\\home\\lala\\test.cpp\""), "test.cpp", std::string);
        CHECK_EQ_TYPE(biker_tests::basename("C:test.cpp"), "test.cpp", std::string);
        
        return EXIT_SUCCESS;
    }
}

std::ostream& operator<<(std::ostream& os, const QString& qs)
{
    return (os << qs.toStdString());
}
