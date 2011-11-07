#ifndef BLOCKINGQUEUE_HPP
#define BLOCKINGQUEUE_HPP

#include <QMutex>
#include <QQueue>

template <typename T>
class BlockingQueue : public QQueue
{
private:
    int _size;
public:
    BlockingQueue(int size) : _size(size) {}
    T dequeue ();
    void enqueue ( const T & t );
    T& head ();
    const T & head () const;
};


#endif //BLOCKINGQUEUE_HPP
