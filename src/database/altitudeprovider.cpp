#include "altitudeprovider.hpp"
#include "tests.hpp"

#include <QtGlobal>

double SRTMProvider::getAltitude(const GPSPosition& pos)
{
    return this->getAltitude(pos.getLat(), pos.getLon());
}

double SRTMProvider::getAltitude(double lat, double lon)
{
    /*TODO:
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
        std::cout << "Downloading data from" << url+continent+"/";

        QString urlTemp = QString(url+continent+"/").toAscii().constData(); // Kontinent zur url hinzufügen
        QUrl srtmUrl(urlTemp); 				 // urlTemp zu QUrl casten, für spätere Verwendung.
        QString replyString; 							// Hierein wird später die NetworkReply aus downloadUrl gespeichert.
        QNetworkReply::NetworkError error = downloadUrl(srtmUrl, replyString);

        if(error == QNetworkReply::NoError) 		 // Bearbeiten der Liste, falls Herunterladen erfolgreich.
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
    
    QFile file("srtmfilelist"); // TODO cachedir+"filelist" wobei cachedir den Pfad enthalten soll
    if (!file.open(QIODevice::WriteOnly)) { 
        std::cerr << "Could not open file" << "filelist"; // TODO cachedir+"filelist" wobei cachedir den Pfad enthalten soll
        //Not a fatal error. We just can't cache the list.
        return;
    }
    QDataStream stream(&file);
    stream << fileList;
    file.close();
    
}
/*
 * Aus Forum: http://stackoverflow.com/questions/2572985/how-can-i-use-qt-to-get-html-code-of-the-redirected-page
 * Stand: 30.11.2011 15:44h
 * User: Claire Huang
 * Wurde angepasst.
*/
QNetworkReply::NetworkError SRTMProvider::downloadUrl(const QUrl &url, QString &data)
{
    QNetworkAccessManager manager;
    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);

    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if (reply->error() != QNetworkReply::NoError)
    {
        return reply->error();
    }
    data = QString(reply->readAll());
    delete reply;
    return QNetworkReply::NoError;
}

/*SRTMProvider::~SRTMProvider()
{
    
}*/
