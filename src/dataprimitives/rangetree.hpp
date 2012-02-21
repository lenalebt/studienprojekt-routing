#ifndef RANGETREE_HPP
#define RANGETREE_HPP

#include <assert.h>
#include <iostream>

template <typename T>
class RangeTree
{
private:
    T lBound;
    T uBound;
    RangeTree<T>* lTree;
    RangeTree<T>* rTree;
public:
    RangeTree(const RangeTree& other)
        : lBound(other.lBound), uBound(other.uBound),
            lTree(other.lTree), rTree(other.rTree)
    {
        
    }
    RangeTree() :
        lBound(0), uBound(0), lTree(0), rTree(0)
    {
        
    }
    
    void insert(const T& element)
    {
        //std::cerr << "in : lBound:" << lBound << " uBound:" << uBound << std::endl;
        if ((lBound == 0) && (uBound == 0))
        {
            lBound = (uBound = element);
        }
        else if (lBound == 0)
        {
            //TODO: Ausgleichen. Dafür: Grenzen prüfen der Kinder... Reicht das?
            if (element < uBound)
                lTree->insert(element);
            else
                rTree->insert(element);
        }
        else
        {
            if (lBound == element + 1)
                lBound = element;
            else if (uBound == element - 1)
                uBound = element;
            else
            {
                if (element < lBound)
                {
                    rTree = new RangeTree(*this);
                    
                    lTree = new RangeTree();
                    lTree->lBound = element;
                    lTree->uBound = element;
                    
                    this->lBound = 0;
                    this->uBound = rTree->lBound;
                }
                else if (uBound < element)
                {
                    lTree = new RangeTree(*this);
                    
                    rTree = new RangeTree();
                    rTree->lBound = element;
                    rTree->uBound = element;
                    
                    this->lBound = 0;
                    this->uBound = element;
                }
            }
        }
        //std::cerr << "out: lBound:" << lBound << " uBound:" << uBound << std::endl;
    }
    
    bool contains(const T& element)
    {
        if (lBound != 0)
        {
            if ((lBound <= element) && (element <= uBound))
                return true;
            else
                return false;
        }
        else
        {
            if (uBound == 0)
                return false;
            else if (uBound == element)
                return true;
            else if (element < uBound)
                return lTree->contains(element);
            else
                return rTree->contains(element);
        }
    }
    
    void remove(const T& element)
    {
        
    }
};

namespace biker_tests
{
    int testRangeTree();
}

#endif //RANGETREE_HPP
