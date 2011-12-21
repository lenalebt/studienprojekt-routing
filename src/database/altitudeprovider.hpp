#ifndef ALTITUDEPROVIDER_HPP
#define ALTITUDEPROVIDER_HPP

#include "gpsposition.hpp"
#include "zip.hpp"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QNetworkRequest>
#include <QFile>
#include <QDir>
#include <QMap>
#include <QCache>
#include <boost/cstdint.hpp>
#include <QEventLoop>
#include <QRegExp>
#include <QThread>
#include <QFuture>
#include <QtConcurrentRun>

#define SRTM_DATA_VOID -32768
/**
 * @brief Ein AltitudeProvider kann für jeden Punkt auf der Erde einen
 *      Höhenwert zurückgeben.
 * 
 * AltitudeProvider ist ein Interface und kann selbst nicht instanziiert
 * werden.
 * 
 * @see SRTMProvider
 * @ingroup database
 * @author Lena Brüder
 * @date 2011-11-01
 * @copyright GNU GPL v3
 * @todo Zugriffe müssen threadsicher sein!
 */
class AltitudeProvider
{
private:
    
public:
    /**
     * @brief Gibt einen Höhenwert für einen Punkt auf der Erdoberfläche zurück.
     * 
     * Dabei wird der Punkt über seine beiden Koordinaten angegeben.
     * 
     * @param lon Der Längengrad.
     * @param lat Der Breitengrad.
     * @return Den Höhenwert an dieser Stelle
     */
    virtual double getAltitude(double lat, double lon)=0;
    /**
     * @brief Gibt einen Höhenwert für einen Punkt auf der Erdoberfläche zurück.
     * 
     * Dabei wird der Punkt über seine GPSPosition angegeben.
     * 
     * @param pos Die Position, an der der Höhenwert ausgegeben werden soll
     * @return Den Höhenwert an dieser Stelle
     */
    virtual double getAltitude(const GPSPosition& pos)=0;
    
    virtual ~AltitudeProvider() {}
};

class FileDownloader : public QThread
{
private:
    
public:
    FileDownloader();
    void run();
    QByteArray downloadURL(QUrl url);
    //QByteArray downloadURL(QUrl url, QNetworkReply::NetworkError *error);
};


/*TODO:
- Soll sich selber runterladen
- Soll selber die URL rausfinden zum runterladen
*/
/**
 * @brief 
 * 
 * 
 * 
 * @author Lena Brüder
 * @date 2011-11-28
 * @copyright GNU GPL v3
 * @todo Doxygen, Implementieren, Definieren, blablabla
 */
class SRTMTile
{
private:
    int _lat;
    int _lon;
    int _size;
    
    boost::uint16_t* _data;
    
    bool _valid;
    QFile _file;
    QDir _cacheDirectory;
    
    /**
     * @brief Lädt Daten aus der zip-Datei, die mit <code>file</code> bezeichnet ist
            und speichert sie in <code>data</code>.
     * 
     * @todo implementieren
     */
    void getData();
    
    /**
     * @brief 
     * 
     * @return 
     * @todo 
     */
    void downloadData();
    
public:
    double getAltitude(double lat, double lon);
    
    SRTMTile(double lat, double lon, QDir cacheDirectory) : _lat(lat), _lon(lon), _cacheDirectory(cacheDirectory)
    {
        //TODO: Daten runterladen und entpacken
        /*
        regex.setPattern("<a href=\"([NS])(\\d{2})([EW])(\\d{3})\\.hgt\\.zip");
        QDir dir;
        if (!dir.exists(cachedir)) {
        
        
        if (_lat => 0 && _lon => 0){
            _file = cacheDirectory + "/" + "N" + _lat + "E" + _lon + ".hgt.zip
        }
        getData()
        */
    }
};


/**
 * @brief SRTM-Implementierung vom AltitudeProvider.
 * 
 * SRTM (Shuttle Radar Topography Mission) war eine Mission der NASA zur
 * Vermessung der Erde (siehe http://de.wikipedia.org/wiki/SRTM). Die
 * entstandenen Daten sind frei im Netz verfügbar (ftp://e0srp01u.ecs.nasa.gov/srtm/)
 * und können ohne Einschränkungen verwendet werden. Sie sind in Europa
 * nicht sehr genau (1 Pixel entspricht etwa 90m - in Amerika entspricht
 * ein Pixel etwa 30m).
 * 
 * @ingroup database
 * @author Lena Brüder
 * @date 2011-11-01
 * @copyright GNU GPL v3
 * @todo Doxygen-Kommentare und Implementierung
 */
class SRTMProvider : public AltitudeProvider
{
private:
    void createFileList();
    
    QMap<int, QString> fileList;
    QCache<int, SRTMTile> tileCache;
    int latLonToIndex(int lat, int lon) { return lat * 1000 + lon; }
    QString _cachedir;
    QUrl _url;
    QByteArray blubb(const QUrl &dUrl);
    void loadFileList();
    double getAltitudeFromLatLon(double lat, double lon);
    int getPixelValue(int x, int y);
    bool valid;
    qint16 *buffer;
    int resolution;
    int intlat;
    int intlon;
    
    float avg(float a, float b, float weight){
        if (a == SRTM_DATA_VOID) return b;
        if (b == SRTM_DATA_VOID) return a;
        return b*weight + a*(1-weight);
    }
    
public:
    /**
     * @brief Gibt einen Höhenwert für einen Punkt auf der Erdoberfläche zurück.
     * 
     * Dabei wird der Punkt über seine beiden Koordinaten angegeben.
     * 
     * @param lon Der Längengrad.
     * @param lat Der Breitengrad.
     * @return Den Höhenwert an dieser Stelle
     */
    double getAltitude(double lat, double lon);
    /**
     * @brief Gibt einen Höhenwert für einen Punkt auf der Erdoberfläche zurück.
     * 
     * Dabei wird der Punkt über seine GPSPosition angegeben.
     * 
     * @param pos Die Position, an der der Höhenwert ausgegeben werden soll
     * @return Den Höhenwert an dieser Stelle
     */
    double getAltitude(const GPSPosition& pos);
    /**
     * @brief Läd Antwort für die übergebe URL in data.
     * 
     * 
     * @param url URL als QUrl die aufgerufen werden soll
     * @param data QString in den der Inhalt der der NetworkReply gespeichert weden soll
     * @return enum QNetworkReply::NetworkError (Ist NoError wenn kein Fehler aufgetreten ist.)
     */
    void downloadUrl(const QUrl &url, QString &data);
    
    SRTMProvider() : _cachedir("~/.biker/srtm/"), _url("http://dds.cr.usgs.gov/srtm/version2_1/SRTM3/") {}
    
    SRTMProvider(QString cachedir) : _cachedir(cachedir), _url("http://dds.cr.usgs.gov/srtm/version2_1/SRTM3/")  {}
    
    SRTMProvider(QUrl url) : _cachedir("~/.biker/srtm/"), _url(url)  {}
    
    SRTMProvider(QString cachedir, QUrl url) : _cachedir(cachedir), _url(url)  {}
    
    ~SRTMProvider();
};

namespace biker_tests
{
	int testSRTMProvider();
}

#endif //ALTITUDEPROVIDER_HPP
