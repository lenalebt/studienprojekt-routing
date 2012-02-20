#include "bloomfilter.hpp"
#include <boost/cstdint.hpp>

template<typename T>
void Bloomfilter<T>::add(const T& element)
{
    for (int i=0; i<_degree; i++)
    {
        _bitarray[hashElement(element, _size, i)] = true;
    }
}

template<typename T>
bool Bloomfilter<T>::contains(const T& element)
{
    for (int i=0; i<_degree; i++)
    {
        if (!_bitarray[hashElement(element, _size, i)])
            return false;
    }
    return true;
}

template<typename T>
Bloomfilter<T>::Bloomfilter(int size, int degree)
    : _bitarray(size), _size(size), _degree(degree)
{
    
}

template<typename T>
int hashElement(const T& element, int size, int number)
{
    return (element + number) % size;
}

template int hashElement(const int& element, int size, int number);
template class Bloomfilter<int>;
template int hashElement(const boost::uint64_t& element, int size, int number);
template class Bloomfilter<boost::uint64_t>;
template int hashElement(const boost::int64_t& element, int size, int number);
template class Bloomfilter<boost::int64_t>;

namespace biker_tests
{
    /**
     * @todo Bloomfiltertest ist noch leer
     */
    int testBloomfilter()
    {
        return EXIT_FAILURE;
    }
}
