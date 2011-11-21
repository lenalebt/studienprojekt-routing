#include "tests.hpp"
#include <iostream>
#include <iomanip>

#include "routingnode.hpp"
#include "routingedge.hpp"
#include "osmrelation.hpp"

//für EXIT_SUCCESS und EXIT_FAILURE
#include <boost/program_options.hpp>

/**
 * @brief Zur Erzeugung eines Strings aus eines Defines.
 */
#define QUOTEME_(x) #x
/**
 * @brief Zur Erzeugung eines Strings aus eines Defines.
 */
#define QUOTEME(x) QUOTEME_(x)

#if defined __FILE__ && defined __LINE__
    #define LINESTR(a,b)           biker_tests::basename(std::string(QUOTEME(__FILE__))) + ":" + QUOTEME(__LINE__) + ": "+ QUOTEME(a) + " == " + QUOTEME(b) + "?"
#else
    #define LINESTR(a,b)           std::string(QUOTEME(a)) + " == " + QUOTEME(b) + "?"
#endif

#define CHECK_EQ(a,b)           if (!check_equality(LINESTR(a,b), a, b)) return EXIT_FAILURE;
#define CHECK_EQ_TYPE(a,b,type) if (!check_equality<type, type >(LINESTR(a,b), a, b)) return EXIT_FAILURE;
#define CHECK(a)                if (!check_equality(LINESTR(a,true), a, true)) return EXIT_FAILURE;
/**
 * @file
 * @ingroup tests
 * @todo i18n/l10n
 * @attention Diese Testfunktionen können das Programm komplett in einen anderen
 *      Zustand überführen als es vor dem Aufruf der Funktion hatte
 */

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

    template<typename S, typename T>
    bool check_equality(std::string message, S a, T b)
    {
        cout << left << setw(60) << message << " - " << flush;
        if (a==b)
        {
            cout << "passed!" << endl;
            return true;
        }
        else
        {
            cout << "failed!" << endl;
            cout << "\tValue A: " << a << endl;
            cout << "\tValue B: " << b << endl;
            return false;
        }
    }
    template<>
    bool check_equality(std::string message, boost::uint8_t a, boost::uint8_t b)
    {
        return check_equality<int, int>(message, a, b);
    }

    int testProgram(std::string testName)
    {
        cout << "starting program tests..." << endl << flush;
        
        boost::algorithm::to_lower(testName);
        cout << "requested test: " << testName << endl;
        
        if (testName == "routingedge")
            return testRoutingEdge();
        else if (testName == "uint64_t2string")
            return test_uint64_t2string();
        else if (testName == "routingnode")
            return testRoutingNode();
        else if (testName == "basename")
            return testBasename();
        
        //Anpassen, falls Fehler auftraten!
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

    int testRoutingEdge()
    {
        RoutingEdge edge1(1), edge2(1);
        
        CHECK_EQ(edge1.getID(), 1u);
        
        edge1.setStairs(true);
        CHECK(edge1.hasStairs());
        
        edge1.setCycleBarrier(true);
        CHECK(edge1.hasCycleBarrier());
        
        edge1.setStopSign(false);
        CHECK(!edge1.hasStopSign());
        
        edge1.setStreetSurfaceQuality(5);
        CHECK_EQ_TYPE(edge1.getStreetSurfaceQuality(), 5, int);
        
        edge1.setCyclewayType(6);
        CHECK_EQ_TYPE(edge1.getCyclewayType(), 6, int);
        
        edge1.setStreetType(15);
        CHECK_EQ_TYPE(edge1.getStreetType(), 15, int);
        
        edge2.setProperties(edge1.getProperties());
        CHECK_EQ(edge1, edge2);
        
        CHECK_EQ(edge1.getProperties(), edge2.getProperties());
        
        return EXIT_SUCCESS;
    }
    
    int testRoutingNode()
    {
        RoutingNode node1(1), node2(2);
        CHECK_EQ(node1.getID(), 1u);
        
        node1.setAndConvertID(1);
        CHECK_EQ(node1.getID(), 256u)
        
        node1.setAndConvertID(2);
        CHECK_EQ(node1.getID(), 512u)
        
        node1.setAndConvertID(3);
        CHECK_EQ(node1.getID(), 768u)
        
        node1.setAndConvertID(4316256737ll);
        CHECK_EQ(node1.getID(), 1104961724672ull)
        
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
