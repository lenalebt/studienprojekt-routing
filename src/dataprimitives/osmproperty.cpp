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
        return EXIT_SUCCESS;
    }
} 
