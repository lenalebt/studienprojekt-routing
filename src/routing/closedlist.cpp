#include "closedlist.hpp" 


namespace biker_tests
{
    int testMultiThreadedHashClosedList()
    {
        MultiThreadedHashClosedList list;
        list.addElement(5, S_THREAD);
        list.addElement(6, S_THREAD);
        CHECK(list.contains(5, S_THREAD));
        CHECK(!list.contains(5, T_THREAD));
        CHECK(list.contains(6, S_THREAD));
        CHECK(!list.contains(6, T_THREAD));
        
        for (int i=10; i<5000; i++)
        {
            list.addElement(i, S_THREAD);
            list.addElement(i+5000, T_THREAD);
        }
        CHECK(list.contains(500, S_THREAD));
        CHECK(list.contains(5500, T_THREAD));
        CHECK(!list.contains(500, T_THREAD));
        CHECK(!list.contains(5500, S_THREAD));
        
        CHECK_EQ_TYPE(list.size(S_THREAD), 4992, boost::uint64_t);
        CHECK_EQ_TYPE(list.size(T_THREAD), 4990, boost::uint64_t);
        
        list.addElement(5500, S_THREAD);
        CHECK_EQ_TYPE(list.getOverlappingElement(), 5500, boost::uint64_t);
        
        //TODO: Test mit wirklichen 2 Threads. Mit einem tut er, was er soll.
        
        return EXIT_SUCCESS;
    }
    
    int testHashClosedList()
    {
        HashClosedList list;
        list.addElement(5);
        list.addElement(6);
        CHECK(list.contains(5));
        CHECK(list.contains(6));
        CHECK(!list.contains(7));
        CHECK(!list.contains(8));
        
        for (int i=10; i<5000; i++)
        {
            list.addElement(i);
            list.addElement(i+5000);
        }
        CHECK(list.contains(500));
        CHECK(list.contains(5500));
        
        CHECK_EQ_TYPE(list.size(), 4992+4990, boost::uint64_t);
        
        list.addElement(5500);
        CHECK_EQ_TYPE(list.getOverlappingElement(), 5500, boost::uint64_t);
        
        return EXIT_SUCCESS;
    }
}
