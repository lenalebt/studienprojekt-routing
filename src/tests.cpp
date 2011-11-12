#include "tests.hpp"
#include <iostream>
#include <iomanip>

#include "routingnode.hpp"
#include "routingedge.hpp"

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

#define CHECK_EQ(a,b)           if (!check_equality(std::string(QUOTEME(a)) + " == " + QUOTEME(b) + "?", a, b)) return EXIT_FAILURE;
#define CHECK_EQ_TYPE(a,b,type) if (!check_equality<type, type >(std::string(QUOTEME(a)) + " == " + QUOTEME(b) + "?", a, b)) return EXIT_FAILURE;
#define CHECK(a)                if (!check_equality(std::string(QUOTEME(a)) + " == true?", a, true)) return EXIT_FAILURE;

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
        cout << left << setw(50) << message << " - " << flush;
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
        
        node1.setAndConvertID(4316256737l);
        CHECK_EQ(node1.getID(), 1104961724672u)
        
        return EXIT_SUCCESS;
    }
}
