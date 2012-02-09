#ifndef ALTITUDEPROVIDER_HPP
#define ALTITUDEPROVIDER_HPP

#include "gpsposition.hpp"
#include "zip.hpp"
#include "tests.hpp"
#include "filedownloader.hpp"

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
#include <QDataStream>
#include <QDir>
#include <QFileInfo>

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
    
    QMap<int, QString> fileList;
    bool valid;
    int resolution;
    int intlat;
    int intlon;
    QString _cachedir;
    QString _srtmFileList;
    QUrl _url;
    qint16 *buffer;
    // QCache<int, SRTMTile> tileCache;
    
    void loadFileList();
    void createFileList();
    /**
     * @brief Holt die Höhe in Metern, für eine übergebene Koordinate.
     * 
     * 
     * @param lat Der Breitengrad
     * @param lon Der Längengrad
     * @return Den Höhenwert an dieser Stelle
     */
    double getAltitudeFromLatLon(double lat, double lon);
    
    /**
     * @brief Holt aus den unter buffer abgelegten Daten den Höhenwert des übergebenen Pixels.
     * 
     * Dabei wird der Pixel über seine beiden Koordinaten angegeben. Das 
     * verwendete Koordinatensystems startet in der oberen linken Ecke 
     * und wächst zur rechten unteren Ecke.
     * 
     * @param x X-Wert (aus Nachkommastelle des Längengrads).
     * @param y Y-Wert (aus Nachkommastelle das Breitengrads).
     * @return Den Höhenwert an dieser Stelle
     */
    int getPixelValue(int x, int y);
    
    int latLonToIndex(int lat, int lon){ 
        return lat * 1000 + lon; 
    }
    
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
    void downloadUrl(QUrl &url, QString &data);
    void downloadUrl(QUrl &dUrl, QByteArray &data);
    
    SRTMProvider() : _cachedir(""), _srtmFileList("srtmfile"), _url("http://dds.cr.usgs.gov/srtm/version2_1/SRTM3/"), buffer(0)
    {
        _cachedir = QDir::homePath() + "/.biker/srtm/";
    }
    
    SRTMProvider(QString cachedir) : _cachedir(cachedir), _srtmFileList("srtmfile"), _url("http://dds.cr.usgs.gov/srtm/version2_1/SRTM3/"), buffer(0)  {}
    
    SRTMProvider(QUrl url) : _cachedir(""), _srtmFileList("srtmfile"), _url(url), buffer(0)  
    {
        _cachedir = QDir::homePath() + "/.biker/srtm/";
    }
    
    SRTMProvider(QString cachedir, QUrl url) : _cachedir(cachedir), _srtmFileList("srtmfile"), _url(url), buffer(0)  {}
    
    ~SRTMProvider();
};






namespace biker_tests
{
	int testSRTMProvider();
}

#endif //ALTITUDEPROVIDER_HPP
