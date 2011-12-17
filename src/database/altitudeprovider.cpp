#include "altitudeprovider.hpp"
#include "tests.hpp"

#include <QtGlobal>
#include <QMutexLocker>

double SRTMProvider::getAltitude(const GPSPosition& pos)
{
    return this->getAltitude(pos.getLat(), pos.getLon());
}

double SRTMProvider::getAltitude(double lat, double lon)
{
    /*TODO:
     * fileListe vorhanden?
     *      falls nein, laden!
     * Koordinate in fileList?
     *      falls nein, Koordinate gibt es nicht!
     * sonst:
    - Zip-Datei für die Koordinate schon vorhanden?
    - Zip-Dateien runterladen, wenn sie noch nicht vorhanden sind.
    - Zip-Datei öffnen und Inhalt im Speicher ablegen.
    - Zip-Dateien evtl geöffnet lassen/im Speicher lassen, damit es schneller wird.
    - Koordinaten aus dem Array raussuchen und Mittelwert berechnen.
    */
    
    return 0.0;//TODO
}

void SRTMProvider::createFileList()
{
    
    
    QStringList continents;
    continents << "Africa" << "Australia" << "Eurasia" << "Islands" << "North_America" << "South_America";
    QString url = "http://dds.cr.usgs.gov/srtm/version2_1/SRTM3/";
    
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

QByteArray SRTMProvider::blubb(QUrl &url)
{
    FileDownloader loader;
    return loader.downloadURL(url);
}

void SRTMProvider::downloadUrl(const QUrl &url, QString &data)
{
    //Instanz von FileDownloader erstellen
    //FileDownloader loader; // oder nur FileDownloader loader?
    //ihr wollt "simulieren":
    //return loader.downloadURL(url);
    extern QByteArray blubb(QUrl &url); // Warum will er nicht loader.download...usw.?!?
    QFuture<QByteArray> future = QtConcurrent::run(blubb, url);
    // future.waitForFinished(); // result() müsste auch blocken
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

QByteArray FileDownloader::downloadURL(QUrl url)
{
    //hier der Kram aus der alten downloadurl-funktion
    QNetworkAccessManager manager;
    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);
    QByteArray data;


    //QEventLoop loop;
    //QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    //loop.exec();

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
