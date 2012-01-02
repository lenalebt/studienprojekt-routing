#include "blockingqueue.hpp"
#include <iostream>
#include <QtConcurrentRun>

template <typename T> 
bool BlockingQueue<T>::dequeue(T& t)
{
    QMutexLocker locker(&_mutex);
    
    bool waitForElement=true;
    while (waitForElement)
    {
        if (_elementCount <= 0)
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
        if (_elementCount >= _size)
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
        QFuture<int> thread1Result = QtConcurrent::run(biker_tests::testBlockingQueueSource<int>, &threadQueue);
        QFuture<int> thread2Result = QtConcurrent::run(biker_tests::testBlockingQueueSource<int>, &threadQueue);
        QFuture<int> thread3Result = QtConcurrent::run(biker_tests::testBlockingQueueSource<int>, &threadQueue);
        QFuture<int> thread4Result = QtConcurrent::run(biker_tests::testBlockingQueueDrain<int>, &threadQueue);
        
        
        CHECK_EQ(thread1Result.result(), EXIT_SUCCESS);
        CHECK_EQ(thread2Result.result(), EXIT_SUCCESS);
        CHECK_EQ(thread3Result.result(), EXIT_SUCCESS);
        std::cout << "there were " << thread4Result.result() <<
            " inserts." << std::endl;
        CHECK(thread4Result.result() >= 100000);
        
        return EXIT_SUCCESS;
    }
    
    template <typename T>
    int testBlockingQueueSource(BlockingQueue<T>* queue)
    {
        for (int i = 0; i < 100000; i++)
        {
            if (!queue->enqueue(T()))
                break;
        }
        queue->destroyQueue();
        
        return EXIT_SUCCESS;
    }
    template <typename T>
    int testBlockingQueueDrain(BlockingQueue<T>* queue)
    {
        int i=0;
        T a;
        bool goOn = true;
        while (goOn)
        {
            i++;
            goOn = queue->dequeue(a);
        }
        return i;
    }
}
