#ifndef CLOSEDLIST_HPP
#define CLOSEDLIST_HPP

#include <boost/cstdint.hpp>
#include <QSet>
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>
#include "tests.hpp"

/**
 * @brief Gibt an, welcher Thread einen Aufruf gestartet hat.
 * @ingroup routing
 */
enum RoutingThread
{
    /**
     * @brief Der Aufruf wurde vom Thread gestartet, der den Startpunkt ansieht.
     */
    S_THREAD,
    /**
     * @brief Der Aufruf wurde vom Thread gestartet, der das Ziel ansieht.
     */
    T_THREAD
};


/**
 * @brief In dieser Liste wird abgelegt, welche Knoten als endgültig fertig
 *      betrachtet angesehen werden.
 * 
 * Diese Klasse ist zentral wichtig beim Routing.
 * 
 * @ingroup routing
 * @author Lena Brueder
 * @date 2011-11-08
 * @copyright GNU GPL v3
 */
class ClosedList
{
public:
    /**
     * @brief Prüft, ob ein Knoten bereits endgültig betrachtet wurde
     * @return ob ein Knoten bereits endgültig betrachtet wurde
     */
	virtual bool contains(boost::uint64_t elementID) const=0;
    
    /**
     * @brief Fügt einen Knoten zur Liste der endgültig betrachteten Knoten hinzu.
     * @param elementID die ID des Knotens, der zur Liste der endgültig betrachteten Knoten hinzugefügt wird
     */
	virtual void addElement(boost::uint64_t elementID)=0;
    /**
     * @brief Entfernt einen Knoten aus der Liste der endgültig betrachteten Knoten
     * @param elementID die ID des Knotens, der aus Liste der endgültig betrachteten Knoten entfernt wird
     */
	virtual void removeElement(boost::uint64_t elementID)=0;
    /**
     * @brief Gibt zurück, wie viele Knoten in der Liste der endgültig betrachteten Knoten vorhanden sind
     * @return wie viele Knoten in der Liste der endgültig betrachteten Knoten vorhanden sind
     */
	virtual int size() const=0;
    
    /**
     * @brief Gibt zurück, welcher Knoten der ist, der zu einer Überlappung der Suchräume des
     *      Graphenalgorithmus führte.
     * @return 0, wenn keine Überlappung gefunden wurde. Die ID des überlappenden Knotens, sonst.
     */
    virtual boost::uint64_t getOverlappingElement() const=0;
};

/**
 * @brief Implementierung einer ClosedList als HashTable.
 * 
 * Benutzt zwei QSets als Speicher. Ist threadsicher. Ist mit einem
 * QReadWriteLock gesichert, erlaubt gleichzeitige Lesezugriffe.
 * 
 * @ingroup routing
 * @author Lena Brueder
 * @date 2011-11-08
 * @copyright GNU GPL v3
 */
class HashClosedList : public ClosedList
{
private:
    QSet<boost::uint64_t> _set;
    boost::uint64_t _overlappingElement;
public:
    bool contains(boost::uint64_t elementID) const
    {
        return _set.contains(elementID);
    }
	void addElement(boost::uint64_t elementID)
    {
        if (_set.contains(elementID))
        _overlappingElement = elementID;
        _set << elementID;
    }
    
	void removeElement(boost::uint64_t elementID)
    {
        _set.remove(elementID);
    }
	int size() const
    {
        return _set.size();
    }
    boost::uint64_t getOverlappingElement() const
    {
        return _overlappingElement;
    }
    
    HashClosedList() : _overlappingElement(0) {}
};



/**
 * @brief In dieser Liste wird abgelegt, welche Knoten als endgültig fertig
 *      betrachtet angesehen werden.
 * 
 * Diese Klasse ist zentral wichtig beim Routing.
 * 
 * @ingroup routing
 * @author Lena Brueder
 * @date 2011-11-08
 * @copyright GNU GPL v3
 * @todo Eventuell sollte die Klasse selbst merken, welcher Thread auf sie zugreift. Aktuell muss man es
 *      ihr über Parameter sagen.
 */
class MultiThreadedClosedList
{
public:
    /**
     * @brief Prüft, ob ein Knoten bereits endgültig betrachtet wurde
     * @return ob ein Knoten bereits endgültig betrachtet wurde
     */
	virtual bool contains(boost::uint64_t elementID, RoutingThread thread) const=0;
    
    /**
     * @brief Fügt einen Knoten zur Liste der endgültig betrachteten Knoten hinzu.
     * @param elementID die ID des Knotens, der zur Liste der endgültig betrachteten Knoten hinzugefügt wird
     * @param thread Gibt an, welcher Thread versucht einen Knoten in der ClosedList abzulegen.
     */
	virtual void addElement(boost::uint64_t elementID, RoutingThread thread)=0;
    /**
     * @brief Entfernt einen Knoten aus der Liste der endgültig betrachteten Knoten
     * @param elementID die ID des Knotens, der aus Liste der endgültig betrachteten Knoten entfernt wird
     * @param thread Gibt an, welcher Thread versucht einen Knoten in der ClosedList abzulegen.
     */
	virtual void removeElement(boost::uint64_t elementID, RoutingThread thread)=0;
    /**
     * @brief Gibt zurück, wie viele Knoten in der Liste der endgültig betrachteten Knoten vorhanden sind
     * @return wie viele Knoten in der Liste der endgültig betrachteten Knoten vorhanden sind
     * @param thread Gibt an, welcher Thread versucht einen Knoten in der ClosedList abzulegen.
     */
	virtual int size(RoutingThread thread) const=0;
    
    /**
     * @brief Gibt zurück, welcher Knoten der ist, der zu einer Überlappung der Suchräume des
     *      Graphenalgorithmus führte.
     * @return 0, wenn keine Überlappung gefunden wurde. Die ID des überlappenden Knotens, sonst.
     */
    virtual boost::uint64_t getOverlappingElement() const=0;
};

/**
 * @brief Implementierung einer ClosedList als HashTable.
 * 
 * Benutzt zwei QSets als Speicher. Ist threadsicher. Ist mit einem
 * QReadWriteLock gesichert, erlaubt gleichzeitige Lesezugriffe.
 * 
 * @ingroup routing
 * @author Lena Brueder
 * @date 2011-11-08
 * @copyright GNU GPL v3
 */
class MultiThreadedHashClosedList : public MultiThreadedClosedList
{
private:
    QSet<boost::uint64_t> _sSet;
    QSet<boost::uint64_t> _tSet;
    mutable QReadWriteLock _lock;
    boost::uint64_t _overlappingElement;
public:
    bool contains(boost::uint64_t elementID, RoutingThread thread) const
    {
        QReadLocker locker(&_lock);
        switch (thread)
        {
            case S_THREAD:
                return _sSet.contains(elementID);
                break;
            case T_THREAD:
                return _tSet.contains(elementID);
                break;
            default:
                return false;
        }
        return false;
    }
	void addElement(boost::uint64_t elementID, RoutingThread thread)
    {
        QWriteLocker locker(&_lock);
        switch (thread)
        {
            case S_THREAD:
                _sSet << elementID;
                if (_tSet.contains(elementID))
                    _overlappingElement = elementID;
                break;
            case T_THREAD:
                _tSet << elementID;
                if (_sSet.contains(elementID))
                    _overlappingElement = elementID;
                break;
            default:
                ;
        }
    }
	void removeElement(boost::uint64_t elementID, RoutingThread thread)
    {
        QWriteLocker locker(&_lock);
        switch (thread)
        {
            case S_THREAD:
                _sSet.remove(elementID);
                break;
            case T_THREAD:
                _tSet.remove(elementID);
                break;
            default:
                ;
        }
    }
	int size(RoutingThread thread) const
    {
        QReadLocker locker(&_lock);
        switch (thread)
        {
            case S_THREAD:
                return _sSet.size();
                break;
            case T_THREAD:
                return _tSet.size();
                break;
            default:
                return 0;
        }
        return 0;
    }
    boost::uint64_t getOverlappingElement() const
    {
        return _overlappingElement;
    }
    
    MultiThreadedHashClosedList() : _overlappingElement(0) {}
};

namespace biker_tests
{
    /**
     * @ingroup tests
     */
    int testHashClosedList();
    
    /**
     * @todo Test erweitern auf 2 Threads: momentan wird reine
     *      Funktion getestet, nichts mit mehreren Threads.
     * @ingroup tests
     */
    int testMultiThreadedHashClosedList();
}

#endif //CLOSEDLIST_HPP 
