#include "blockingqueue.hpp"

template <typename T> 
bool BlockingQueue<T>::dequeue(T& t)
{
    QMutexLocker locker(&_mutex);
    
    if (isEmpty())
    {
        bool waitForElement=true;
        while (waitForElement)
        {
            notEmpty.wait(&_mutex);
            if (isEmpty())
            {
                if (!_queueDestroyed)
                {
                    
                }
                else
                {
                    return false;
                }
            }
            else
            {
                waitForElement = false;
            }
        }
    }
    else
    {
        t = _queue.dequeue();
        return true;
    }
    return false;
}

template <typename T> 
bool BlockingQueue<T>::enqueue(const T &t)
{
    QMutexLocker locker(&_mutex);
    return false;
}

template <typename T> 
bool BlockingQueue<T>::isEmpty()
{
    QMutexLocker locker(&_mutex);
    
    return _queue.isEmpty();
}

template <typename T> 
void BlockingQueue<T>::destroyQueue()
{
    QMutexLocker locker(&_mutex);
    _queueDestroyed = true;
    notFull.wakeAll();
    notEmpty.wakeAll();
}


namespace biker_tests
{
    int testBlockingQueue()
    {
        BlockingQueue<int> bQueue(10);
        return EXIT_FAILURE;
    }
}
