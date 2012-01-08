#include "gpsroute.hpp" 
#include "gpsposition.hpp"
#include "routingnode.hpp"
// wird benötigt zum schreiben von Dokumenten
#include <limits>
#include <QtXml>
#include <stdexcept>
#include <iostream>

void GPSRoute::exportGPX(QString filename, GPSRoute& route)
{
    QDomDocument doc;
    //zuerst das Grundelement erstellen, dann runter bis die Wegpunkte eingefügt werden.
    QDomElement root = doc.createElement("gpx");
    QLocale locale(QLocale::C);//sonst schreibt er in eine GPX-Datei Kommas statt Punkte
    root.setAttribute("version", "1.0");
    root.setAttribute("xmlns", "http://www.topografix.com/GPX/1/0");			//TODO: Werte dazu!
    root.setAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    root.setAttribute("xsi:schemaLocation", "http://www.topografix.com/GPX/1/0 http://www.topografix.com/GPX/1/0/gpx.xsd");
    root.setAttribute("creator", "Biker https://github.com/lenalebt/Biker"); //TODO: hier etwas aktuelles einfügen!
    doc.appendChild(root);
    QDomNode node( doc.createProcessingInstruction( "xml", "version=\"1.0\" standalone=\"no\"" ) );
    doc.insertBefore(node, doc.firstChild());
    //zusätzliche Daten, wie Zeit & Entfernung
    QDomElement rootExtensions = doc.createElement("extensions");
    QDomElement  rootDistance = doc.createElement("distance");
    QDomText distanceText = doc.createTextNode(locale.toString(route.calcLength()));
    rootDistance.appendChild(distanceText);
    rootExtensions.appendChild(rootDistance);
    /*
     * für eventuelle spätere Verwendung auskommentiert
     * QDomElement rootTime = doc.createElement("time");
     * rootTime.setNodeValue(locale.toString(//Funktions Aufruf für die gesammt Zeit);
     * rootExtensions.appendChild(rootTime);
     */
    root.appendChild(rootExtensions);
    //Wegpunkte einfügen
    QDomElement wptPoint;
    //Wegpunkt zur Liste hinzutun
    for (int i=0; i<route.getSize(); i++)
    {
        wptPoint = doc.createElement("wpt");
        wptPoint.setAttribute("lat", locale.toString(route[i].getLat()));
        wptPoint.setAttribute("lon", locale.toString(route[i].getLon()));
        root.appendChild(wptPoint);
    }
    //Routepunkte einfügen
    //Route-Tag
    QDomElement rteNode = doc.createElement("rte");
    root.appendChild(rteNode);
    //Routepunkte mit Attributen einfügen
    QDomElement rtePoint;
    GPSPosition help = route[0];
    for(int i=0; i<route.getSize(); i++)
    {
        rtePoint = doc.createElement("rtept");
        rtePoint.setAttribute("lat", locale.toString(route[i].getLat()));
        rtePoint.setAttribute("lon", locale.toString(route[i].getLon()));
        QDomElement extensions = doc.createElement("extensions");
        QDomElement distance = doc.createElement("distance");
        QDomText distanceElementText = doc.createTextNode(locale.toString(route[i].calcDistance(help)));
        distance.appendChild(distanceElementText);
        extensions.appendChild(distance);
        // AUSKOMMENTIERT: eventuel zur späteren Verwendung 
        //QDomElement time = doc.createElement("time");
        //extensions.appendChild(time);
        rtePoint.appendChild(extensions);
        rteNode.appendChild(rtePoint);
        help = route[i];
    }
    //Datei öffnen und so
    QFile file(filename);
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

// ja, ide Art und Weise der Implementierung ist etwas unschön, aber sie sollte laufen :)
void GPSRoute::exportJSON(QString filename, GPSRoute& route)
{
    QLocale locale(QLocale::C);//sonst schreibt er in eine GPX-Datei Kommas statt Punkte
    QString all;
    //Version
    all.append("{\"version\":1.0,");
    //Status
    all.append("\"status\":0,");
    //Routen-Zusammenfassung (eventuell verfeinern)
    all.append("\"route_summary\":{},");
    //Routen-Geometrie
    all.append("\"route_geometry\":[");
    //Wegpunkte
    int i=0;
    for(; i<route.getSize()-1; i++)
    {
        all.append("[");
        all.append(locale.toString(route[i].getLat()));
        all.append(",");
        all.append(locale.toString(route[i].getLon()));
        all.append( "],");
    }
    all.append("[");
    all.append(locale.toString(route[i].getLat()));
    all.append(",");
    all.append(locale.toString(route[i].getLon()));
    all.append("]");
    //Instruktionen (eventuell verfeinern)
    all.append("],\"route_instructions\":[]");
    //Ende
    all.append("}");
    //Datei öffnen und so
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))	//Versuche, die Datei zu öffnen
    {
        std::cerr << "Opening file for writing failed." << std::endl;
        throw std::ios_base::failure("Opening file for writing failed.");
        return;
    }
    //jetzt noch Daten wegschreiben
    QTextStream stream(&file);
    stream << all;//doc.toString();
    file.close();
}
namespace biker_tests
{
    /**
     * @todo Umstellen auf das Test-Framework! Tests sind aktuell unvollständig
     * und sehr kurz, und nicht im Log nachvollziehbar. Mit dem Test-Framework
     * (->tests.{c,h}pp) gibt es Zusatzausgaben auf der Konsole. Beispiele,
     * wie das zu verwenden ist, finden sich in den anderen Modulen.
     */
    int testGPSRoute()
    {
        //drei Test-Positionen initalisieren
        GPSPosition pos_one(48.0, 2.0);
        GPSPosition pos_two(52.0, 13.0);
        GPSPosition pos_three(51.0, 0.0);
        // neue Route mit zweitem Wert zuerst eingefügt
        GPSRoute test(pos_two);
        // nun sollte zumindest ein Element in der Liste drin sein
        if(test.isEmpty() == true) return EXIT_FAILURE;
        // nun die anderen beiden Positionen einfügen 
        test.insertBackward(pos_one);
        test.insertForward(pos_three);
        // oops ... falsch herum  ;)
        test.reverse();
        // die Route exportieren
        test.exportGPX("test.gpx", test);
        test.exportJSON("test.js", test);
        // nun sollte pos_one am anfang stehen
        if(test.getStartingPoint()!=pos_one) return EXIT_FAILURE;
        // nun sollte pos_three am ende stehen
        if(test.getDestination()!=pos_three) return EXIT_FAILURE;
        // nun wird alles entfernt
        test.clear();
        // und dann sollte alles leer sein . . . 
        if(test.isEmpty() == false) return EXIT_FAILURE;
        // und das wäre dann der Test
        return EXIT_SUCCESS;
    }
}

