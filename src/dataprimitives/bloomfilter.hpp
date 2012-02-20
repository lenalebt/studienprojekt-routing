#ifndef BLOOMFILTER_HPP
#define BLOOMFILTER_HPP

#include <QBitArray>

template<typename T>
class Bloomfilter
{
private:
    QBitArray _bitarray;
    int _size;
    int _degree;
public:
    Bloomfilter(int size, int degree);
    bool contains(const T& element);
    void add(const T& element);
};

template <typename T>
int hashElement(const T& element, int size, int number);

#endif //BLOOMFILTER_HPP
