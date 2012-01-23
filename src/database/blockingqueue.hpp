#ifndef BLOCKINGQUEUE_HPP
#define BLOCKINGQUEUE_HPP

#include <QMutex>
#include <QQueue>
#include <QWaitCondition>
#include "tests.hpp"

/**
 * @brief Implementierung einer blockierenden, threadsicheren Queue mit
 *      beschränkter Kapazität
 * 
 * Die Queue blockiert, wenn sie voll ist und ein Element hinzugefügt werden
 * soll - ebenso, wenn sie leer ist und eines entfernt werden soll. Wenn
 * die Queue in einem Zustand ist, in dem keine weiteren Elemente mehr
 * hinzugefügt werden sollen, wird sie zerstört (->destroyQueue()). Mit Aufruf
 * dieser Funktion werden alle eventuell auf neue Elemente wartenden Threads
 * geweckt. Die Queue kann noch geleert werden, so sie noch Elemente enthält -
 * es können aber keine Elemente mehr hinzugefügt werden.
 * 
 * @author Lena Brüder
 * @date 2011-11-07
 * @copyright GNU GPL v3
 * @ingroup database
 */

template <typename T>
class BlockingQueue
{
private:
    int _size;
    int _elementCount;
    QQueue<T> _queue;
    QMutex _mutex;
    QWaitCondition _notFull;
    QWaitCondition _notEmpty;
    bool _queueDestroyed;
    
    BlockingQueue(const BlockingQueue<T>& queue) :
        _size(queue._size), _elementCount(queue._elementCount),
        _queue(queue._queue), _mutex(), _notFull(),
         _notEmpty(), _queueDestroyed(queue._queueDestroyed)
    {
        
    }
    
public:
    /**
     * @brief Initialisiert die Queue mit der Kapazität <code>size</code>
     * @param size Bestimmt die Kapazität der Queue, in Anzahl Elementen.
     */
    BlockingQueue(int size) : _size(size), _elementCount(0),
        _queue(), _mutex(), _notFull(),
         _notEmpty(), _queueDestroyed(false)
    {
        
    }
    
    /**
     * @brief Entfernt ein Element aus der Queue.
     * 
     * Blockiert, wenn kein Element vorhanden ist. Gibt <code>false</code>
     * zurück, wenn die Queue zerstört wurde und kein Element mehr vorhanden
     * ist - in diesem Fall kann nie mehr
     * ein Element zurückgegeben werden.
     * 
     * Um ein Element aus der Queue zu entfernen, wird sie folgendermaßen benutzt:
     * @code
     * BlockingQueue<int> queue;
     * int a = 5;
     * queue.enqueue(3);
     * queue.enqueue(5);
     * queue.enqueue(7);
     * 
     * while(queue.dequeue(a))
     * {
     *      std::cout << a << " ";
     * }
     * //Ausgabe: 3 5 7
     * @endcode
     * 
     * @param[out] t In dieser Variable wird das Element abgelegt, das
     *      aus der Queue entfernt wurde, so es eines gab.
     * @return <code>true</code>, wenn das Entfernen aus der Queue erfolgreich war,
     *      <code>false</code> sonst.
     * @see destroyQueue()
     */
    bool dequeue (T& t);
    /**
     * @brief Fügt ein Element zur Queue hinzu.
     * 
     * Blockiert, wenn die Queue bereits voll ist.
     * Wenn es nicht möglich war das Element hinzuzufügen,
     * wird <code>false</code> zurückgegeben. Dies ist der Fall,
     * wenn die Queue zerstört wurde.
     * 
     * @param t Das Element, das zur Queue hinzugefügt werden soll
     * @return Ob das Hinzufügen erfolgreich war
     * @see destroyQueue()
     */
    bool enqueue ( const T & t );
    /**
     * @brief Gibt an, ob die Queue aktuell leer ist.
     * 
     * @remarks Diese Funktion ist bei Zugriffen auf Threads evtl. wertlos.
     * @return Ob die Queue aktuell leer ist.
     * @see destroyQueue()
     */
    bool isEmpty();
    /**
     * @brief Gibt an, ob die Queue aktuell voll ist.
     * 
     * Wenn die Queue zerstört wurde, gibt die Funktion
     * <code>true</code> zurück.
     * 
     * @remarks Diese Funktion ist bei Zugriffen auf Threads evtl. wertlos.
     * @return Ob die Queue aktuell leer ist.
     * @see destroyQueue()
     */
    bool isFull();
    /**
     * @brief Zerstört die Queue und sorgt dafür, dass alle
     *  blockierenden Zugriffe abgebrochen werden.
     */
    void destroyQueue();
    /**
     * @brief Gibt an, ob die Queue zerstört wurde, oder nicht.
     * @return Ob die Queue zerstört wurde.
     */
    bool isDestroyed();
    
    BlockingQueue<T>& operator<<(const T& element);
    T& operator>>(T& element);
};


namespace biker_tests
{
    /**
     * @ingroup tests
     */
    int testBlockingQueue();
    /**
     * @ingroup tests
     */
    template <typename T> int testBlockingQueueSource(BlockingQueue<T>* queue);
    /**
     * @ingroup tests
     */
    template <typename T> int testBlockingQueueDrain(BlockingQueue<T>* queue);
}

#endif //BLOCKINGQUEUE_HPP
