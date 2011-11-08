#ifndef HEAP_HPP
#define HEAP_HPP

#include <boost/shared_ptr.hpp>
#include <boost/cstdint.hpp>

/**
 * @brief Diese Klasse ist das Interface für einen Heap.
 * 
 * 
 * 
 * @author Lena Brüder
 * @date 2011-11-07
 * @copyright GNU GPL v3
 * @todo Doxygen-Kommentare
 * @todo Hinschreiben, dass operator< für T geschrieben werden muss
 */
template<typename T>
class Heap
{
public:
    /**
     * @brief Entfernt den Knoten mit niedrigsten Kosten
     * 
     * @return 
     * @todo 
     */
    virtual boost::shared_ptr<T> removeMinimumCostNode()=0;
    virtual void addNode(boost::shared_ptr<T> node)=0;
    virtual void decreaseKey(boost::shared_ptr<T> node)=0;
    virtual bool contains(boost::uint64_t nodeID)=0;
    virtual bool isEmpty()=0;
    virtual int size()=0;
    virtual ~Heap() {}
};

#endif //HEAP_HPP
