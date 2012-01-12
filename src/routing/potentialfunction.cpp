#include "potentialfunction.hpp"

double StandardValuePotentialFunction::ratePosition(const double lon, const double lat)
{
    return _value;
}


double StandardValuePotentialFunction::ratePosition(const GPSPosition &pos)
{
    return _value;
}


namespace biker_tests
{
    int testPotentialFunction()
    {
        StandardValuePotentialFunction pot(5.0);
        CHECK_EQ(pot.ratePosition(0.0, 0.0), 5.0);
        CHECK_EQ(pot.ratePosition(GPSPosition(5.0, 6.0)), 5.0);
        CHECK_EQ(pot.ratePosition(5.0, 6.0), pot.ratePosition(GPSPosition(5.0, 6.0)));
        
        return EXIT_SUCCESS;
    }
}
