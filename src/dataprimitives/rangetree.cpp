#include "rangetree.hpp"

#include "tests.hpp"
#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>
#include <ctime>
#include <QSet>

namespace biker_tests
{
    int testRangeTree()
    {
        RangeTree<int> tree;
        
        CHECK(!tree.contains(5));
        tree.insert(5);
        CHECK(tree.contains(5));
        tree.insert(5);
        std::cerr << tree << std::endl;
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
        tree.insert(5);
        std::cerr << tree << std::endl;
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
        
        std::cerr << tree << std::endl;
        
        RangeTree<int> tree2;
        for (int i=0; i<5000000; i++)
            tree2.insert(i);
        std::cerr << tree2 << std::endl;
        
        RangeTree<int> tree3;
        boost::minstd_rand generator(static_cast<unsigned int>(std::time(0)));
        typedef boost::variate_generator<boost::minstd_rand&, boost::uniform_int<> > gen_type;
        gen_type dist(generator, boost::uniform_int<>(1, 20000));
        
        QSet<int> set;
        for (int i=0; i<50000; i++)
        {
            int k = dist();
            tree3.insert(k);
            set.insert(k);
        }
        //std::cerr << tree3 << std::endl;
        std::cerr << "size for 50000 numbers: " << tree3.sizeInBytes()
            << " instead of " << set.size()*sizeof(int) << std::endl;
        return EXIT_FAILURE;
    }
    int testAdvancedRangeTree()
    {
        AdvancedRangeTree<int> tree;
        
        CHECK(!tree.contains(5));
        tree.insert(5);
        CHECK(tree.contains(5));
        tree.insert(5);
        std::cerr << tree << std::endl;
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
        tree.insert(5);
        std::cerr << tree << std::endl;
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
        
        std::cerr << tree << std::endl;
        
        AdvancedRangeTree<int> tree2;
        for (int i=0; i<5000000; i++)
            tree2.insert(i);
        std::cerr << tree2 << std::endl;
        
        AdvancedRangeTree<int> tree3;
        boost::minstd_rand generator(static_cast<unsigned int>(std::time(0)));
        typedef boost::variate_generator<boost::minstd_rand&, boost::uniform_int<> > gen_type;
        gen_type dist(generator, boost::uniform_int<>(1, 20000));
        
        QSet<int> set;
        for (int i=0; i<50000; i++)
        {
            int k = dist();
            tree3.insert(k);
            set.insert(k);
        }
        //std::cerr << tree3 << std::endl;
        std::cerr << "size for 50000 numbers: " << tree3.sizeInBytes()
            << " instead of " << set.size()*sizeof(int) << std::endl;
        return EXIT_FAILURE;
    }
}
