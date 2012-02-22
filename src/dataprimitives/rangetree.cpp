#include "rangetree.hpp"

#include "tests.hpp"

namespace biker_tests
{
    int testRangeTree()
    {
        RangeTree<int> tree;
        
        CHECK(!tree.contains(5));
        std::cerr << tree << std::endl;
        tree.insert(5);
        CHECK(tree.contains(5));
        CHECK(!tree.contains(6));
        CHECK(!tree.contains(6));
        std::cerr << tree << std::endl;
        tree.insert(6);
        CHECK(tree.contains(6));
        CHECK(!tree.contains(4));
        CHECK(!tree.contains(3));
        CHECK(!tree.contains(2));
        CHECK(!tree.contains(1));
        std::cerr << tree << std::endl;
        tree.insert(4);
        CHECK(tree.contains(6));
        CHECK(tree.contains(5));
        CHECK(tree.contains(4));
        CHECK(!tree.contains(3));
        CHECK(!tree.contains(2));
        CHECK(!tree.contains(1));
        std::cerr << tree << std::endl;
        tree.insert(1);
        CHECK(tree.contains(4));
        CHECK(!tree.contains(3));
        CHECK(!tree.contains(2));
        CHECK(tree.contains(1));
        std::cerr << tree << std::endl;
        tree.insert(10);
        CHECK(tree.contains(5));
        CHECK(tree.contains(6));
        CHECK(tree.contains(4));
        CHECK(!tree.contains(3));
        CHECK(!tree.contains(2));
        CHECK(tree.contains(1));
        CHECK(tree.contains(10));
        CHECK(!tree.contains(3));
        std::cerr << tree << std::endl;
        tree.insert(3);
        CHECK(tree.contains(5));
        CHECK(tree.contains(6));
        CHECK(tree.contains(4));
        CHECK(tree.contains(1));
        CHECK(tree.contains(10));
        CHECK(tree.contains(3));
        std::cerr << tree << std::endl;
        tree.insert(2);
        CHECK(tree.contains(5));
        CHECK(tree.contains(6));
        CHECK(tree.contains(4));
        CHECK(tree.contains(1));
        CHECK(tree.contains(10));
        CHECK(tree.contains(3));
        CHECK(tree.contains(2));
        
        std::cerr << tree << std::endl;
        
        RangeTree<int> tree2;
        for (int i=0; i<5000000; i++)
            tree2.insert(i);
        std::cerr << tree2;
        
        return EXIT_FAILURE;
    }
}
