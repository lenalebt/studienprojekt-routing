#include "altitudeprovider.hpp"

#include <QtGlobal>
#include <QMutexLocker>
#include <math.h>
#include <QtEndian>

// TODO
//  Downloaden läuft jetzt.
// - Was man noch machen könnte: Eine ausgeklügeltere Fehlerbehandlung, falls Download schief läuft.
//  Momentan führt jeder Fehler in der Verarbeitung dazu, dass als ermittelter Höhenwer NN zurückgegeben wird.
// - Was auch noch nicht fertig ist, ist die Behandlung von falschen SRTM-Werten. Da muss noch eine schöne Lösung für her: Gewichtete Höhe aus umliegenden Werten.
// - Das öffnen der Zipdateien läuft auch noch nicht. Oder es liegt am Speichern der selbigen.
// - Schön gemacht werden muss auch noch, dass falls Datei, bzw Ordner nicht vorhanden sind, der Ordner noch erstellt wird.
// - Speichern der fileList in eine Datei mit Endung? Um dem Betriebssystem den Umgang zu erleichtern?
//
// ghc wegschmeißen!!!!!!!

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
 
    QString fileName; // hier soll die Zipdatei liegen nach dem Download, also Name incl. Pfad (wird aus _cachedir und Index erstellt)

         
    loadFileList(); // per loadFilelist(): fileListe vorhanden? falls nicht, laden!
    intlat = int(floor(lat)); //Nachkommastellen der Koordinaten entfernen und zu int casten
    intlon = int(floor(lon));
    
    if (fileList.contains(latLonToIndex(intlat, intlon))){// Falls Koordinate vorhanden..
        
        std::cerr << "Gesuchte Koordinate ist in filelist." << std::endl;
        fileName = _cachedir+fileList[latLonToIndex(intlat,intlon)];
        
		QFile zipFile(fileName);
        
        if(!zipFile.open(QIODevice::ReadOnly)){
            
            std::cerr << "Entsprechendes Zipfile konnte nicht geöffnet werden, soll nun runtetgeladen werden." << std::endl;
            QString altZipUrl =  _url.toString() + fileList[latLonToIndex(intlat, intlon)]; //Url bis .hgt.zip
            altZipUrl.toAscii().constData();
            QUrl srtmUrl(altZipUrl);
            QByteArray data;
            
            std::cerr << "Zipfile soll nun runtergeladen werden." << std::endl;
            downloadUrl(srtmUrl, data);
            
            if(data.isEmpty()){ // Download nicht geglückt
                std::cout << "Fehler beim downloaden der Daten für " << fileList[latLonToIndex(intlat, intlon)] << "." << std::endl;
                return altitude;
            }
            std::cerr << "Irgendwas wurde auch runtergeladen." << std::endl;
            
            //QDir makedir;
            //makedir.mkpath(_cachedir);
            zipFile.open(QIODevice::WriteOnly);
            QDataStream zipFileOutStream(&zipFile);
            zipFileOutStream << data;            
            zipFile.close(); 
                       
            //if(!zipFile.open(QIODevice::ReadOnly)){
                //std::cout << "Fehler beim öffnen des Downloads der Daten für " << fileList[latLonToIndex(intlat, intlon)] << "." << std::endl;
                //return altitude;
            //}
        }
        // [TODO] Zip-Dateien evtl geöffnet lassen/im Speicher lassen, damit es schneller wird.
        
        SrtmZipFile srtmZipFileObject;//Zip-Datei entzippen:
        std::cerr << "Soll jetzt entzippt werden." << std::endl;
        resolution = srtmZipFileObject.getData(fileName, &buffer); //Pixeldichte (Pixel entlang einer Seite) im Tile
        std::cerr << "Bei entzippen ermittelte Auflösung der entsippten Kachel: " << QString::number(resolution, 10) << std::endl;
        if(resolution > 0){ // srtmZipFileObject.getData hat nicht den Defaultwert zurückgegeben
            valid = true;
            altitude = getAltitudeFromLatLon(lat, lon);
        }
        
    }
    
    std::cerr << "Altitide wurde zu " << QString::number(altitude, 'g', 3) << " berechnet." << std::endl;
    if (buffer) delete buffer;        
    return altitude;
}

int SRTMProvider::getPixelValue(int x, int y)
	{
    qint16 value = 0;
    if(x >= 0 && x < resolution && y >= 0 && y < resolution){
        int offset = x + resolution * (resolution - y - 1);
        value = qFromBigEndian(buffer[offset]);
    }
    return value;
}

double SRTMProvider::getAltitudeFromLatLon(double lat, double lon)
{
    std::cerr << "Sind nun in getAltitideFromLatLon angekommen." << std::endl;
    if (!valid) return SRTM_DATA_VOID;
    lat -= intlat;
    lon -= intlon;
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
        std::cerr << "Haben jetzt statt Defaulthöhe: "<< QString::number(value__, 'g', 3) << std::endl;
    }
    return value__;
}

void SRTMProvider::loadFileList()
{
    QFile file(_cachedir + _srtmFileList);    
    std::cerr << "In loadFileList." << std::endl;
    if (!file.open(QIODevice::ReadOnly)) {   
        std::cerr << "Filelist konnte nicht geöffnet werden, soll nun erstellt werden." << std::endl;
        createFileList();
        return;
    }   
    std::cerr << "Filelist konnte geöffnet werden." << std::endl;
    QDataStream stream(&file);
    stream >> fileList;
    file.close();   
    std::cerr << "In Filelist sollte jetzt aus Datei geladen sein." << std::endl;
    return;
}

void SRTMProvider::createFileList()
{
    
    std::cerr << "In createFileList angekommen." << std::endl;
    
    QStringList continents;
    continents << "Africa" << "Australia" << "Eurasia" << "Islands" << "North_America" << "South_America";

    QString url = _url.toString();
    
    QStringList dateiListe;	
    int capCount = 0;
    int lat;
    int lon;
    int pos = 0;
    
    foreach (QString continent, continents) { // für jeden Kontinent, die vorhandenen Ziparchive in die Liste eintragen
        std::cout << "Downloading data from " << url+continent+"/" << std::endl;

        QString urlTemp = QString(url+continent+"/").toAscii().constData(); // Kontinent zur url hinzufügen
        QUrl srtmUrl(urlTemp); 				 // urlTemp zu QUrl casten, für spätere Verwendung.
        QString replyString; 					// Hierein wird später die NetworkReply aus downloadUrl gespeichert.

        
        downloadUrl(srtmUrl, replyString);

        if(!replyString.isEmpty()) 		 // Bearbeiten der Liste, falls Herunterladen erfolgreich.
        {
            // Download nach Listenelementen durchsuchen und diese in fileList eintragen.
            std::cerr << "Irgendwas haben wir runtergeladen." << std::endl;
            
            std::cerr << replyString << std::endl;
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
            std::cerr << "capCount: " << QString::number(capCount, 10) << std::endl;
            std::cerr << "Länge von dateiListe: " << QString::number(dateiListe.length(), 10) << std::endl;
                      
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
             
        std::cerr << "Letzter Eintrag in fileList: Index: " << QString::number(latLonToIndex(lat, lon), 10) << " Datei: " << continent << "/" << dateiListe[capCount-1].right(15) << std::endl;
            
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
    
    //QDir makedir;
    //makedir.mkpath(_cachedir);
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
    //QUrl url = QUrl(dUrl);
    // QFuture<QByteArray> future = QtConcurrent::run(&loader, &FileDownloader::downloadURL, dUrl);
    data = QString(loader.downloadURL(dUrl)); //future.result());
    return;    
}

void SRTMProvider::downloadUrl(QUrl &dUrl, QByteArray &data)
{
    FileDownloader loader;
    //QUrl url = QUrl(dUrl);
    //QFuture<QByteArray> future = QtConcurrent::run(&loader, &FileDownloader::downloadURL, dUrl);
    data = loader.downloadURL(dUrl);//future.result();
    return;    
}

SRTMProvider::~SRTMProvider()
{	
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// Klasse SRTMProvider //////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
FileDownloader::FileDownloader():QObject()
{
    //manager = new QNetworkAccessManager(this);//this);
    //connect(manager, SIGNAL(finished(QNetworkReply*)),
         //this, SLOT(replyFinished(QNetworkReply* r)));
} 
FileDownloader::~FileDownloader()
{
}

//void FileDownloader::run()
//{
    ////Wird aufgerufen, wenn start() gestartet wird
//}

//void FileDownloader::replyFinished(QNetworkReply* r){
    //finished = true;
//}

QByteArray FileDownloader::downloadURL(QUrl &url)
{
    
    QNetworkAccessManager manager;
    QNetworkRequest request(url);    
    QNetworkReply *reply = manager.get(request);
    QByteArray data;

    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    
    if(reply->error() == QNetworkReply::NoError){
        data = reply->readAll();
    }
    
    delete reply;
    return data;
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
        s.getAltitude(51.457, 7.014);
		
		return EXIT_SUCCESS;
	}
}
