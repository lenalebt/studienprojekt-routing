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
    
    int getUBound()
    {
        if (lBound == 0)
        {
            assert(rTree != 0);
            return rTree->getUBound();
        }
        else
        {
            return uBound;
        }
    }
    int getLBound()
    {
        if (lBound == 0)
        {
            assert(lTree != 0);
            return lTree->getLBound();
        }
        else
        {
            return lBound;
        }
    }
    
    RangeTree<T>* cutRightmostLeafTreeIfNecessary(const T& otherLBound)
    {
        if (this->lBound == 0)
        {
            assert(this->rTree != 0);
            RangeTree<T>* retTree = rTree->cutRightmostLeafTreeIfNecessary(otherLBound);
            if (retTree == this->rTree)
            {   //Endknoten gefunden. Ausschneiden und hochpropagieren.
                RangeTree<T>* tmp = lTree;
                *this = *lTree;
                tmp->lTree = 0;
                tmp->rTree = 0;
                delete tmp;
                return retTree;
            }
            return retTree;
        }
        else
        {
            if (otherLBound == uBound + 1)
                return this;
            else
                return 0;
        }
    }
    void insertLeafTree(RangeTree<T>* leafTree)
    {
        assert(leafTree->lBound != 0);
        if (this->lBound == 0)
        {
            if (this->uBound > leafTree->uBound)
            {
                lTree->insertLeafTree(leafTree);
            }
            else
                std::cerr << "müp" << std::endl;
        }
        else
        {
            if (this->lBound == leafTree->uBound + 1)
            {
                this->lBound = leafTree->lBound;
            }
            else if (this->uBound == leafTree->lBound - 1)
            {
                this->uBound = leafTree->uBound;
            }
            else
            {
                std::cerr << "möp" << std::endl;
            }
        }
    }
    
    void rearrangeTree()
    {
        if (lBound == 0)
        {
            assert(lTree != 0);
            assert(rTree != 0);
            if ((lTree->lBound != 0) && (lTree->uBound + 1 == rTree->lBound))
            {
                lBound = lTree->lBound;
                uBound = rTree->uBound;
                delete lTree;
                delete rTree;
                lTree=0;
                rTree=0;
            }
            else
            {
                RangeTree<T>* rightmost = lTree->cutRightmostLeafTreeIfNecessary(rTree->getLBound());
                if (rightmost)
                {
                    rTree->insertLeafTree(rightmost);
                    if (rightmost == lTree)
                    {
                        //Kopie von rTree machen nach *this
                        RangeTree<T>* tmp = rTree;
                        this->lBound = rTree->lBound;
                        this->uBound = rTree->uBound;
                        this->lTree = rTree->lTree;
                        this->rTree = rTree->rTree;
                        
                        //sonst gibts Speicherlecks oder tmp löscht mir r-und lTree weg
                        tmp->lTree=0;
                        tmp->rTree=0;
                        delete tmp;
                    }
                    else
                        this->uBound = rightmost->lBound;
                    delete rightmost;
                }
            }
        }
    }
public:
    RangeTree(const RangeTree<T>& other)
        : lBound(other.lBound), uBound(other.uBound),
            lTree(other.lTree), rTree(other.rTree)
    {
        
    }
    RangeTree() :
        lBound(0), uBound(0), lTree(0), rTree(0)
    {
        
    }
    ~RangeTree()
    {
        if (lTree)
            delete lTree;
        if (rTree)
            delete rTree;
    }
    
    void insert(const T& element)
    {
        if (element == 0)
        {
            return;
        }
        
        if ((lBound == 0) && (uBound == 0))
        {
            lBound = element;
            uBound = element;
        }
        else if (lBound == 0)
        {   //Dies ist ein Mittelknoten, kein Blattknoten.
            assert(lTree != 0);
            assert(rTree != 0);
            if (element < uBound)
            {
                lTree->insert(element);
                rearrangeTree();
            }
            else
            {
                rTree->insert(element);
                rearrangeTree();
            }
        }
        else //Dies ist ein Blattknoten.
        {
            //Wenn man einfach die Grenzen erweitern kann, weil element direkt daneben liegt: Das tun.
            if (lBound == element + 1)
            {
                lBound = element;
            }
            else if (uBound == element - 1)
            {
                uBound = element;
            }
            else
            {   //Mehr als Grenzen erweitern: Knoten teilen.
                if (element < lBound)
                {   //Element links einfügen. Alter Baum wird rechter Teilbaum.
                    this->rTree = new RangeTree<T>(*this);
                    
                    this->lTree = new RangeTree<T>();
                    lTree->lBound = element;
                    lTree->uBound = element;
                    
                    this->lBound = 0;
                    this->uBound = rTree->lBound;
                }
                else if (uBound < element)
                {   //Element rechts einfügen. Alter Baum wird linker Teilbaum.
                    this->lTree = new RangeTree<T>(*this);
                    
                    this->rTree = new RangeTree<T>();
                    rTree->lBound = element;
                    rTree->uBound = element;
                    
                    this->lBound = 0;
                    this->uBound = element;
                }
                else
                {   //Knoten ist schon drin. Prima, nix zu tun.
                    return;
                }
            }
        }
    }
    
    bool contains(const T& element) const
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
            {
                assert(lTree != 0);
                return lTree->contains(element);
            }
            else
            {
                assert(rTree != 0);
                return rTree->contains(element);
            }
        }
    }
    
    void remove(const T& element)
    {
        
    }
    
    size_t sizeInBytes()
    {
        return (lTree!=0 ? lTree->sizeInBytes() : 0) + (rTree!=0 ? rTree->sizeInBytes() : 0) + sizeof(RangeTree);
    }
    
    template <typename U>
    friend std::ostream& operator<<(std::ostream& os, const RangeTree<U>& tree);
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const RangeTree<T>& tree)
{
    if (tree.lBound == 0)
        if (tree.uBound == 0)
        {
            os << "empty tree";
        }
        else
        {
            assert(tree.lTree != 0);
            assert(tree.rTree != 0);
            os << "(" << *(tree.lTree) << " " << tree.lBound << "/"
                << tree.uBound << " " << *(tree.rTree) << ")";
        }
    else
    {
        os << "[" << tree.lBound << "-" << tree.uBound << "]";
    }
    return os;
}

namespace biker_tests
{
    int testRangeTree();
}

#endif //RANGETREE_HPP
