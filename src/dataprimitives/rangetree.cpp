#include "rangetree.hpp"

#include "tests.hpp"

namespace biker_tests
{
    int testRangeTree()
    {
        RangeTree<int> tree;
        
        
        CHECK(!tree.contains(5));
        tree.insert(5);
        CHECK(tree.contains(5));
        CHECK(!tree.contains(6));
        CHECK(!tree.contains(6));
        tree.insert(6);
        CHECK(tree.contains(6));
        CHECK(!tree.contains(4));
        CHECK(!tree.contains(3));
        CHECK(!tree.contains(2));
        CHECK(!tree.contains(1));
        tree.insert(4);
        CHECK(tree.contains(6));
        CHECK(tree.contains(5));
        CHECK(tree.contains(4));
        CHECK(!tree.contains(3));
        CHECK(!tree.contains(2));
        CHECK(!tree.contains(1));
        tree.insert(1);
        CHECK(tree.contains(4));
        CHECK(!tree.contains(3));
        CHECK(!tree.contains(2));
        CHECK(tree.contains(1));
        tree.insert(10);
        CHECK(tree.contains(5));
        CHECK(tree.contains(6));
        CHECK(tree.contains(4));
        CHECK(!tree.contains(3));
        CHECK(!tree.contains(2));
        CHECK(tree.contains(1));
        CHECK(tree.contains(10));
        CHECK(!tree.contains(3));
        tree.insert(3);
        CHECK(tree.contains(5));
        CHECK(tree.contains(6));
        CHECK(tree.contains(4));
        CHECK(tree.contains(1));
        CHECK(tree.contains(10));
        CHECK(tree.contains(3));
        tree.insert(2);
        CHECK(tree.contains(5));
        CHECK(tree.contains(6));
        CHECK(tree.contains(4));
        CHECK(tree.contains(1));
        CHECK(tree.contains(10));
        CHECK(tree.contains(3));
        CHECK(tree.contains(2));
        
        return EXIT_FAILURE;
    }
}
