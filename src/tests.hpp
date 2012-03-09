#ifndef TESTS_HPP
#define TESTS_HPP

#include <boost/cstdint.hpp>
#include <string>
#include <QString>

/**
 * @defgroup tests Tests
 * @brief Hier werden alle Klassen und Funktionen und Dateien abgelegt,
 *      die für das Testen der Anwendung zuständig sind.
 * 
 * @subsection usage_testframework Verwendung des Testframeworks
 * 
 * Das Testframework besteht hauptsächlich aus den 3 Makros
 * <code>CHECK</code>, <code>CHECK_EQ</code> und <code>CHECK_EQ_TYPE</code>.
 * Eine Standard-Testfunktion besteht aus folgendem Methodenrumpf:
 * @code
 * #include "tests.hpp"
 * int testSomething()
 * {
 *     //Test an dieser Stelle infügen
 *     return EXIT_SUCCESS;
 * }
 * @endcode
 * Der eigentliche Test ist normaler C++-Code, der durch die Verwendung
 * der genannten Makros unterstützt wird. Die Makros haben gemein, dass
 * der Quellcode der Zeile, die überprüft wird, auf der Konsole ausgegeben
 * wird. Schlägt der Test fehl, wird jeweils
 * <code>return EXIT_FAILURE;</code> aufgerufen. Zusätzlich wird für jede
 * Zeile ausgegeben, ob der Test ohne Beanstandung war (<code>passed</code>),
 * oder fehlgeschlagen ist (<code>failed</code>). Ein Beispieltest sähe
 * folgendermaßen aus:
 * @code
 * #include "tests.hpp"
 * #include <boost/cstdint.hpp>
 * 
 * int testInteger()
 * {
 *     int var=0;
 *     CHECK(!var);
 *     CHECK_EQ(var, 0);
 *     var++;
 *     CHECK(var);
 *     CHECK_EQ(var, 1);
 *     
 *     boost::uint64_t var2 = 0;
 *     var2++;
 *     CHECK_EQ_TYPE(var, var2, boost::uint64_t);
 *     
 *     CHECK(false);
 *     
 *     return EXIT_SUCCESS;
 * }
 * @endcode
 * Der Code erzeugt folgende Ausgabe:
@verbatim
beispieltest.cpp:7: !var == true?                                                - passed!
beispieltest.cpp:8: var == 0?                                                    - passed!
beispieltest.cpp:10: var == true?                                                - passed!
beispieltest.cpp:11: var == 1?                                                   - passed!
beispieltest.cpp:15: var == var2?                                                - passed!
beispieltest.cpp:17: false == true?                                              - failed!
	Value A: 0
	Value B: 1
@endverbatim
 * <code>CHECK_EQ</code> und <code>CHECK_EQ_TYPE</code> unterscheiden sich
 * lediglich dadurch, dass <code>CHECK_EQ_TYPE</code> einen Cast auf den
 * angegebenen Typ durchführt, bevor der Vergleich ausgeführt wird. Dies
 * sorgt in einigen Fällen für mehr Übersichtlichkeit im Code, als auch -
 * und besonders -
 * in der Ausgabe der Testergebnisse.
 * 
 * Für die meisten primitiven Datentypen sind die Makros ohne besondere
 * Vorsicht verwendbar. Möchte man Zwei Typen vergleichen, die selbst
 * programmiert wurden, ist es nötig dafür einen Ausgabe- und
 * Vergleichsoperator zu schreiben, sowie eine Anpassung in
 * <code>tests.cpp</code> zu machen. Dort findet sich die template-Funktion
 * <code>check_equality</code>:
 * @code
 * template<typename S, typename T> bool check_equality(std::string message, S a, T b);
 * @endcode
 * Für sie muss eine Anpassung an den eigenen Typ eingefügt werden. Für den
 * eigenen Typ <code>EigenerTyp</code> sieht die Anpassung folgendermaßen aus:
 * @code
 * template bool check_equality(std::string message, EigenerTyp a, EigenerTyp b);
 * @endcode
 * Eine Implementierung ist nicht erforderlich, kann aber - so es nötig erscheint -
 * gemacht werden.
 * 
 * Ausgabe- und Vergleichsoperatoren können so definiert werden:
 * @code
 * bool operator==(const EigenerTyp& a, const EigenerTyp& b);
 * #include <iostream>
 * std::ostream operator<<(std::ostream& os, const EigenerTyp& a);
 * @endcode
 * Hier ist eine Implementierung noch nötig, sie wird wie üblich vorgenommen.
 * Die Definition der Funktionen muss der <code>tests.hpp</code> bekannt gemacht
 * werden durch
 * @code
 * #include "beispieltest.hpp"
 * @endcode
 * 
 * Damit die Testfunktion durch die ausführbare Datei aufgerufen werden kann,
 * muss in der Funktion <code>int testProgram(std::string testName)</code>
 * ein entsprechender Aufruf eingefügt werden:
 * @code
 * //...
 * else if (testName == "webserver")
 *     return biker_tests::testWebServer();
 * else if (testName == "potentialfunction")
 *     return biker_tests::testPotentialFunction();
 * //hier einfügen:
 * //else if (testName == "beispieltest")
 * //    return testInteger();
 * //...
 * @endcode
 * Es bietet sich an, die Testfunktionen im Namespace biker_tests abzulegen.
 * Dies ist nicht zwingend nötig.
 * 
 */

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

#define DOUBLE_EQUALITY_BARRIER 10e-7
#define FLOAT_EQUALITY_BARRIER  10e-5

/**
 * @file
 * @ingroup tests
 * @attention Diese Testfunktionen können das Programm komplett in einen anderen
 *      Zustand überführen als es vor dem Aufruf der Funktion hatte, daher sollten
 *      die Tests in einem extra dafür vorgesehenen Programmstart ausgeführt werden.
 */

/**
 * @ingroup tests
 * @brief Hier werden alle Funktionen abgelegt, die im weiteren Sinne mit Tests zu tun haben.
 */
namespace biker_tests
{
    /**
     * @brief Führt Tests am Programm durch und gibt zurück, ob sie erfolgreich waren.
     * 
     * 
     * @return Ob alle ausgeführten Tests erfolgreich waren.
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
