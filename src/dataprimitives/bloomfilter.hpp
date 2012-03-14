#ifndef BLOOMFILTER_HPP
#define BLOOMFILTER_HPP

#include <QBitArray>
#include "tests.hpp"

/**
 * @brief Diese Klasse implementiert einn klassischen Bloomfilter.
 * 
 * Ein Bloomfilter ist eine Datenstruktur, die ähnlich wie eine Menge
 * benutzt wird. Es ist möglich, Elemente hinzuzufügen und festzustellen,
 * ob ein Element enthalten ist, oder nicht. Dabei irrt sich die Datenstruktur
 * manchmal: Es kann passieren, dass die Datenstruktur meint, ein Element
 * sei enthalten, obwohl dies nicht der Fall ist. Andersherum ist dies nicht
 * möglich, wenn die Datenstruktur sagt, ein Element ist nicht enthalten, dann
 * ist dies auch sicher nicht der Fall.
 * 
 * Hauptvorteil des Bloomfilters gegenüber einer klassischen Menge, die über
 * Hashlisten implementiert wird, ist der wesentlich geringere Speicherbedarf.
 * 
 * Die Datenstruktur wird bei der Datenvorverarbeitung verwendet, um
 * festzustellen ob es nötig ist, die Datenbank über das Vorhandensein
 * von bestimmten Elementen zu befragen, oder nicht. Eine klassische
 * Hashliste verbraucht zu viel Speicher, und ohne eine Beschleunigung ist
 * das Verfahren viel zu langsam.
 * 
 * @author Lena Brüder
 * @date 2012-02-20
 * @copyright GNU GPL v3
 * @ingroup dataprimitives
 */
template<typename T>
class Bloomfilter
{
private:
    QBitArray _bitarray;
    boost::uint64_t _size;
    int _degree;
public:
    /**
     * @brief Erstellt einen neuen Bloomfilter mit <code>size</code> bits
     *      Breite und <code>degree</code> bits in der Hashfunktion.
     */
    Bloomfilter(boost::uint64_t size, int degree);
    /**
     * @brief Stellt fest, ob sich ein Element in dem Bloomfilter befindet.
     * 
     * Die Rückgabe von <code>true</code> bedeutet nur, dass das Element
     * möglicherweise im Filter vorhanden ist. Dies ist nicht mit Sicherheit
     * feststellbar. <code>false</code> bedeutet aber in jedem Falle, dass das
     * Element nicht vorhanden ist.
     * 
     * @return <code>false</code>, wenn sich das Element nicht im Filter befindet.
     *      <code>true</code>, wenn das Element möglicherweise im Filter ist.
     */
    bool contains(const T& element);
    /**
     * @brief Fügt ein Element zum Bloomfilter hinzu.
     * 
     * @return Ob eine Kollision beim Einfügen auftrat, oder nicht
     */
    bool add(const T& element);
};

template <typename T>
int hashElement(const T& element, boost::uint64_t size, int number);

namespace biker_tests
{
    int testBloomfilter();
}

#endif //BLOOMFILTER_HPP
