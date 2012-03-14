#include "bloomfilter.hpp"

template<typename T>
void Bloomfilter<T>::add(const T& element)
{
    for (int i=0; i<_degree; i++)
    {
        _bitarray[hashElement(element, i)] = true;
    }
}

template<typename T>
bool Bloomfilter<T>::contains(const T& element)
{
    for (int i=0; i<_degree; i++)
    {
        if (!_bitarray[hashElement(element, i)])
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
    //TODO
    return 0;
}
