#include "gpsroute.hpp" 

#include "gpsposition.hpp"
#include "routingnode.hpp"

static void GPSRoute::exportGPX(QString filename, GPSRoute r)
{
    QDomDocument doc;
    //zuerst das Grundelement erstellen, dann runter bis die Wegpunkte eingefügt werden.
    QDomElement root = doc.createElement("gpx");
    root.setAttribute("version", "1.0");
    root.setAttribute("xmlns", "http://www.topografix.com/GPX/1/0");			//TODO: Werte dazu!
    root.setAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    root.setAttribute("xsi:schemaLocation", "http://www.topografix.com/GPX/1/0 http://www.topografix.com/GPX/1/0/gpx.xsd");
    root.setAttribute("creator", "Biker https://github.com/lenalebt/Biker");
    doc.appendChild(root);
    
    QDomNode node( doc.createProcessingInstruction( "xml", "version=\"1.0\" standalone=\"no\"" ) );
    doc.insertBefore(node, doc.firstChild());

    QDomElement wptNode = doc.createElement("wpt");
    root.appendChild(trkNode);

    QDomElement trkSegNode = doc.createElement("trkseg");
    trkNode.appendChild(trkSegNode);

    QDomElement trkPoint;
    QLocale locale(QLocale::C); //sonst schreibt er in eine GPX-Datei Kommas statt Punkte
    //Wegpunkt zur Liste hinzutun
    for (int i=0; i<r.size(); i++)
    {
        trkPoint = doc.createElement("trkpt");
        trkPoint.setAttribute("lon", locale.toString(r.getWaypoint(i).getLon()));
        trkPoint.setAttribute("lat", locale.toString(r.getWaypoint(i).getLat()));
        trkSegNode.appendChild(trkPoint);
    }

    //Datei öffnen und so
    QFile file(gpxFilename);
    if (!file.open(QIODevice::WriteOnly))	//Versuche, die Datei zu öffnen
    {
        std::cerr << "Opening file for writing failed." << std::endl;
        throw std::ios_base::failure("Opening file for writing failed.");
        return;
    }
    //jetzt noch Daten wegschreiben
    QTextStream stream(&file);
    stream << doc.toString();
    file.close();
}

namespace biker_tests
{
    int testGPSRoute()
    {
        RoutingNode node;
        GPSPosition pos;
        
        QVector<GPSPosition> list;
        list << pos;
        list << node;
        
        return EXIT_SUCCESS;
    }
}
