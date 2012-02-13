#ifndef FILEDOWNLOADER_HPP
#define FILEDOWNLOADER_HPP

#include "tests.hpp"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QNetworkRequest>
#include <QEventLoop>

/**
 * @brief Mit dieser Klasse können einfache http-Downloads vorgenommen werden.
 * 
 * Es gibt keine Fehlerbehandlung für missglückte Downloads.
 * 
 * 
 * 
 * @author Claudia Rudolph, Pia-Lucia Triebel
 * @date 2012-01-30
 * @copyright GNU GPL v3
 * @todo Doxygen, Test
 * @ingroup network
 */
class FileDownloader: public QObject
{
private:
public:
    FileDownloader();
    ~FileDownloader();
    /**
     * @brief Führt Download für angegebene Url aus.
     * 
     * Dabei wird der Punkt über seine GPSPosition angegeben.
     * 
     * @param url Die Url für die der Download vorgenommen werden soll (muss inkl. http:// angegeben werden).
     * @return Die vollständige Netzwerkantwort (ist leer falls Download gescheitert ist).
     */
    QByteArray downloadURL(const QUrl &url);
    /**
     * @brief Führt Download für angegebene Url aus.
     *
     * Dabei wird der Punkt über seine GPSPosition angegeben.
     *
     * @param url Die Url für die der Download vorgenommen werden soll (muss inkl. http:// angegeben werden).
     * @param error Hier wird der für den Download zutreffende Errortyp eingetragen.
     * @return Die vollständige Netzwerkantwort (ist leer falls Download gescheitert ist).
     */
    QByteArray downloadURL(const QUrl &url, QNetworkReply::NetworkError &error);
};

namespace biker_tests
{
	int testFileDownloader();
}

#endif //FILEDOWNLOADER_HPP
