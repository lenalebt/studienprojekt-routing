#include "filedownloader.hpp"
#include <QDebug>
#include <iostream>
#include "webserver.hpp"
#include "programoptions.hpp"

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
    
    QByteArray data;
    QNetworkReply::NetworkError error;

    return data = downloadURL(url, error);
}

QByteArray FileDownloader::downloadURL(const QUrl &url, QNetworkReply::NetworkError &error)
{

    QNetworkAccessManager manager;
    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);
    QByteArray data;

    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    error = reply->error();
    if(error == QNetworkReply::NoError){
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
            FileDownloader fd;
            QByteArray qba = fd.downloadURL(QUrl("http://www.openstreetmap.de/faq.html"));
            CHECK(!qba.isNull());

            ProgramOptions::getInstance()->webserver_public_html_folder = "./gui/";
            HttpServerThread<BikerHttpRequestProcessor> server(8081);
            server.startServer();
            FileDownloader downloader;
            QByteArray gui_html = downloader.downloadURL(QUrl("http://localhost:8081/files/gui.html"));
            CHECK(gui_html.size()>0);


            return EXIT_SUCCESS;
	}
}
