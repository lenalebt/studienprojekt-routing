#ifndef BINTREE_HPP
#define BINTRIE_HPP

#include <boost/cstdint.hpp>
#include "tests.hpp"
#include <cmath>
#include <iostream>
#include <assert.h>

#define TRIE_PROPERTY_INVALID 0
#define TRIE_PROPERTY_ALL     2
#define TRIE_PROPERTY_NONE    1
#define TRIE_PROPERTY_FOLLOW  3

#define TRIE_PROPERTY_LENGTH 2
#define TRIE_FOLLOWER_COUNT  64 / TRIE_PROPERTY_LENGTH
#define TRIE_STEP_BITS       ((int)(log(TRIE_FOLLOWER_COUNT)/log(2)))

template <typename T>
class BinTrie
{
private:
    boost::uint64_t follower;
    BinTrie* bintrieArray;
    int _size;
    
    inline void setFollowerValue(int position, int value)
    {
        //follower-wert löschen
        follower &= ~((boost::uint64_t)((1<<TRIE_PROPERTY_LENGTH)-1u) << position);
        //follower-wert auf den richtigen wert setzen
        follower |= (boost::uint64_t)value << position;
    }
    
    inline int getFollowerValue(int position)
    {
        return (follower & ((((boost::uint64_t)1<<(boost::uint64_t)TRIE_PROPERTY_LENGTH)-(boost::uint64_t)1u) << position)) >> position;
    }
    
    inline int getSummedFollowerValue()
    {
        int retVal = 0;
        //Schiebe follower immer ein Stückchen weiter nach rechts und verodere dann
        for (int i=0; i<_size; i+=TRIE_PROPERTY_LENGTH)
            retVal |= (follower >> i) & 3;
        return retVal;
    }
    
    int insert(const T& element, int level)
    {
        //Am Ende angekommen: Nun sind alle gespeichert.
        if (level <= 0)
        {
            for (int i=0; i<_size; i+=TRIE_PROPERTY_LENGTH)
                setFollowerValue(i, TRIE_PROPERTY_ALL);
            return TRIE_PROPERTY_ALL;
        }
        
        //Position der Bits im Element auf diesem Level ausrechnen
        int pos = (level - TRIE_STEP_BITS);
        //Bitmaske, um die entsprechenden Bits aus dem Element herauszulösen
        T mask;
        if (pos > 0)
            mask = ((T)(1<<(T)TRIE_STEP_BITS)-(T)1) << pos;
        else
            mask = ((T)(1<<(T)TRIE_STEP_BITS)-(T)1) >> -pos;
        //Rechnet aus, an welcher Stelle im follower-Speicher die gesuchte Eigenschaft steht
        boost::uint64_t posInFollower;
        if (pos > 0)
            posInFollower = (element & mask) >> pos;
        else
            posInFollower = (element & mask) << -pos;
        //Sucht heraus, was denn für ein Wert beim follower genau steht
        int followerValue = getFollowerValue(posInFollower);
        
        assert(followerValue != TRIE_PROPERTY_INVALID);
        
        switch (followerValue)
        {
            case TRIE_PROPERTY_ALL:
                return TRIE_PROPERTY_ALL;
            case TRIE_PROPERTY_NONE:
                //Wenn Array noch unbelegt: Belegen!
                if (!bintrieArray)
                    bintrieArray = new BinTrie[TRIE_FOLLOWER_COUNT];
            case TRIE_PROPERTY_FOLLOW:
                //Einfügen auf der Ebene darunter
                int retVal = bintrieArray[posInFollower].insert(element, level - TRIE_STEP_BITS);
                std::cerr << retVal << std::endl;
                assert (retVal != TRIE_PROPERTY_NONE);
                assert (retVal != TRIE_PROPERTY_INVALID);
                
                switch (retVal)
                {
                    case TRIE_PROPERTY_ALL:
                        //TODO: Wenn alle auf ALL stehen: löschen und zusammenfassen.
                        if (bintrieArray[posInFollower].getSummedFollowerValue() == TRIE_PROPERTY_ALL)
                        {
                            setFollowerValue(posInFollower, TRIE_PROPERTY_ALL);
                            delete[] bintrieArray;
                            bintrieArray = 0;
                        }
                        else
                        {
                            setFollowerValue(posInFollower, TRIE_PROPERTY_FOLLOW);
                        }
                        break;
                    case TRIE_PROPERTY_FOLLOW:
                        setFollowerValue(posInFollower, TRIE_PROPERTY_FOLLOW);
                        break;
                }
                
                //TODO: follower-value entsprechend setzen.
                //setFollower(posInFollower, retVal);
                return getFollowerValue(posInFollower);
        }
        //std::cerr << biker_tests::uint64_t2string(mask) << std::endl;
        return TRIE_PROPERTY_INVALID;
    }
    bool contains(const T& element, int level)
    {
        //Am Ende angekommen: Nun sind alle gespeichert.
        if (level <= 0)
            return TRIE_PROPERTY_ALL;
        
        //Position der Bits im Element auf diesem Level ausrechnen
        int pos = (level - TRIE_STEP_BITS);
        //Bitmaske, um die entsprechenden Bits aus dem Element herauszulösen
        T mask;
        if (pos > 0)
            mask = ((T)(1<<(T)TRIE_STEP_BITS)-(T)1) << pos;
        else
            mask = ((T)(1<<(T)TRIE_STEP_BITS)-(T)1) >> (-pos);
        //Rechnet aus, an welcher Stelle im follower-Speicher die gesuchte Eigenschaft steht
        boost::uint64_t posInFollower;
        if (pos > 0)
            posInFollower = (element & mask) >> pos;
        else
            posInFollower = (element & mask) << -pos;
        //Sucht heraus, was denn für ein Wert beim follower genau steht
        int followerValue = getFollowerValue(posInFollower);
        
        switch (followerValue)
        {
            case TRIE_PROPERTY_ALL:
                return true;
            case TRIE_PROPERTY_NONE:
                return false;
            case TRIE_PROPERTY_FOLLOW:
                assert(bintrieArray != 0);
                return bintrieArray[posInFollower].contains(element, level - TRIE_STEP_BITS);
        }
        //std::cerr << biker_tests::uint64_t2string(mask) << std::endl;
        return false;
    }
public:
    BinTrie() : follower(0), bintrieArray(0)
    {
        _size = sizeof(T) * 8;
        //std::cerr << "_size = " << _size << std::endl;
        //keine Nachfolger.
        for (int i=0; i<_size; i+=TRIE_PROPERTY_LENGTH)
            setFollowerValue(i, TRIE_PROPERTY_NONE);
        assert(getSummedFollowerValue() == TRIE_PROPERTY_NONE);
        
    }
    bool insert (const T& element)
    {
        bool retVal = (insert(element, _size) != TRIE_PROPERTY_NONE);
        return retVal;
    }
    bool contains(const T& element)
    {
        return contains(element, _size);
    }
    
    ~BinTrie()
    {
        //TODO: Array und so löschen
        if (bintrieArray)
        {
            delete[] bintrieArray;
            bintrieArray = 0;
        }
    }
};

class BinTrie2
{
private:
    boost::uint64_t follower;
    BinTrie2* bintrieArray;
    
    inline int getFollowerValue(int number)
    {
        return ((3ull << (2ull*number)) & follower) >> (2ull*number);
    }
    inline int setFollowerValue(int number, int value)
    {
        return ((3ull << (2ull*number)) & follower) >> (2ull*number);
    }
    
    int insert(boost::uint64_t element, int level)
    {
        if (level >= 0)
        {
            int followerbucket = ((15ull << level) & element) >> level;
            switch (getFollowerValue(followerbucket))
            {
                case TRIE_PROPERTY_ALL:
                    return 0;   //schon belegt, kann nichts einfügen.
                case TRIE_PROPERTY_NONE:
                    if (!bintrieArray)
                        bintrieArray = new BinTrie2[16];
                case TRIE_PROPERTY_FOLLOW:
                    assert(bintrieArray != 0);
                    switch (bintrieArray[followerbucket].insert(element, level-4))
                    {
                        case -1:
                            //Überprüfen
                        case 0:
                            return 0;   //Einfügen ging nicht. Keine Nachbearbeitung.
                        case 1:
                            
                            return 1;   //Einfügen hat geklappt. Keine Nachbearbeitung.
                    }
            }
        }
        else
        {
            return -1;
        }
    }
    bool contains(boost::uint64_t element, int level)
    {
        int followerbucket = ((15ull << level) & element) >> level;
        switch (getFollowerValue(followerbucket))
        {
            case TRIE_PROPERTY_ALL:
                return true;
            case TRIE_PROPERTY_NONE:
                return false;
            case TRIE_PROPERTY_FOLLOW:
                assert(bintrieArray != 0);
                return bintrieArray[followerbucket].contains(element, level-4);
        }
    }
public:
    BinTrie2() : follower(6148914691236517205), bintrieArray(0)
    {
        //Setzt die Nachfolger alle auf NONE ^^.
    }
    bool contains(boost::uint64_t element)
    {
        return contains(element, 60);
    }
    bool insert (boost::uint64_t element)
    {
        return insert(element, 60);
    }
    ~BinTrie2()
    {
        //TODO: Array und so löschen
        if (bintrieArray)
        {
            delete[] bintrieArray;
            bintrieArray = 0;
        }
    }
};

namespace biker_tests
{
    int testBinTrie();
}
#endif //BINTRIE_HPP
