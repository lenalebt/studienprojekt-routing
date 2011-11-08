#ifndef CLOSEDLIST_HPP
#define CLOSEDLIST_HPP

#include <boost/cstdint.hpp>
#include <QSet>
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>

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
	virtual bool contains(boost::uint64_t nodeID, RoutingThread thread) const=0;
    
    /**
     * @brief Fügt einen Knoten zur Liste der endgültig betrachteten Knoten hinzu.
     * @param nodeID die ID des Knotens, der zur Liste der endgültig betrachteten Knoten hinzugefügt wird
     * @param thread Gibt an, welcher Thread versucht einen Knoten in der ClosedList abzulegen.
     */
	virtual void addNode(boost::uint64_t nodeID, RoutingThread thread)=0;
    /**
     * @brief Entfernt einen Knoten aus der Liste der endgültig betrachteten Knoten
     * @param nodeID die ID des Knotens, der aus Liste der endgültig betrachteten Knoten entfernt wird
     * @param thread Gibt an, welcher Thread versucht einen Knoten in der ClosedList abzulegen.
     */
	virtual void removeNode(boost::uint64_t nodeID, RoutingThread thread)=0;
    /**
     * @brief Gibt zurück, wie viele Knoten in der Liste der endgültig betrachteten Knoten vorhanden sind
     * @return wie viele Knoten in der Liste der endgültig betrachteten Knoten vorhanden sind
     */
	virtual int size(RoutingThread thread) const=0;
    
    /**
     * @brief Gibt zurück, welcher Knoten der ist, der zu einer Überlappung der Suchräume des
     *      Graphenalgorithmus führte.
     * @return 0, wenn keine Überlappung gefunden wurde. Die ID des überlappenden Knotens, sonst.
     * @param thread Gibt an, um welchen Thread es sich handelt.
     */
    virtual boost::uint64_t getOverlappingNode() const=0;
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
 * @todo Testen!
 */
class HashClosedList : public ClosedList
{
private:
    QSet<boost::uint64_t> _sSet;
    QSet<boost::uint64_t> _tSet;
    mutable QReadWriteLock _lock;
    boost::uint64_t _overlappingNode;
public:
    bool contains(boost::uint64_t nodeID, RoutingThread thread) const;
	void addNode(boost::uint64_t nodeID, RoutingThread thread);
	void removeNode(boost::uint64_t nodeID, RoutingThread thread);
	int size(RoutingThread thread) const;
    boost::uint64_t getOverlappingNode() const;
    
    HashClosedList() : _overlappingNode(0) {}
};

#endif //CLOSEDLIST_HPP 
