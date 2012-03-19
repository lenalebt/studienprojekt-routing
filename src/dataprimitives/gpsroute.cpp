#include "gpsroute.hpp" 
#include "gpsposition.hpp"
#include "routingnode.hpp"
// wird benötigt zum schreiben von Dokumenten
#include <limits>
#include <QtXml>
#include <stdexcept>
#include <iostream>



QString GPSRoute::exportGPXString(boost::shared_ptr<AltitudeProvider> provider)
{
    GPSRoute route = *this;
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
    QDomText distanceText = doc.createTextNode(locale.toString(route.calcLength(), 'f', 9).replace(",",""));
    rootDistance.appendChild(distanceText);
    rootExtensions.appendChild(rootDistance);
    
    QDomElement rootTime = doc.createElement("time");
    QDomText timeText = doc.createTextNode(locale.toString(route.getDuration(), 'f', 9).replace(",",""));
    rootTime.appendChild(timeText);
    rootExtensions.appendChild(rootTime);
    
    root.appendChild(rootExtensions);
    //Wegpunkte einfügen
    QDomElement wptPoint;
    //Wegpunkt zur Liste hinzutun
    for (int i=0; i<route.getSize(); i++)
    {
        wptPoint = doc.createElement("wpt");
        wptPoint.setAttribute("lat", locale.toString(route[i].getLat(), 'f', 9));
        wptPoint.setAttribute("lon", locale.toString(route[i].getLon(), 'f', 9));
        if (provider.get() != 0)
        {
            QDomElement elevation = doc.createElement("ele");
            QDomText elevationText = doc.createTextNode(locale.toString(provider->getAltitude(route[i]), 'f', 6).replace(",",""));
            elevation.appendChild(elevationText);
            wptPoint.appendChild(elevation);
        }
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
        rtePoint.setAttribute("lat", locale.toString(route[i].getLat(), 'f', 9));
        rtePoint.setAttribute("lon", locale.toString(route[i].getLon(), 'f', 9));
        QDomElement extensions = doc.createElement("extensions");
        QDomElement distance = doc.createElement("distance");
        QDomText distanceElementText = doc.createTextNode(locale.toString(route[i].calcDistance(help), 'f', 9).replace(",",""));
        distance.appendChild(distanceElementText);
        extensions.appendChild(distance);
        if (provider.get() != 0)
        {
            QDomElement elevation = doc.createElement("ele");
            QDomText elevationText = doc.createTextNode(locale.toString(provider->getAltitude(route[i]), 'f', 6).replace(",",""));
            elevation.appendChild(elevationText);
            rtePoint.appendChild(elevation);
        }
        // AUSKOMMENTIERT: eventuel zur späteren Verwendung 
        //QDomElement time = doc.createElement("time");
        //extensions.appendChild(time);
        rtePoint.appendChild(extensions);
        rteNode.appendChild(rtePoint);
        help = route[i];
    }
    QString text;
    // String ausgeben
    text = doc.toString();
    return text;
}

void GPSRoute::exportGPX(QString filename, boost::shared_ptr<AltitudeProvider> provider)
{
    // Text-String erstellen
    QString text = this->exportGPXString();
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
    stream << text;
    file.close();
}

QString GPSRoute::exportJSONString()
{
    GPSRoute route = *this;
    QLocale locale(QLocale::C);//sonst schreibt er in eine GPX-Datei Kommas statt Punkte
    QString all;
    //Version
    all.append("{\"version\":\"0.3\",");
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
        all.append(locale.toString(route[i].getLat(), 'f', 9));
        all.append(",");
        all.append(locale.toString(route[i].getLon(), 'f', 9));
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
    //QString zurückgeben
    return all;
}

// ja, ide Art und Weise der Implementierung ist etwas unschön, aber sie sollte laufen :)
void GPSRoute::exportJSON(QString filename)
{
    // QString übernehmen
    QString all = this->exportJSONString();
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
        GPSPosition pos_one(48.333333333, 2.123456789);
        GPSPosition pos_two(52.6, 13.00091);
        GPSPosition pos_three(51.0, 0.73098);
        GPSPosition pos_test(1.23344, 0.0);
        // neue Route mit zweitem Wert zuerst eingefügt
        GPSRoute test(pos_two);
        GPSRoute test2(test);
        // nun sollte zumindest ein Element in der Liste drin sein
        CHECK_EQ(test.isEmpty(), false);
        // nun die anderen beiden Positionen einfügen 
        test.insertBackward(pos_one);
        test.insertForward(pos_three);
        //einen Klon
        test2 = test;
        // ... und die test-Position
        test = test << pos_test;
        // und beide Routen zusammenfügen
        test << test2;
        // oops ... falsch herum  ;)
        test.reverse();
        // die Route exportieren
        test.exportGPX("test.gpx");
        test.exportJSON("test.js");
        // nun sollte pos_one am anfang stehen
        CHECK_EQ(test.getStartingPoint(), pos_one);
        // nun sollte pos_three am ende stehen
        CHECK_EQ(test.getDestination(), pos_three);
        // nun wird alles entfernt
        test.clear();
        // und dann sollte alles leer sein . . . 
        CHECK(test.isEmpty());
        // und das wäre dann der Test
        return EXIT_SUCCESS;
    }
}

