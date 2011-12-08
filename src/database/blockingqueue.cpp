#include "blockingqueue.hpp"

template <typename T> 
BlockingQueueElement<T> BlockingQueue<T>::dequeue()
{
    return BlockingQueueElement<T>();
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


