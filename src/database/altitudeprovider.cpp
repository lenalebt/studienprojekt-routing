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
    /*
    //Store data in memory
    //curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_data_callback);
    //curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
    //TODO: Hier Qt benutzen!
    
    QStringList continents;
    continents << "Africa" << "Australia" << "Eurasia" << "Islands" << "North_America" << "South_America";
    foreach (QString continent, continents) {
        std::cerr << "Downloading data from" << url+continent+"/";
        //curlData.clear();
        //curl_easy_setopt(curl, CURLOPT_URL, QString(url+continent+"/").toAscii().constData());
        //CURLcode error = curl_easy_perform(curl);
        //if (error) {
        //    std::cerr << "Error downloading data for" << continent << "(" << curl_easy_strerror(error) << ")";
        //}
        //TODO: Hier mit Qt alle Dateinamen in dem Ordner runterladen!
        
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
    //curlData.clear(); //Free mem
    
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

/*SRTMProvider::~SRTMProvider()
{
    
}*/
