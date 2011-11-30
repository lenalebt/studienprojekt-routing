#include "altitudeprovider.hpp"
#include "tests.hpp"

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
    QString url = "http://dds.cr.usgs.gov/srtm/version2_1/SRTM3/"
    
    foreach (QString continent, continents) {
        std::cout << "Downloading data from" << url+continent+"/";

        QString urlTemp = QString(url+continent+"/").toAscii().constData()); // Kontinent zur url hinzufügen
        srtmUrl = QUrl( const &urlTemp); 									 // urlTemp zu QUrl casten, für spätere Verwendung.
		QString replyString; 												 // Hierein wird später die NetworkReply aus downloadUrl gespeichert.
		QNetworkReply::NetworkError error = downloadUrl(srtmUrl, replyString);
		
		if(error = QNetworkReply::NoError) 									 // Bearbeiten der Liste, falls Herunterladen erfolgreich.
		{
			// Download nach Listenelementen durchduchen udn diese in fileList eintragen.
			
			// mit cap() durchgehen über alle gefunden captures, mit caplength vorher Anzahl holen.
			/*foreach (Match match in Regex.Matches(replyString, "<li>\s*<a\s+href=\"([^\"]+)\"))
			{
			String placeHolder = match.cap(i).Value;

			* */

			int index = -1;
			while ((index = curlData.indexOf(regex, index+1)) != -1) {
				int lat = regex.cap(2).toInt();
				int lon = regex.cap(4).toInt();
				if (regex.cap(1) == "S") {
					lat = -lat;
				}
				if (regex.cap(3) == "W") {
					lon = - lon;
				}
				//S00E000.hgt.zip
				//123456789012345 => 15 bytes long
				fileList[latLonToIndex(lat, lon)] = continent+"/"+regex.cap().right(15);
			}
		}
		else
		{
			// TODO
			std::cout << "Fehler beim laden der Daten für " << continent << "." << std::endl;
		}
    }
    
    if (fileList.size() != SRTM_FILE_COUNT) {
        std::cerr << "Could not download complete list of tiles from SRTM server. Got" << fileList.size() << "tiles but" << SRTM_FILE_COUNT << "were expected.";
        //exit(1); //ERROR: SRTM-Filecount was wrong. Should not matter to comment this out.
    }
    
    QFile file(cachedir+"srtmfilelist");
    if (!file.open(QIODevice::WriteOnly)) {
        std::cerr << "Could not open file" << cachedir+"filelist";
        //Not a fatal error. We just can't cache the list.
        return;
    }
    QDataStream stream(&file);
    stream << fileList;
    file.close();
    */
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
    
}
