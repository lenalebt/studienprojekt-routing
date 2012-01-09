#ifndef HEAP_HPP
#define HEAP_HPP

#include <boost/shared_ptr.hpp>
#include <boost/cstdint.hpp>
#include <QVector>
#include <QHash>
#include <QHash>
#include "tests.hpp"
#include <iostream>

/**
 * @brief Dies ist ein Interface für einen Heap.
 * 
 * Man muss, damit es funktioniert, einen Funktor implementieren, der
 * zwei Elemente vergleicht. Dieser wird in der Klasse verwendet für Vergleiche.
 * Der Funktor muss einen operator() implementieren mit 2 Argumenten,
 * der bool zurückgibt. Die Argumente müssen den Typ T des Heaps haben:
 * 
 * bool operator()(T a, T b);
 * 
 * @ingroup routing
 * @author Lena Brueder
 * @date 2011-11-08
 * @copyright GNU GPL v3
 */
template<typename T, typename Less>
class Heap
{
public:
    /**
     * @brief Entfernt das kleinste Element aus dem Heap.
     * @return das kleinste Element aus dem Heap.
     */
    virtual T removeMinimumCostElement()=0;
    /**
     * @brief Fügt ein Element zum Heap hinzu.
     */
    virtual void add(T element)=0;
    /**
     * @brief Verringert den Key eines Elements im Heap.
     * @param element Das Element, für den der Key angepasst werden soll
     */
    virtual void decreaseKey(T element)=0;
    /**
     * @brief Gibt zurück, ob im Heap ein Element mit der angegebenen ID existiert.
     * @return ob im Heap dieses Element existiert
     */
    virtual bool contains(T element) const=0;
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
template<typename T, typename Less>
class BinaryHeap : public Heap<T, Less>
{
private:
	QVector<T> heap;
	QHash<boost::uint64_t, int> positionInHeap;	//nötig für decreaseKey
    Less *less;
    
	void letSink(int i)
	{
		int j=2*i+1;
		while (j<heap.size())	//"sinken lassen"
		{
			/* schau die Kinder von i an, welches ist kleiner?
			 * j ist das linke Kind, j+1 ist das rechte Kind.*/
			if ((j+1>=heap.size()) || (*less)(heap[j], heap[j+1]))
			{	//linkes Kind ist kleiner
                if ((*less)(heap[j], heap[i]))
                {
                    T tmpVal = heap[i];	//Tauschen
                    heap[i] = heap[j];
                    heap[j] = tmpVal;
                    positionInHeap[heap[i]] = i;
                    positionInHeap[heap[j]] = j;
                    i=j;	//und weiter, zum linken Kind
                }
                else
                    break;
			}
			else	//rechtes Kind ist kleiner
			{
                if ((*less)(heap[j+1], heap[i]))
                {
                    T tmpVal = heap[i];	//Tauschen
                    heap[i] = heap[j+1];
                    heap[j+1] = tmpVal;
                    positionInHeap[heap[i]] = i;
                    positionInHeap[heap[j+1]] = j+1;
                    i=j+1;	//weiter, zum rechten Kind
                }
                else
                    break;
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
			if ((*less)(heap[j], heap[i]))
			{	//Papa ist kleiner: fertig.
				return;
			}
			else	//Kind ist kleiner, tauschen!
			{
				T tmpVal = heap[i];	//Tauschen
				heap[i] = heap[j];
				heap[j] = tmpVal;
				positionInHeap[heap[i]] = i;
				positionInHeap[heap[j]] = j;
				i=j;	//weiter, zum Papa
			}
			j=(i-1)/2;
		}
	}
public:
	T removeMinimumCostElement()
	{
		//Rückgabewert herausnehmen
		T retVal = heap[0];
		heap[0] = heap.last();	//Wurzel durch letztes Element ersetzen
		heap.remove(heap.size()-1);		//und dieses am Ende wegnehmen
		positionInHeap.remove(retVal);
        
		if (!this->isEmpty())
		{
			positionInHeap[heap[0]] = 0;
			letSink(0);		//erstes Element sinken lassen
		}
		
		return retVal;
	}
	void add(T element)
	{
		heap << element;
		positionInHeap[element] = heap.size()-1;
		letAscend(heap.size()-1);	//letztes Element aufsteigen lassen
	}
    /**
     * @todo Verlässt sich darauf, dass der Key nur kleiner wurde, und sich nicht vergrößert hat.
     *      Das sollte verbessert werden!
     */
	void decreaseKey(T element)
	{
		if (positionInHeap.contains(element))
		{
//			int i = positionInHeap[element];
			letAscend(positionInHeap[element]);
		}
		else
		{
			add(element);
		}
	}
	bool isEmpty() const {return heap.isEmpty();}
	bool contains(T element) const {return positionInHeap.contains(element);}
	int size() const {return positionInHeap.size();}
    
    BinaryHeap(Less& lessFunctor) : less(&lessFunctor)
    {
        
    }
    
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
    template<typename T>
    class BinaryHeapTestLessFunctor
    {
    private:
        //LALA
    public:
        bool operator()(T a, T b)
        {
            return (a<b);
        }
    };
    template<typename K, typename V>
    class BinaryHeapTestLessAndQHashFunctor
    {
    private:
        QHash<K, V> hashMap;
    public:
        BinaryHeapTestLessAndQHashFunctor()
        {
            
        }
        bool operator()(K a, K b)
        {
            return (hashMap[a] < hashMap[b]);
        }
        void setValue(K key, V value)
        {
            hashMap.insert(key, value);
        }
        V getValue(K key)
        {
            return hashMap[key];
        }
    };
    int testBinaryHeap();
}
#endif //HEAP_HPP
