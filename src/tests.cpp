#include "tests.hpp"

#include "routingnode.hpp"
#include "routingedge.hpp"
#include "osmrelation.hpp"
#include <QString>

//für EXIT_SUCCESS und EXIT_FAILURE
#include <boost/program_options.hpp>


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
    
    template<typename S, typename T>
    bool check_equality(std::string message, S a, T b)
    {
        std::cout << std::left << std::setw(60) << message << " - " << std::flush;
        if (a==b)
        {
            std::cout << "passed!" << std::endl;
            return true;
        }
        else
        {
            std::cout << "failed!" << std::endl;
            std::cout << "\tValue A: " << a << std::endl;
            std::cout << "\tValue B: " << b << std::endl;
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
    template<> bool check_equality(std::string message, QString a, QString b)
    {
        return check_equality<std::string, std::string>(message, a.toStdString(), b.toStdString());
    }
    
    template bool check_equality<bool, bool>(std::string message, bool a, bool b);
    template bool check_equality(std::string message, RoutingEdge a, RoutingEdge b);
    //template bool check_equality(std::string message, RoutingNode a, RoutingNode b);
    //template bool check_equality(std::string message, std::string a, std::string b);
    template bool check_equality(std::string message, boost::uint16_t a, boost::uint16_t b);
    template bool check_equality(std::string message, boost::int16_t a,  boost::int16_t b);
    template bool check_equality(std::string message, boost::uint32_t a, boost::uint32_t b);
    template bool check_equality(std::string message, boost::uint64_t a, boost::uint64_t b);
    template bool check_equality(std::string message, boost::int64_t a,  boost::int64_t b);
    template bool check_equality(std::string message, boost::int64_t a,  boost::int32_t b);
    template bool check_equality(std::string message, boost::int32_t a,  boost::int64_t b);
    template bool check_equality(std::string message, boost::uint64_t a,  boost::uint32_t b);
    template bool check_equality(std::string message, boost::uint32_t a,  boost::uint64_t b);
    template bool check_equality(std::string message, unsigned long a,  unsigned long long b);
    
    //template<> bool check_equality(std::string message, boost::uint16_t a, boost::uint16_t b);
    //template<> bool check_equality(std::string message, bool a, bool b);
    //template<> bool check_equality(std::string message, bool a, bool b);
    //template<> bool check_equality(std::string message, bool a, bool b);
    
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
