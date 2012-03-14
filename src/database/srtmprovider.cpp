#ifdef ZZIP_FOUND

#include "srtmprovider.hpp"


#include <QtGlobal>
#include <QMutexLocker>
#include <math.h>
#include <QtEndian>
#include <iostream>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QNetworkRequest>
#include <QMap>
#include <boost/cstdint.hpp>
#include <QEventLoop>
#include <QRegExp>
#include <QThread>
#include <QFuture>
#include <QtConcurrentRun>
#include <QDataStream>
#include <QDir>
#include <QFileInfo>

// TODO
// - Was man noch machen könnte: Eine ausgeklügeltere Fehlerbehandlung, falls Download schief läuft.
//  Momentan führt jeder Fehler in der Verarbeitung dazu, dass als ermittelter Höhenwer NN zurückgegeben wird.
// - Was auch noch nicht fertig ist, ist die Behandlung von falschen SRTM-Werten. Da muss noch eine schöne Lösung für her: Gewichtete Höhe aus umliegenden Werten.


/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// Klasse SRTMProvider //////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

QCache<int, SRTMTile> SRTMProvider::tileCache;

QReadWriteLock SRTMProvider::lock(QReadWriteLock::Recursive);

double SRTMProvider::getAltitude(const GPSPosition& pos)
{
    return this->getAltitude(pos.getLat(), pos.getLon());
}

double SRTMProvider::getAltitude(double lat, double lon)
{
    double altitude = 0.0; // Defaultwert für Höhe ist NN
    
    index = latLonToIndex(int(floor(lat)), int(floor(lon)));
    
    lock.lockForWrite();
    SRTMTile *tile = new SRTMTile(index);
    if(tileCache.contains(index)){
        tile = tileCache[index];
    }
    else if(fillTile(index, &tile)){
        tileCache.insert(index, tile);
    }
    else{
        lock.unlock();
        return altitude;
    }
    lock.unlock();
    
    lock.lockForRead();
    altitude = tile->getAltitudeFromLatLon(lat, lon);
    lock.unlock();
    
    return altitude;
}


bool SRTMProvider::downloadZipFile(QString fileName, QFile &zipFile){

    QString altZipUrl =  _url.toString() + fileList[index]; //Url bis .hgt.zip
    altZipUrl.toAscii().constData();
    QUrl srtmUrl(altZipUrl);
    QByteArray data;

    downloadUrl(srtmUrl, data); // Zipfile soll nun runtergeladen werden.

    if(data.isEmpty()){
        std::cout << "Fehler beim downloaden der Daten für " << fileList[index] << "." << std::endl;
        return false; // download failed
    }

    QFileInfo fileInfo(fileName);
    QString filePath = fileInfo.absolutePath();
    QDir makedir;
    makedir.mkpath(filePath);
    zipFile.open(QIODevice::WriteOnly);
    zipFile.write(data); // Speichern des geladenen Zipfiles
    zipFile.close();

    return true; // download successful
}

bool SRTMProvider::fillTile(int index, SRTMTile **tile){

    bool tileFilled = false;

    QString fileName;   // Name incl. Pfad (wird aus _cachedir und Index erstellt)

    loadFileList();     // per loadFilelist(): fileListe vorhanden? falls nicht: erstellen!

    if (fileList.contains(index)){

        fileName = _cachedir+fileList[index];

        QFile zipFile(fileName);

        if(!zipFile.open(QIODevice::ReadOnly) && !downloadZipFile(fileName, zipFile)){

            return tileFilled;

        }

        tileFilled = (*tile)->fillTile(fileName);
    }

    return tileFilled;
}

void SRTMProvider::loadFileList()
{
    QFile file(_cachedir + _srtmFileList);
    
    if (!file.open(QIODevice::ReadOnly)) {   

        createFileList();
        return;
    }   
    
    QDataStream stream(&file);
    stream >> fileList;
    file.close();
    
    return;
}

void SRTMProvider::createFileList()
{
    QStringList continents;
    continents << "Africa" << "Australia" << "Eurasia" << "Islands" << "North_America" << "South_America";

    QString url = _url.toString();
    
    QStringList dateiListe;	
    int capCount = 0;
    int lat;
    int lon;
    int pos = 0;
    
    foreach (QString continent, continents) { // für jeden Kontinent, die vorhandenen Ziparchive in die Liste eintragen

        QString urlTemp = QString(url+continent+"/").toAscii().constData(); // Kontinent zur url hinzufügen
        QUrl srtmUrl(urlTemp);                                              // urlTemp zu QUrl casten, für spätere Verwendung.
        QString replyString;                                                // Hierein wird später die NetworkReply aus downloadUrl gespeichert.

        
        downloadUrl(srtmUrl, replyString);

        if(!replyString.isEmpty())                                          // Erstellen der Liste, falls Download erfolgreich.
        {
            QRegExp regex("<li>\\s*<a\\s+href=\"([^\"]+)");
            regex.indexIn(replyString);
            pos = 0;
            capCount = 0;
            dateiListe.clear();	

            while ((pos = regex.indexIn(replyString, pos)) != -1) {
                dateiListe << regex.cap(1);
                pos += regex.matchedLength();
                capCount++;
            }
                      
            QRegExp innerRx("([NS])(\\d{2})([EW])(\\d{3})");
            for (int i = 0; i < capCount; i++){
                int po = innerRx.indexIn(dateiListe[i]);
                if (po > -1){
                    lat = innerRx.cap(2).toInt();
                    lon = innerRx.cap(4).toInt();
                    if (innerRx.cap(1) == "S") {
                        lat = -lat;
                    }
                    if (innerRx.cap(3) == "W") {
                        lon = - lon;
                    }
                    //S00E000.hgt.zip
                    //123456789012345 => 15 bytes long
                    fileList[latLonToIndex(lat, lon)] = continent+"/"+dateiListe[i].right(15);
                }
        
            }
             
        }
        else
        {
            std::cout << "Fehler beim laden der Daten für " << continent << "." << std::endl;
        }
    } // end: foreach(QString continent, continents)
    
    QFileInfo fileInfo(_cachedir + _srtmFileList);
    QString filePath = fileInfo.absolutePath();
    QDir makedir;
    makedir.mkpath(filePath);
    QFile file(_cachedir + _srtmFileList);
    if (!file.open(QIODevice::WriteOnly)) { 
        std::cerr << "Could not open file " << _cachedir << _srtmFileList << std::endl;
        //Not a fatal error. We just can't cache the list.
        return;
    }
    QDataStream stream(&file);
    stream << fileList;
    file.close();
    
} 

void SRTMProvider::downloadUrl(QUrl &dUrl, QString &data)
{
    FileDownloader loader;
    data = QString(loader.downloadURL(dUrl));
    return;    
}

void SRTMProvider::downloadUrl(QUrl &dUrl, QByteArray &data)
{
    FileDownloader loader;
    data = loader.downloadURL(dUrl);
    return;    
}

SRTMProvider::~SRTMProvider()
{	
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// SRTMTile /////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

bool SRTMTile::fillTile(QString fileName){
    valid = false;

    resolution = SrtmZipFile::getData(fileName, &buffer); //Pixeldichte (Pixel entlang einer Seite) im Tile

    if(resolution > 0){ // srtmZipFileObject.getData hat nicht den Defaultwert zurückgegeben
        valid = true;
    }

    return valid;
}

int SRTMTile::getPixelValue(int x, int y)
        {
    qint16 value = 0;
    if(x >= 0 && x < resolution && y >= 0 && y < resolution){
        int offset = x + resolution * (resolution - y - 1);
        value = qFromBigEndian(buffer[offset]);
    }
    return value;
}

double SRTMTile::getAltitudeFromLatLon(double lat, double lon)
{
    if (!valid) return SRTM_DATA_VOID;
    lat -= int(floor(lat));
    lon -= int(floor(lon));
    double value__ = 0.0; // Defaultwert für Höhe ist NN
    if(lat >= 0.0 && lat < 1.0 && lon >= 0.0 && lon < 1.0){
        double x = lon * (resolution - 1);
        double y = lat * (resolution - 1);
        /* Variable names:
            valueXY with X,Y as offset from calculated value, _ for average
        */
        double value00 = getPixelValue(x, y);
        double value10 = getPixelValue(x+1, y);
        double value01 = getPixelValue(x, y+1);
        double value11 = getPixelValue(x+1, y+1);
        double value_0 = avg(value00, value10, x-int(x));
        double value_1 = avg(value01, value11, x-int(x));
        value__ = avg(value_0, value_1, y-int(y));
        // std::cerr << "Haben jetzt statt Defaulthöhe: "<< QString::number(value__, 'g', 3) << std::endl; // Testausgabe
    }
    return value__;
}

SRTMTile::~SRTMTile()
{
    if (buffer) delete buffer;
}


/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// Tests ////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
namespace biker_tests
{
	int testSRTMProvider()
	{
		      
        SRTMProvider s;
        CHECK_EQ_TYPE(s.getAltitude(51.527, 16.96), 107, double); // Tetraeder in Bottrop
        CHECK_EQ_TYPE(s.getAltitude(51.427, 16.86), 97.6, double); // Zweiter Wert im gleichen Tile

        return EXIT_SUCCESS;
	}
}

#endif //ZZIP_FOUND
