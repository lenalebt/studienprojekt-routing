class AltitudeProvider
{
private:
    
public:
    virtual double getAltitude(double lon, double lat)=0;
    virtual double getAltitude(const GPSPosition& pos)=0;
}

class SRTMProvider : public AltitudeProvider
{
private:
    
public:
    double getAltitude(double lon, double lat);
    double getAltitude(const GPSPosition& pos);
}
