#ifndef CLOSEDLIST_HPP
#define CLOSEDLIST_HPP

#include <boost/cstdint.hpp>
#include <QSet>

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
	virtual bool contains(boost::uint64_t nodeID)=0;
    
    /**
     * @brief Fügt einen Knoten zur Liste der endgültig betrachteten Knoten hinzu.
     * @param nodeID die ID des Knotens, der zur Liste der endgültig betrachteten Knoten hinzugefügt wird
     */
	virtual void addNode(boost::uint64_t nodeID)=0;
    /**
     * @brief Entfernt einen Knoten aus der Liste der endgültig betrachteten Knoten
     * @param nodeID die ID des Knotens, der aus Liste der endgültig betrachteten Knoten entfernt wird
     */
	virtual void removeNode(boost::uint64_t nodeID)=0;
    /**
     * @brief Gibt zurück, wie viele Knoten in der Liste der endgültig betrachteten Knoten vorhanden sind
     * @return wie viele Knoten in der Liste der endgültig betrachteten Knoten vorhanden sind
     */
	virtual int size()=0;
};

/**
 * @brief Implementierung einer ClosedList als HashTable.
 * 
 * Diese Implementierung verwendet ein QSet.
 * 
 * @ingroup routing
 * @author Lena Brueder
 * @date 2011-11-08
 * @copyright GNU GPL v3
 */
class HashClosedList : public ClosedList
{
private:
	QSet<boost::uint64_t> hashtable;
public:
	bool contains(boost::uint64_t nodeID);
	void addNode(boost::uint64_t nodeID);
	void removeNode(boost::uint64_t nodeID);
	int size();
};

#endif //CLOSEDLIST_HPP 
