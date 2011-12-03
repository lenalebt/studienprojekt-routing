#include "osmproperty.hpp"
#include "tests.hpp"

std::ostream& operator<<(std::ostream& os, const OSMProperty& prop)
{
    os << "key: \"" << prop.getKey() << "\" value: \"" << prop.getValue() << "\"";
    return os;
}

std::size_t hash_value(QString const& b)
{
    return boost::hash_value(b.toStdString());
}

bool OSMProperty::operator==(const OSMProperty& prop) const
{
    if (this->key == prop.key)	//key gleicht: Voraussetzung für die meisten Sachen.
    {
        if (this->value == prop.value || this->value.get().isEmpty() || prop.value.get().isEmpty())
            return true;
        else
            return false;
    }
    else
        return false;
}

namespace biker_tests
{
    int testOSMProperty()
    {
        OSMProperty prop1("key", "value");
        OSMProperty prop2("key", "value");
        
        CHECK_EQ(prop1, prop2);
        CHECK_EQ_TYPE(prop1.getKey().toStdString(), "key", std::string);
        CHECK_EQ_TYPE(prop1.getValue().toStdString(), "value", std::string);
        
        prop1.setKey("key2");
        CHECK_EQ_TYPE(prop1.getKey().toStdString(), "key2", std::string);
        
        CHECK(!(prop1 == prop2));
        
        return EXIT_SUCCESS;
    }
} 
