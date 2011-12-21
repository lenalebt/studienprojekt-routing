#include "blockingqueue.hpp"
#include <iostream>

template <typename T> 
bool BlockingQueue<T>::dequeue(T& t)
{
    QMutexLocker locker(&_mutex);
    
    bool waitForElement=true;
    while (waitForElement)
    {
        if (isEmpty())
        {
            if (!_queueDestroyed)
            {
                _notEmpty.wait(&_mutex);
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
    
    _elementCount--;
    t = _queue.dequeue();
    _notFull.wakeAll();
    return true;
}

template <typename T> 
bool BlockingQueue<T>::enqueue(const T &t)
{
    QMutexLocker locker(&_mutex);
    
    if (_queueDestroyed)
        return false;
    
    bool waitForSpace=true;
    while (waitForSpace)
    {
        if (isFull())
        {
            if (!_queueDestroyed)
            {
                _notFull.wait(&_mutex);
            }
            else
            {
                return false;
            }
        }
        else
        {
            waitForSpace = false;
        }
    }
    
    _elementCount++;
    _queue.enqueue(t);
    _notEmpty.wakeAll();
    return true;
}

template <typename T> 
bool BlockingQueue<T>::isEmpty()
{
    QMutexLocker locker(&_mutex);
    
    return _elementCount <= 0;
}

template <typename T> 
bool BlockingQueue<T>::isFull()
{
    QMutexLocker locker(&_mutex);
    
    return _elementCount >= _size;
}

template <typename T> 
void BlockingQueue<T>::destroyQueue()
{
    QMutexLocker locker(&_mutex);
    _queueDestroyed = true;
    _notFull.wakeAll();
    _notEmpty.wakeAll();
}

template <typename T> 
bool BlockingQueue<T>::isDestroyed()
{
    QMutexLocker locker(&_mutex);
    return _queueDestroyed;
}

template <typename T> 
BlockingQueue<T>& BlockingQueue<T>::operator<<(const T& element)
{
    this->enqueue(element);
    return *this;
}
template <typename T> 
T& BlockingQueue<T>::operator>>(T& element)
{
    this->dequeue(element);
    return element;
}

namespace biker_tests
{
    int testBlockingQueue()
    {
        //Zuerst nur ein bisschen hin und her, was die normale Queue schon können sollte...
        BlockingQueue<int> bQueue(5);
        CHECK(bQueue.isEmpty());
        CHECK(!bQueue.isFull());
        CHECK(!bQueue.isDestroyed());
        
        CHECK(bQueue.enqueue(5));
        CHECK(!bQueue.isEmpty());
        CHECK(!bQueue.isFull());
        CHECK(!bQueue.isDestroyed());
        
        bQueue << 9;
        
        int a;
        bQueue >> a;
        CHECK_EQ(a, 5);
        CHECK(bQueue.dequeue(a));
        CHECK_EQ(a, 9);
        CHECK(bQueue.isEmpty());
        CHECK(!bQueue.isFull());
        CHECK(!bQueue.isDestroyed());
        
        CHECK(bQueue.enqueue(7));
        
        bQueue.destroyQueue();
        CHECK(bQueue.isDestroyed());
        //in eine zerstörte Queue sollte nichts mehr eingefügt werden können
        CHECK(!bQueue.enqueue(6));
        
        //aber entfernen sollte man noch können, was drin ist...
        CHECK(bQueue.dequeue(a));
        CHECK_EQ(a, 7);
        
        //jetzt ist alles raus: nichts mehr entfernbar...
        CHECK(!bQueue.dequeue(a));
        //Der Wert darf sich nicht geändert haben...
        CHECK_EQ(a, 7);
        
        //TODO: Test mit mehreren Threads fehlt noch.
        BlockingQueue<int> threadQueue(10);
        //QtConcurrent::run();
        //Funktionen unten ausführen mit mehreren Threads, die die Queue füllen und mehreren,
        //die sie leeren.
        
        CHECK(false);
        
        return EXIT_SUCCESS;
    }
    
    template <typename T>
    int testBlockingQueueSource(BlockingQueue<T>& queue)
    {
        for (int i = 0; i < 50000; i++)
        {
            if (!queue.enqueue(T()))
                break;
        }
        queue.destroyQueue();
        
        return EXIT_SUCCESS;
    }
    template <typename T>
    int testBlockingQueueDrain(BlockingQueue<T>& queue)
    {
        T a;
        while (queue.dequeue(a))
        {
            
        }
        return EXIT_SUCCESS;
    }
}
