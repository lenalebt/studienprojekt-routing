#ifndef TESTS_HPP
#define TESTS_HPP

#include <boost/cstdint.hpp>
#include <string>
#include <QString>


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

#define DOUBLE_EQUALITY_BARRIER 10e-8
#define FLOAT_EQUALITY_BARRIER  10e-5

/**
 * @file
 * @ingroup tests
 * @attention Diese Testfunktionen können das Programm komplett in einen anderen
 *      Zustand überführen als es vor dem Aufruf der Funktion hatte
 */

/**
 * @ingroup tests
 * @brief Hier werden alle Funktionen abgelegt, die im weiteren Sinne mit Tests zu tun haben.
 * @todo Doxygen-Kommentare, weitere Tests
 */
namespace biker_tests
{
    /**
     * @brief Führt Tests am Programm durch und gibt zurück, ob sie erfolgreich waren.
     * 
     * 
     * @return Ob alle ausgeführten Tests erfolgreich waren.
     * @todo Implemetieren, wahrscheinlich immerzu.
     */
    int testProgram(std::string testName);

    int testBasename();
    int test_uint64_t2string();
    
    /**
     * @brief Konvertiert einen 64Bit-Int ohne Vorzeichen in einen String im 2er-System.
     * 
     * Für Testausgaben mit Bitzugriffen ganz brauchbar.
     * 
     * @return Den Wert als String im 2er-System
     */
    std::string uint64_t2string(boost::uint64_t integer);
    
    /**
     * @brief Gibt zu einem gegebenen Dateinamen den Basisnamen,
     *      ohne Pfadangabe, zurück.
     * 
     * Achtet auf Unterschiede zwischen unixoiden Dateiangaben und Windows.
     * 
     * @return 
     * @todo Tests!
     */
    std::string basename(std::string filename);


    template<typename S, typename T>
    bool check_equality(std::string message, S a, T b);
}

std::ostream& operator<<(std::ostream& os, const QString& qs);

#endif //TESTS_HPP 
