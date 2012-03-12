#ifndef RANGETREE_HPP
#define RANGETREE_HPP

#include <assert.h>
#include <iostream>

/**
 * @brief Diese Klasse implementiert einen binären Suchbaum, der Bereiche von Zahlen speichert.
 * 
 * 
 * @remarks Diese Datenstruktur wird nicht mehr verwendet und ist eher
 *      für Referenzzwecke noch im Quellcode verfügbar.
 * @see AdvancedRangeTree
 * @ingroup name
 * @author Lena Brueder
 * @date 2012-02-23
 * @copyright GNU GPL v3
 * @ingroup dataprimitives
 * @todo Doxygen!
 */
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
    
    /*void remove(const T& element)
    {
        
    }*/
    
    size_t sizeInBytes()
    {
        return (lTree!=0 ? lTree->sizeInBytes() : 0) + (rTree!=0 ? rTree->sizeInBytes() : 0) + sizeof(RangeTree);
    }
    
    template <typename U>
    friend std::ostream& operator<<(std::ostream& os, const RangeTree<U>& tree);
};


/**
 * @brief Diese Klasse implementiert einen binären Suchbaum, der Bereiche von Zahlen speichert.
 *      Dabei ist diese Version schneller als der RangeTree, und verbraucht weniger Speicher.
 * 
 * 
 * @remarks In der PDF-Dokumentation ist diese Datenstruktur genauer beschrieben.
 * @see RangeTree
 * @ingroup name
 * @author Lena Brueder
 * @date 2012-02-23
 * @copyright GNU GPL v3
 * @ingroup dataprimitives
 */
template <typename T>
class AdvancedRangeTree
{
private:
    T lBound;
    T uBound;
    AdvancedRangeTree<T>* lTree;
    AdvancedRangeTree<T>* rTree;
    
    AdvancedRangeTree<T>* cutLargestIfNecessary(const T& barrier)
    {
        if (rTree)
        {
            AdvancedRangeTree<T>* returnTree = rTree->cutLargestIfNecessary(barrier);
            if (returnTree == rTree)
            {
                rTree = returnTree->lTree;
                returnTree->lTree = 0;
            }
            return returnTree;
        }
        else
        {
            if (barrier == uBound)
            {
                return this;
            }
            else
                return 0;
        }
    }
    AdvancedRangeTree<T>* cutSmallestIfNecessary(const T& barrier)
    {
        if (lTree)
        {
            AdvancedRangeTree<T>* returnTree = lTree->cutSmallestIfNecessary(barrier);
            if (returnTree == lTree)
            {
                lTree = returnTree->rTree;
                returnTree->rTree = 0;
            }
            return returnTree;
        }
        else
        {
            if (barrier == lBound)
            {
                return this;
            }
            else
                return 0;
        }
    }
    
public:
    AdvancedRangeTree(const AdvancedRangeTree<T>& other)
        : lBound(other.lBound), uBound(other.uBound)
    {
        if (other.lTree)
            lTree = new AdvancedRangeTree<T>(*(other.lTree));
        else
            lTree = 0;
        if (other.rTree)
            rTree = new AdvancedRangeTree<T>(*(other.rTree));
        else
            rTree=0;
    }
    /**
     * @brief Erstellt einen neuen, leeren Baum.
     */
    AdvancedRangeTree() :
        lBound(2), uBound(1), lTree(0), rTree(0)
    {
        
    }
    ~AdvancedRangeTree()
    {
        if (lTree)
            delete lTree;
        if (rTree)
            delete rTree;
    }
    
    /**
     * @brief Fügt ein Element zum Baum hinzu.
     * 
     * @param element Das Element, das hinzugefügt werden soll.
     * @remarks Wenn ein Element schon enthalten ist, ändert diese Funktion
     *      nichts an dem Baum.
     */
    void insert(const T& element)
    {
        //untere Grenze größer obere Grenze bedeutet: ungültiger Wert drin.
        if (lBound > uBound)
        {
            lBound = element;
            uBound = element;
        }
        else if (lBound == element + 1)
        {
            lBound = element;
            //Ausgleichen.
            if (lTree)
            {
                AdvancedRangeTree<T>* cuttedTree = lTree->cutLargestIfNecessary(element-1);
                if (cuttedTree)
                {   //hat was gefunden...
                    lBound = cuttedTree->lBound;
                    if (cuttedTree == lTree)
                    {
                        lTree=cuttedTree->lTree;
                        cuttedTree->lTree=0;
                    }
                    delete cuttedTree;
                }
            }
        }
        else if (uBound == element - 1)
        {
            uBound = element;
            //Ausgleichen.
            if (rTree)
            {
                AdvancedRangeTree<T>* cuttedTree = rTree->cutSmallestIfNecessary(element+1);
                if (cuttedTree)
                {   //hat was gefunden...
                    uBound = cuttedTree->uBound;
                    if (cuttedTree == rTree)
                    {
                        rTree=cuttedTree->rTree;
                        cuttedTree->rTree=0;
                    }
                    delete cuttedTree;
                }
            }
        }
        else //Neuen Unterknoten aufmachen, bzw an Kinder weiterleiten
        {
            if (element < lBound)
            {   //Element links einfügen.
                if (!lTree)
                    lTree = new AdvancedRangeTree<T>();
                lTree->insert(element);
                
                //Ausgleichen...
                AdvancedRangeTree<T>* cuttedTree = lTree->cutLargestIfNecessary(element-1);
                if (cuttedTree)
                {   //hat was gefunden...
                    lBound = cuttedTree->lBound;
                    if (cuttedTree == lTree)
                    {
                        lTree=cuttedTree->lTree;
                        cuttedTree->lTree=0;
                    }
                    delete cuttedTree;
                }
            }
            else if (uBound < element)
            {   //Element rechts einfügen.
                if (!rTree)
                    rTree = new AdvancedRangeTree<T>();
                rTree->insert(element);
                
                //Ausgleichen...
                AdvancedRangeTree<T>* cuttedTree = rTree->cutSmallestIfNecessary(element+1);
                if (cuttedTree)
                {   //hat was gefunden...
                    uBound = cuttedTree->uBound;
                    if (cuttedTree == rTree)
                    {
                        rTree=cuttedTree->rTree;
                        cuttedTree->rTree=0;
                    }
                    delete cuttedTree;
                }
            }
            else
            {   //Knoten ist schon drin. Prima, nix zu tun.
                assert(this->contains(element));
                return;
            }
        }
        assert(this->contains(element));
    }
    
    /**
     * @brief Stellt fest, ob ein bestimmtes Element im Baum enthalten ist, oder nicht.
     * 
     * @param element Das Element, das gesucht werden soll.
     * @return Ob das übergebende Element im Baum enthalten ist, oder nicht.
     */
    bool contains(const T& element) const
    {
        if (uBound < lBound)
        {
            return false;
        }
        else
        {
            if ((lBound <= element) && (element <= uBound))
                return true;
            else if (element < lBound)
            {
                if (lTree)
                    return lTree->contains(element);
                else
                    return false;
            }
            else if (uBound < element)
            {
                if (rTree)
                    return rTree->contains(element);
                else
                    return false;
            }
            else
                return false;
        }
    }
    
    /*void remove(const T& element)
    {
        
    }*/
    
    /**
     * @brief Gibt die Größe eines AdvancedRangeTrees zurück, in Bytes.
     * 
     * Es werden die Größen aller Kinder mit berücksichtigt.
     * 
     * @return 
     * @todo 
     */
    size_t sizeInBytes()
    {
        return (lTree!=0 ? lTree->sizeInBytes() : 0) + (rTree!=0 ? rTree->sizeInBytes() : 0) + sizeof(AdvancedRangeTree);
    }
    
    template <typename U>
    friend std::ostream& operator<<(std::ostream& os, const AdvancedRangeTree<U>& tree);
    
    AdvancedRangeTree<T>& operator=(const AdvancedRangeTree<T>& other)
    {
        lBound = other.lBound;
        uBound = other.uBound;
        if (other.lTree)
            lTree = new AdvancedRangeTree<T>(*(other.lTree));
        else
            lTree = 0;
        if (other.rTree)
            rTree = new AdvancedRangeTree<T>(*(other.rTree));
        else
            rTree=0;
        
        return *this;
    }
};

/**
 * @brief Gibt einen RangeTree auf der Konsole aus.
 */
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

/**
 * @brief Gibt einen AdvancedRangeTree auf der Konsole aus.
 */
template <typename T>
std::ostream& operator<<(std::ostream& os, const AdvancedRangeTree<T>& tree)
{
    os << "(";
    if (tree.lTree != 0)
        os << *(tree.lTree);
    else
        os << " x ";
    
    os << " [" << tree.lBound << "-" << tree.uBound << "] ";
    
    if (tree.rTree != 0)
        os << *(tree.rTree);
    else
        os << " x ";
    os << ")";
    return os;
}

namespace biker_tests
{
    int testRangeTree();
    int testAdvancedRangeTree();
}

#endif //RANGETREE_HPP
