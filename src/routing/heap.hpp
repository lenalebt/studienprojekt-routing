#ifndef HEAP_HPP
#define HEAP_HPP

#include <boost/shared_ptr.hpp>
#include <boost/cstdint.hpp>
#include <QVector>
#include <QHash>
#include "tests.hpp"

/**
 * @brief Dies ist ein Interface für einen Heap.
 * 
 * Damit es funktioniert, muss der Datentyp T einen operator< unterstützen,
 * sowie eine Funktion boost::uint64_t getID() const.
 * 
 * Die Idee ist folgende: Man implementiert eine neue Klasse, die die
 * zu speichernden Eigenschaften enthält. Dazu gehört z.B. der Knoten selbst
 * (wahrscheinlich als Integerwert),
 * die bisherigen
 * Kosten, der Vorgängerzeiger (wahrscheinlich als Integer),
 * und im Falle von A* die heuristischen Kosten.
 * Man implementiert operator<, sodass der gewünschte Wert verglichen wird: Im
 * Falle von Dijkstra sind es die Kosten, im Falle von A* die heuristischen Kosten.
 * 
 * @ingroup routing
 * @author Lena Brueder
 * @date 2011-11-08
 * @copyright GNU GPL v3
 * @todo Evtl. besser statt operator< einen less-Funktor verwenden?
 */
template<typename T>
class Heap
{
public:
    /**
     * @brief Entfernt das kleinste Element aus dem Heap.
     * @return das kleinste Element aus dem Heap.
     */
    virtual boost::shared_ptr<T> removeMinimumCostNode()=0;
    /**
     * @brief Fügt ein Element zum Heap hinzu.
     */
    virtual void addNode(boost::shared_ptr<T> node)=0;
    /**
     * @brief Verringert den Key eines Elements im Heap.
     * @param nodeID die ID des Knotens, für den der Key angepasst werden soll
     */
    virtual void decreaseKey(boost::shared_ptr<T> node)=0;
    /**
     * @brief Gibt zurück, ob im Heap ein Element mit der angegebenen ID existiert.
     * @return ob im Heap ein Element mit der angegebenen ID existiert
     */
    virtual bool contains(boost::uint64_t nodeID) const=0;
    /**
     * @brief Gibt an, ob der Heap leer ist
     * @return ob der Heap leer ist
     */
    virtual bool isEmpty() const=0;
    /**
     * @brief Gibt die Anzahl der im Heap gespeicherten Elemente zurück.
     * @return die Anzahl der im Heap gespeicherten Elemente
     */
    virtual int size() const=0;
    virtual ~Heap() {}
};

/**
 * @brief Implementierung eines Heaps als Binärer Heap mit Feldern.
 * 
 * 
 * 
 * @ingroup routing
 * @author Lena Brueder
 * @date 2011-11-08
 * @copyright GNU GPL v3
 */
template<typename T>
class BinaryHeap : public Heap<T>
{
private:
	QVector<boost::shared_ptr<T> > heap;
	QHash<boost::uint64_t, int> positionInHeap;	//nötig für decreaseKey
	void letSink(int i)
	{
		int j=2*i+1;
		while (j<heap.size())	//"sinken lassen"
		{
			/* schau die Kinder von i an, welches ist kleiner?
			 * j ist das linke Kind, j+1 ist das rechte Kind.*/
			if ((j+1>=heap.size()) || (*(heap[j]) < *(heap[j+1])))
			{	//linkes Kind ist kleiner
				boost::shared_ptr<T> tmpVal = heap[i];	//Tauschen
				heap[i] = heap[j];
				heap[j] = tmpVal;
				positionInHeap[heap[i]->getID()] = i;
				positionInHeap[heap[j]->getID()] = j;
				i=j;	//und weiter, zum linken Kind
			}
			else	//rechtes Kind ist kleiner
			{
				boost::shared_ptr<T> tmpVal = heap[i];	//Tauschen
				heap[i] = heap[j+1];
				heap[j+1] = tmpVal;
				positionInHeap[heap[i]->getID()] = i;
				positionInHeap[heap[j+1]->getID()] = j+1;
				i=j+1;	//weiter, zum rechten Kind
			}
			j=2*i+1;
		}
	}
	void letAscend(int i)
	{
		int j=(i-1)/2;
		while (i != 0)	//steigen lassen...
		{
			/* Papa von i ansehen: Ist der kleiner? Wenn nein, tauschen.
			 * Wenn ja, fertig.*/
			if ((j>=heap.size()) || (*(heap[j]) < *(heap[i])))
			{	//Papa Kind ist kleiner: fertig.
				break;
			}
			else	//Kind ist kleiner, tauschen!
			{
				boost::shared_ptr<T> tmpVal = heap[i];	//Tauschen
				heap[i] = heap[j];
				heap[j] = tmpVal;
				positionInHeap[heap[i]->getID()] = i;
				positionInHeap[heap[j]->getID()] = j;
				i=j;	//weiter, zum Papa
			}
			j=(i-1)/2;
		}
	}
public:
	boost::shared_ptr<T> removeMinimumCostNode()
	{
		//Rückgabewert herausnehmen
		boost::shared_ptr<T> retVal = heap[0];
		heap[0] = heap.last();	//Wurzel durch letztes Element ersetzen
		heap.removeLast();		//und dieses am Ende wegnehmen
		positionInHeap.remove(retVal->getID());
		
		if (!this->isEmpty())
		{
			positionInHeap[heap[0]->getID()] = 0;
			letSink(0);		//erstes Element sinken lassen
		}
		
		return retVal;
	}
	void addNode(boost::shared_ptr<T> node)
	{
		heap << node;
		positionInHeap[node->getID()] = heap.size()-1;
		letAscend(heap.size()-1);	//letztes Element aufsteigen lassen
	}
    /**
     * @todo Verlässt sich darauf, dass der Key nur kleiner wurde, und sich nicht vergrößert hat.
     *      Das sollte verbessert werden!
     */
	void decreaseKey(boost::shared_ptr<T> node)
	{
		if (positionInHeap.contains(node->getID()))
		{
//			int i = positionInHeap[node->getID()];
			letAscend(positionInHeap[node->getID()]);
		}
		else
		{
			addNode(node);
		}
	}
	bool isEmpty() const {return heap.isEmpty();}
	bool contains(boost::uint64_t nodeID) const {return positionInHeap.contains(nodeID);}
	int size() const {return positionInHeap.size();}
    
    ~BinaryHeap()
    {
        /*for (QListIterator<boost::shared_ptr<T> > it(heap); it.hasNext(); )
        {
            it.next().reset();
        }*/
        heap.clear();
    }
};

namespace biker_tests
{
    /**
     * @todo Implementieren
     */
    int testBinaryHeap();
}
#endif //HEAP_HPP
