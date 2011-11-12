#ifndef TESTS_HPP
#define TESTS_HPP

#include <boost/cstdint.hpp>
#include <boost/algorithm/string.hpp> 

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

    int testRoutingEdge();
    int testRoutingNode();
    int test_uint64_t2string();
    
    /**
     * @brief Konvertiert einen 64Bit-Int ohne Vorzeichen in einen String im 2er-System.
     * 
     * Für Testausgaben mit Bitzugriffen ganz brauchbar.
     * 
     * @return Den Wert als String im 2er-System
     */
    std::string uint64_t2string(boost::uint64_t integer);


    template<typename S, typename T>
    bool check_equality(std::string message, S a, T b);

}

#endif //TESTS_HPP 
