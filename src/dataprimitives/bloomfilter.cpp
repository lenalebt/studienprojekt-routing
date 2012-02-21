#include "bloomfilter.hpp"
#include <boost/cstdint.hpp>
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>
#include <ctime>
#include <iostream>

template<typename T>
bool Bloomfilter<T>::add(const T& element)
{
    bool collision = true;
    for (int i=0; i<_degree; i++)
    {
        int index = hashElement(element, _size, i);
        collision &= _bitarray[index];
        _bitarray[index] = true;
    }
    return collision;
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
Bloomfilter<T>::Bloomfilter(boost::uint64_t size, int degree)
    : _bitarray(size), _size(size), _degree(degree)
{
    
}

template<typename T>
int hashElement(const T& element, boost::uint64_t size, int number)
{
    return ((element < 0 ? -element : element) + number) % size;
}

template int hashElement(const int& element, boost::uint64_t size, int number);
template class Bloomfilter<int>;
template int hashElement(const boost::uint64_t& element, boost::uint64_t size, int number);
template class Bloomfilter<boost::uint64_t>;
template int hashElement(const boost::int64_t& element, boost::uint64_t size, int number);
template class Bloomfilter<boost::int64_t>;

namespace biker_tests
{
    /**
     * @todo Bloomfiltertest ist noch leer
     */
    int testBloomfilter()
    {
        Bloomfilter<int> filter(104729, 1);
        
        boost::minstd_rand generator(static_cast<unsigned int>(std::time(0)));
        typedef boost::variate_generator<boost::minstd_rand&, boost::uniform_int<> > gen_type;
        gen_type dist(generator, boost::uniform_int<>(1, 5000000));
        /*boost::minstd_rand gen;
        boost::uniform_int<> dist(0, std::numeric_limits<int>::max());
        boost::variate_generator< boost::minstd_rand&, boost::uniform_int<> > 
            getrand(gen, dist);*/


        int collisions=0;
        for (int i=0; i<50000; i++)
        {
            if (filter.add(dist()))
            {
                std::cerr << i << ", ";
                collisions++;
            }
        }
        std::cerr << std::endl << collisions << " collisions." << std::endl;
        
        return EXIT_FAILURE;
    }
}
