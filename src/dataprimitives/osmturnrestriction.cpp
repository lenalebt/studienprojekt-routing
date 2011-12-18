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
        return EXIT_SUCCESS;
    }
} 
