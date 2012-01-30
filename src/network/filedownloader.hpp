#ifndef FILEDOWNLOADER_HPP
#define FILEDOWNLOADER_HPP

#include "tests.hpp"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QNetworkRequest>
#include <QEventLoop>
//#include <boost/cstdint.hpp>
//#include <QFile>
//#include <QDir>
//#include <QMap>
//#include <QCache>
//#include <QRegExp>
//#include <QThread>
//#include <QFuture>
//#include <QtConcurrentRun>
//#include <QDataStream>
//#include <QDir>
//#include <QFileInfo>

/**
 * @brief 
 * 
 * 
 * 
 * @author Claudia Rudolph, Pia-Lucia Triebel
 * @date 2012-01-30
 * @copyright GNU GPL v3
 * @todo Doxygen, Test
 */
class FileDownloader: public QObject
{
private:
public:
    FileDownloader();
    ~FileDownloader();
    QByteArray downloadURL(QUrl &url);
    //QByteArray downloadURL(QUrl url, QNetworkReply::NetworkError *error); // vll TODO
};

namespace biker_tests
{
	int testFileDownloader();
}

#endif //FILEDOWNLOADER_HPP
