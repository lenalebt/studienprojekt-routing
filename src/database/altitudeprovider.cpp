#include "altitudeprovider.hpp"
#include "tests.hpp"

#include <QtGlobal>
#include <QMutexLocker>
#include <math.h>
#include <QtEndian>

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// Klasse SRTMProvider //////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
double SRTMProvider::getAltitude(const GPSPosition& pos)
{
    return this->getAltitude(pos.getLat(), pos.getLon());
}

double SRTMProvider::getAltitude(double lat, double lon)
{
    double altitude = 0.0; // Defaultwert für Höhe ist NN
 
    QString fileName; // hier soll die Zipdatei liegen nach dem Download, also Name incl. Pfad

         
    loadFileList(); // per loadFilelist(): fileListe vorhanden? falls nicht, laden!
    intlat = int(floor(lat)); //Nachkommastellen der Koordinaten entfernen und zu int casten
    intlon = int(floor(lon));
    
    if (fileList.contains(latLonToIndex(intlat, intlon))){// Falls Koordinate vorhanden..
        fileName = _cachedir+fileList[latLonToIndex(intlat,intlon)];
		QFile zipFile(fileName);
        
        if(!zipFile.open(QIODevice::ReadOnly)){
            QString altZipDir =  fileList[latLonToIndex(intlat, intlon)]; // Url ab Kontinentverzeichnis bis .hgt.zip
            // [TODO] Zip-Dateien runterladen, wenn sie noch nicht vorhanden sind. //TODO
            // [TODO] Zip-Datei unter filename ablegen (Pfad in filename)
        }
        // [TODO] Zip-Dateien evtl geöffnet lassen/im Speicher lassen, damit es schneller wird.
        
        SrtmZipFile srtmZipFileObject;//Zip-Datei entzippen:
        resolution = srtmZipFileObject.getData(fileName, &buffer); //Pixeldichte (Pixel entlang einer Seite) im Tile
        valid = true;
        altitude = getAltitudeFromLatLon(lat, lon);
        //
        //
        ///** Get the value of a pixel from the data using a coordinate system
  //* starting in the upper left (NW) edge growing to the lower right
  //* egde (SE) instead of the SRTM coordinate system.
  //*/
    }
            //- Koordinaten aus dem Array raussuchen und Mittelwert berechne
    
    if (buffer) delete buffer;        
    return altitude;//TODO
}
int SRTMProvider::getPixelValue(int x, int y)
	{
    //Q_ASSERT(x >= 0 && x < resolution && y >= 0 && y < resolution);
    int offset = x + resolution * (resolution - y - 1);
    qint16 value;
    value = qFromBigEndian(buffer[offset]);
    return value;
}

///** Gets the altitude in meters for a given coordinate. */
double SRTMProvider::getAltitudeFromLatLon(double lat, double lon)
{
    if (!valid) return SRTM_DATA_VOID;
    lat -= intlat;
    lon -= intlon;
    //Q_ASSERT(lat >= 0.0 && lat < 1.0 && lon >= 0.0 && lon < 1.0);
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
    double value__ = avg(value_0, value_1, y-int(y));
    return value__;
}


void SRTMProvider::loadFileList()
{
    QFile file(_cachedir+"srtmfilelist");
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
    
    foreach (QString continent, continents) { // für jeden Kontinent, die vorhandenen Ziparchive in die Liste eintragen
        std::cout << "Downloading data from " << url+continent+"/" << std::endl;

        QString urlTemp = QString(url+continent+"/").toAscii().constData(); // Kontinent zur url hinzufügen
        QUrl srtmUrl(urlTemp); 				 // urlTemp zu QUrl casten, für spätere Verwendung.
        QString replyString; 					// Hierein wird später die NetworkReply aus downloadUrl gespeichert.
        
        downloadUrl(srtmUrl, replyString);

        if(!replyString.isEmpty()) 		 // Bearbeiten der Liste, falls Herunterladen erfolgreich.
        {
            // Download nach Listenelementen durchsuchen und diese in fileList eintragen.
            
            QRegExp regex("(<li>\\s*<a\\s+href=\"([^\"]+)\\)");
            regex.indexIn(replyString);
            QStringList dateiListe = regex.capturedTexts();			
            int capCount = 	regex.captureCount();
                        
            QRegExp innerRx("([NS])(\\d{2})([EW])(\\d{3})");
            for (int i=1;i<=capCount;i++){
                int lat = innerRx.cap(2).toInt();
                int lon = innerRx.cap(4).toInt();
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
        else
        {
            // TODO
            std::cout << "Fehler beim laden der Daten für " << continent << "." << std::endl;
        }
    }
    
    //if (fileList.size() != SRTM_FILE_COUNT) {
        //std::cerr << "Could not download complete list of tiles from SRTM server. Got" << fileList.size() << "tiles but" << SRTM_FILE_COUNT << "were expected.";
        ////exit(1); //ERROR: SRTM-Filecount was wrong. Should not matter to comment this out.
    //}
    
    QFile file(_cachedir + "srtmfilelist");
    if (!file.open(QIODevice::WriteOnly)) { 
        std::cerr << "Could not open file " << _cachedir << "srtmfilelist" << std::endl;
        //Not a fatal error. We just can't cache the list.
        return;
    }
    QDataStream stream(&file);
    stream << fileList;
    file.close();
    
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// Klasse SRTMProvider //////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
void SRTMProvider::downloadUrl(const QUrl &dUrl, QString &data)
{
    FileDownloader loader;
    QFuture<QByteArray> future = QtConcurrent::run(&loader, &FileDownloader::downloadURL, dUrl);
    data = QString(future.result());
    return;
    
}

SRTMProvider::~SRTMProvider()
{
	
}



FileDownloader::FileDownloader():QThread()
{
}
  


void FileDownloader::run()
{
    //Wird aufgerufen, wenn start() gestartet wird
}

QByteArray FileDownloader::downloadURL(QUrl &url)
{
    //hier der Kram aus der alten downloadurl-funktion
    QNetworkAccessManager manager;
    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);
    QByteArray data;

    while (reply->isRunning())
    {
		wait(5);
		
	}
    
    data = reply->readAll();
    return data;
}

namespace biker_tests
{
	int testSRTMProvider()
	{
		SRTMProvider s;
		
		return EXIT_SUCCESS;
	}
}
