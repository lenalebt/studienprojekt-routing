#include "osmturnrestriction.hpp"


bool operator==(const OSMTurnRestriction& r1, const OSMTurnRestriction& r2)
{
    return ((r1.getFromId() == r2.getFromId()) &&
        (r1.getViaId() == r2.getViaId()) &&
        (r1.getToId() == r2.getToId()) &&
        (r1.getLeft() == r2.getLeft()) && 
        (r1.getRight() == r2.getRight()) &&
        (r1.getStraight() == r2.getStraight()) &&
        (r1.getUTurn() == r2.getUTurn()));
}
std::ostream& operator<<(std::ostream& os, const OSMTurnRestriction& r)
{
    os << "fromID: " << r.getFromId() << " viaID: " << r.getViaId() <<
        " toID: " << r.getToId() << " left: " << r.getLeft() <<
        " straight: " << r.getStraight() << " right: " <<
        r.getRight() << " uturn: " << r.getUTurn();
    return os;
}

namespace biker_tests
{
    int testOSMTurnRestriction()
    {
        OSMTurnRestriction restriction1;
        OSMTurnRestriction restriction2(0, 0, 0, false, false, false, false);
        
        CHECK_EQ(restriction1, restriction2);
        restriction1.setViaId(1);
        restriction2.setViaId(1);
        CHECK_EQ(restriction1, restriction2);
        restriction1.setFromId(2);
        restriction2.setFromId(2);
        CHECK_EQ(restriction1, restriction2);
        restriction1.setToId(3);
        restriction2.setToId(4);
        CHECK(!(restriction1 == restriction2));
        
        restriction1.setLeft(true);
        CHECK(restriction1.getLeft());
        CHECK(!restriction1.getRight());
        CHECK(!restriction1.getStraight());
        CHECK(!restriction1.getUTurn());
        
        restriction1.setRight(true);
        CHECK(restriction1.getRight());
        
        restriction1.setStraight(true);
        CHECK(restriction1.getStraight());
        
        restriction1.setUTurn(true);
        CHECK(restriction1.getUTurn());
        
        CHECK_EQ_TYPE(restriction1.getToId(), 3, boost::uint64_t);
        CHECK_EQ_TYPE(restriction1.getFromId(), 2, boost::uint64_t);
        CHECK_EQ_TYPE(restriction1.getViaId(), 1, boost::uint64_t);
        
        return EXIT_SUCCESS;
    }
} 
