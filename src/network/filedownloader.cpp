#include "filedownloader.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
// Klasse FileDownloader ////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
FileDownloader::FileDownloader():QObject()
{
} 

FileDownloader::~FileDownloader()
{
}

QByteArray FileDownloader::downloadURL(const QUrl &url)
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
	int testFileDownloader()
	{
		return EXIT_SUCCESS;
	}
}
