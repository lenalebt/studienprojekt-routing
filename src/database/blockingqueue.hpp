#ifndef BLOCKINGQUEUE_HPP
#define BLOCKINGQUEUE_HPP

#include <QMutex>
#include <QQueue>
#include "tests.hpp"


/**
 * @brief Dient als Wrapperelement für die BlockingQueue.
 * 
 * Wenn dequeue() auf einer BlockingQueue aufgerufen wird kann es passieren,
 * dass die Queue leer ist und in Zukunft keine weiteren Elemente hinzugefügt
 * werden. In diesem Fall wird ein leeres BlockingQueueElement zurückgegeben
 * (<code>isEmpty()==true</code>).
 * 
 * @author Lena Brüder
 * @date 2011-11-07
 * @copyright GNU GPL v3
 * @ingroup database
 */
template <typename T>
class BlockingQueueElement
{
private:
    bool _empty;
    T _element;
public:
    /**
     * @brief Erzeugt ein leeres BlockingQueueElement
     */
    BlockingQueueElement() : _empty(true) {}
    /**
     * @brief Erzeugt ein BlockingQueueElement, dass <code>element</code>
     *      als Element enthält.
     * @param element Das Element, was das BlockingQueueElement enthält.
     */
    BlockingQueueElement(const T& element) : _empty(false), _element(element) {}
    /**
     * @brief Gibt das im BlockingQueueElement enthaltene Element zurück.
     * 
     * Der Wert ist ungültig, wenn <code>isEmpty()==true</code> gilt.
     * 
     * @return Das enthaltene Element
     */
    T get() {return _element;}
    /**
     * @brief Gibt an, ob im BlockingQueueElement ein Element enthalten ist.
     * @return Ob ein Element enthalten ist.
     */
    bool isEmpty() {return _empty;}
};

/**
 * @brief Implementierung einer blockierenden, threadsicheren Queue mit
 *      beschränkter Kapazität
 * 
 * 
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
public:
    /**
     * @brief Initialisiert die Queue mit der Kapazität <code>size</code>
     * @param size Bestimmt die Kapazität der Queue
     */
    BlockingQueue(int size) : _size(size) {}
    
    /**
     * @brief Entfernt ein Element aus der Queue.
     * 
     * Blockiert, wenn kein Element vorhanden ist. Gibt ein leeres
     * BlockingQueueElement
     * zurück, wenn die Queue zerstört wurde (-> destroyQueue()).
     * 
     * @return Das Element am Anfang der Queue
     * @todo Implementierung fehlt
     */
    BlockingQueueElement<T> dequeue ();
    /**
     * @brief Fügt ein Element zur Queue hinzu.
     * 
     * Blockiert, wenn die Queue bereits voll ist.
     * Wenn es nicht möglich war das Element hinzuzufügen,
     * wird <code>false</code> zurückgegeben.
     * 
     * @param t Das Element, das zur Queue hinzugefügt werden soll
     * @return Ob das Hinzufügen erfolgreich war
     * @see destroyQueue()
     * @todo Implementierung fehlt
     */
    bool enqueue ( const T & t );
    /**
     * @brief Gibt an, ob die Queue aktuell leer ist.
     * @remarks Diese Funktion ist bei Zugriffen auf Threads evtl. wertlos.
     * @return Ob die Queue aktuell leer ist.
     * @see destroyQueue()
     * @todo Implementierung fehlt
     */
    bool isEmpty();
    /**
     * @brief Zerstört die Queue und sorgt dafür, dass alle
     *  blockierenden Zugriffe abgebrochen werden.
     * @todo Implementierung fehlt
     */
    void destroyQueue();
};

namespace biker_tests
{
    int testBlockingQueue();
}

#endif //BLOCKINGQUEUE_HPP
