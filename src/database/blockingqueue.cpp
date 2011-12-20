#include "blockingqueue.hpp"

template <typename T> 
bool BlockingQueue<T>::dequeue(T& t)
{
    return false;
}

template <typename T> 
bool BlockingQueue<T>::enqueue(const T &t)
{
    return false;
}

template <typename T> 
bool BlockingQueue<T>::isEmpty()
{
    return true;
}

template <typename T> 
void BlockingQueue<T>::destroyQueue()
{
    
}


namespace biker_tests
{
    int testBlockingQueue()
    {
        return EXIT_FAILURE;
    }
}
