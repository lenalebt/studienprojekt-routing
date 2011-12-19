#ifndef OSMPROPERTY_HPP
#define OSMPROPERTY_HPP

#include <boost/flyweight.hpp>
#include <QString>

std::size_t hash_value(QString const& b);

/**
 * @brief OSMProperty speichert eine Eigenschaft (->Tag) eines Objektes im OSM-Datenmodell.
 * 
 * OSMProperty benutzt das <code>flyweight</code>-Pattern, um Speicher zu sparen:
 * Viele Tags werden mehrfach im Programm verwendet - es reicht, sie ein Mal im
 * Speicher zu halten. <code>boost::flyweight<T></code> übernimmt dies
 * automatisch.
 * 
 * @ingroup dataprimitives
 * @author Lena Brüder
 * 
 * @copyright Lizenz: GNU GPL v3
 */

class OSMProperty
{
private:
    boost::flyweight<QString> key;
    boost::flyweight<QString> value;
    
public:
    /**
     * @brief Erzeugt ein OSMProperty-Objekt mit leerem <code>key</code>
     *  und <code>value</code>.
     */
    OSMProperty() : key(""), value("") {}
    /**
     * @brief Erzeugt ein OSMProperty mit den entsprechend übergebenen
     *  Werten für <code>key</code> und <code>value</code>.
     * @param key Der Key der Eigenschaft
     * @param value Der Wert der Eigenschaft
     * 
     */
    OSMProperty(QString key, QString value) : key(key), value(value) {}
    
    /**
     * @brief Gibt den <code>key</code> der Eigenschaft zurück.
     * @return den <code>key</code>
     */
    QString getKey() const {return key;}
    /**
     * @brief Gibt die <code>value</code> der Eigenschaft zurück.
     * @return die <code>value</code>
     */
    QString getValue() const {return value;}
    
    /**
     * @brief Setzt den <code>key</code> der Eigenschaft auf den entsprechenden Wert
     * @param key der <code>key</code>
     */
    void setKey(const QString key) {this->key = key;}
    /**
     * @brief Setzt die <code>value</code> der Eigenschaft auf den entsprechenden Wert
     * @param value die <code>value</code>
     */
    void setValue(const QString value) {this->value = value;}
    
    //war wichtig für OSMPropertyTree
    //bool containsWildcards() const;
    
    /**
     * @brief Vergleicht zwei OSMProterys auf Gleichheit anhand von
     *  <code>key</code> und <code>value</code>.
     * @param prop Eine andere OSMProperty.
     */
    bool operator==(const OSMProperty& prop) const;
};

std::ostream& operator<<(std::ostream& os, const OSMProperty& prop);


namespace biker_tests
{
    /**
     * @todo Implementieren, dieser Test ist noch leer.
     */
    int testOSMProperty();
} 


#endif // OSMPROPERTY_HPP
