#include "heap.hpp" 

namespace biker_tests
{
    int testBinaryHeap()
    {
        
        biker_tests::BinaryHeapTestLessFunctor<int> less1;
        BinaryHeap<int, biker_tests::BinaryHeapTestLessFunctor<int> > heap1(less1);
        
        CHECK(heap1.isEmpty());
        heap1.add(9);
        heap1.add(8);
        heap1.add(7);
        heap1.add(6);
        heap1.add(5);
        heap1.add(4);
        heap1.add(3);
        heap1.add(2);
        heap1.add(1);
        
        CHECK(less1(1, 2));
        CHECK(less1(1, 5));
        CHECK(less1(1, 9));
        CHECK(!less1(2, 1));
        CHECK(!less1(2, 2));
        CHECK(heap1.contains(6));
        CHECK(heap1.contains(4));
        CHECK(!heap1.contains(10));
        
        CHECK_EQ(heap1.removeMinimumCostElement(), 1);
        CHECK_EQ(heap1.removeMinimumCostElement(), 2);
        CHECK_EQ(heap1.removeMinimumCostElement(), 3);
        CHECK_EQ(heap1.removeMinimumCostElement(), 4);
        CHECK_EQ(heap1.removeMinimumCostElement(), 5);
        CHECK_EQ(heap1.removeMinimumCostElement(), 6);
        CHECK_EQ(heap1.removeMinimumCostElement(), 7);
        CHECK_EQ(heap1.removeMinimumCostElement(), 8);
        CHECK_EQ(heap1.removeMinimumCostElement(), 9);
        
        biker_tests::BinaryHeapTestLessAndQHashFunctor<int, double> less2;
        BinaryHeap<int, biker_tests::BinaryHeapTestLessAndQHashFunctor<int, double> > heap2(less2);
        
        less2.setValue(1, 1);
        less2.setValue(2, 2);
        less2.setValue(3, 3);
        less2.setValue(4, 4);
        less2.setValue(5, 5);
        less2.setValue(6, 6);
        less2.setValue(7, 7);
        less2.setValue(8, 8);
        less2.setValue(9, 9);
        
        CHECK_EQ(less2.getValue(1), 1.0);
        CHECK_EQ(less2.getValue(2), 2.0);
        CHECK_EQ(less2.getValue(3), 3.0);
        CHECK_EQ(less2.getValue(4), 4.0);
        CHECK_EQ(less2.getValue(5), 5.0);
        CHECK_EQ(less2.getValue(6), 6.0);
        CHECK_EQ(less2.getValue(7), 7.0);
        CHECK_EQ(less2.getValue(8), 8.0);
        CHECK_EQ(less2.getValue(9), 9.0);
        
        CHECK(less2(1, 2));
        CHECK(less2(1, 5));
        CHECK(less2(1, 9));
        CHECK(less2(4, 9));
        CHECK(!less2(2, 1));
        CHECK(!less2(2, 2));
        
        heap2.add(1);
        heap2.add(2);
        heap2.add(3);
        heap2.add(4);
        heap2.add(5);
        heap2.add(6);
        heap2.add(7);
        heap2.add(8);
        heap2.add(9);
        CHECK_EQ(heap2.removeMinimumCostElement(), 1);
        CHECK_EQ(heap2.removeMinimumCostElement(), 2);
        CHECK_EQ(heap2.removeMinimumCostElement(), 3);
        CHECK_EQ(heap2.removeMinimumCostElement(), 4);
        CHECK_EQ(heap2.removeMinimumCostElement(), 5);
        CHECK_EQ(heap2.removeMinimumCostElement(), 6);
        CHECK_EQ(heap2.removeMinimumCostElement(), 7);
        CHECK_EQ(heap2.removeMinimumCostElement(), 8);
        CHECK_EQ(heap2.removeMinimumCostElement(), 9);
        
        CHECK(heap1.isEmpty());
        
        heap2.add(1);
        heap2.add(9);
        heap2.add(4);
        heap2.add(3);
        heap2.add(5);
        heap2.add(6);
        heap2.add(8);
        heap2.add(7);
        heap2.add(2);
        less2.setValue(9, 3.5);
        heap2.decreaseKey(9);
        less2.setValue(8, 3.3);
        heap2.decreaseKey(8);
        less2.setValue(4, 3.4);
        heap2.decreaseKey(4);
        
        CHECK(heap2.contains(9));
        
        CHECK_EQ(heap2.removeMinimumCostElement(), 1);
        CHECK_EQ(heap2.removeMinimumCostElement(), 2);
        CHECK_EQ(heap2.removeMinimumCostElement(), 3);
        CHECK_EQ(heap2.removeMinimumCostElement(), 8);
        CHECK_EQ(heap2.removeMinimumCostElement(), 4);
        CHECK_EQ(heap2.removeMinimumCostElement(), 9);
        CHECK_EQ(heap2.removeMinimumCostElement(), 5);
        CHECK_EQ(heap2.removeMinimumCostElement(), 6);
        CHECK_EQ(heap2.removeMinimumCostElement(), 7);
        
        return EXIT_SUCCESS;
    }
}
