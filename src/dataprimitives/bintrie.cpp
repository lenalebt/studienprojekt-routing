#include "bintrie.hpp"
#include <iostream>

namespace biker_tests
{
    int testBinTrie()
    {
        BinTrie<boost::uint64_t> trie;
        
        CHECK(!trie.contains(5));
        CHECK(trie.insert(5));
        CHECK(trie.contains(5));
        CHECK(!trie.contains(6));
        CHECK(!trie.contains(6));
        CHECK(!trie.contains(4));
        CHECK(!trie.contains(3));
        CHECK(!trie.contains(2));
        CHECK(!trie.contains(1));
        //CHECK(!trie.insert(5));
        CHECK(trie.contains(5));
        
        CHECK(!trie.contains(5764));
        CHECK(trie.insert(5764));
        CHECK(trie.contains(5764));
        CHECK(!trie.contains(5765));
        //CHECK(!trie.insert(5764));
        CHECK(trie.contains(5764));
        
        /*
        for (int i=0; i<100; i++)
        {
            CHECK(!trie.contains(i));
            CHECK(!trie.insert(i));
            CHECK(trie.contains(i));
            CHECK(trie.insert(i));
            CHECK(trie.contains(i));
        }
        */
        
        return EXIT_FAILURE;
    }
}
