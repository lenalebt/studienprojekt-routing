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
 * @todo Bei Doxygen-see-Kommentaren: Wie macht man da Links?
 * @ingroup database
 */

template <typename T>
class BlockingQueue
{
private:
    int _size;
    QQueue<T> _queue;
    QMutex _mutex;
    QWaitCondition notFull;
    QWaitCondition notEmpty;
    bool _queueDestroyed;
public:
    /**
     * @brief Initialisiert die Queue mit der Kapazität <code>size</code>
     * @param size Bestimmt die Kapazität der Queue, in Anzahl Elementen.
     */
    BlockingQueue(int size) : _size(size), _queue(QQueue<T>()),
        _mutex(), notFull(), notEmpty(), _queueDestroyed(false)
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
     * @param[out] t In dieser Variable wird das Element abgelegt, das
     *      aus der Queue entfernt wurde, so es eines gab.
     * @return <code>true</code>, wenn das Entfernen aus der Queue erfolgreich war,
     *      <code>false</code> sonst.
     * @see destroyQueue()
     * @todo Implementierung fehlt
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
     * @todo Implementierung fehlt
     */
    bool enqueue ( const T & t );
    /**
     * @brief Gibt an, ob die Queue aktuell leer ist.
     * 
     * Wenn die Queue zerstört wurde, gibt die Funktion
     * <code>true</code> zurück.
     * 
     * @remarks Diese Funktion ist bei Zugriffen auf Threads evtl. wertlos.
     * @return Ob die Queue aktuell leer ist.
     * @see destroyQueue()
     */
    bool isEmpty();
    /**
     * @brief Zerstört die Queue und sorgt dafür, dass alle
     *  blockierenden Zugriffe abgebrochen werden.
     */
    void destroyQueue();
};


namespace biker_tests
{
    /**
     * @todo Implementieren
     */
    int testBlockingQueue();
}

#endif //BLOCKINGQUEUE_HPP
