#ifndef MAIN_HPP
#define MAIN_HPP

/**
 * @file
 * @copyright Lizenz: GNU GPL v3
 */

/**
 * @mainpage Einleitung
 * @brief Biker ist ein Programm zum Berechnen von Fahrradrouten.
 * 
 * @section get Bezugsquelle
 * Um den Quellcode des Projekts zu beziehen gibt es 2 Wege:
 * 
 * @subsection get_git Bezug direkt aus dem Quellcodearchiv
 * Nachdem man Zugriff auf das Projektarchiv hat (-> ssh-Schlüssel austauschen!)
 * kann man den Quellcode herunterladen, indem man tippt:
 * @code
 * git clone git@lenalebt.dyndns.org:studienprojekt-routing.git
 * @endcode
 * Anschließend ist im entstandenen Verzeichnis
 * <code>studienprojekt-routing</code> der Quellcode verfügbar.
 * 
 * @subsection get_web Bezug einer Version über http
 * Noch nicht unterstützt. Das Archiv wird dann als
 * <code>.tar.gz</code>, <code>.tar.bz2</code>, oder <code>.zip</code>
 * verfügbar sein. Wer schon Quellcode so erhalten hat
 * kann das Archiv einfach entpacken -
 * es ist alles enthalten.
 * 
 * @section compile Kompilierung
 * Das Programm wird im Quellcode ausgeliefert und muss vom Benutzer selbst
 * übersetzt werden. Binärpakete werden nicht offiziell angeboten.
 * @subsection bibliotheken Bibliotheken
 * Um das Programm zu kompilieren, sind einige Bibliotheken auf dem Zielsystem
 * nötig. Diese sind:
 * @verbatim
   Qt 4.7 oder höher
   Boost 1.42 oder höher mit der Bibliothek program_options
   sqlite 3.7.2 oder höher
   libprotobuf (optional, um .osm.pbf-Dateien zu lesen)
   protoc (optional, s.o.)
   libzzip (optional, um Höhendaten zu verarbeiten)
   spatialite (optional, um ein besseres Datenbankformat benutzen zu können)
   @endverbatim
 * Bei Bibliotheken, bei denen eine Versionsnummer angegeben ist, ist
 * es möglich dass Vorgängerversionen auch funktionieren. Dies ist jedoch
 * nicht getestet worden.
 * 
 * Zusätzlich ist zur Erzeugung der Quellcodedokumentation
 * Doxygen erfoderlich. Ist dies nicht installiert, kann das
 * Programm aber trotzdem übersetzt werden.

 * @subsection compile_unix Kompilieren unter Linux / Unixoiden
 * Um das Programm zu kompilieren tippt man folgende Befehle auf der Konsole ein,
 * wenn man sich im Hauptverzeichnis des Heruntergeladenen Archives befindet:
 * 
 * @code
 * cd build/
 * cmake ..
 * make
 * @endcode
 * Sollten Bibliotheken fehlen oder nicht gefunden werden,
 * wird dies nach dem zweiten Befehl angezeigt.
 * Zum Installieren ist dann noch nötig,
 * @code
 * make install
 * @endcode
 * zu tippen.
 * 
 * Es ist möglich, Debian- und rpm-Pakete zu bauen. Dazu muss nach dem
 * Kompilieren im build-Verzeichnis folgendes getippt werden:
 * @code
 * make package
 * @endcode
 * Die entsprechenden Pakete erscheinen im build-Verzeichnis.
 * 
 * @subsection compile_windows Kompilieren unter Windows
 * Prinzipiell ist es möglich, das Programm unter Windows zu kompilieren.
 * Leider ist uns dies bisher nicht gelungen - Erfahrungen werden sonst hier
 * dokumentiert.
 * 
 * @section usage Benutzung
 * Hier wird eine kleine Einführung in die Benutzung des Programms gegeben.
 * 
 * @subsection usage_structure Grobstruktur des Programms
 * Das Programm ist in in 2 Teile aufgeteilt: Server und GUI. Bevor das
 * Programm verwendet werden kann, muss eine Datenaufbereitung
 * durchgeführt werden.
 * 
 * @subsubsection datenaufbereitung Datenaufbereitung
 * Für die Datenaufbereitung stehen 2 Module bereit: Die einfache, und
 * die normale Vorverarbeitung. Bei der normalen Vorverarbeitung wird
 * ein komplexerer Routinggraph erzeugt, mit dem es auch möglich ist,
 * Beschränkungen beim Abbiegen zu beachten, oder Abbiegen mit höheren
 * Kosten zu belegen als einer Geradeausfahrt. mit dem einfacheren Routinggraphen
 * sind solche Unterscheidungen nicht möglich. Die einfache Vorverarbeitung
 * ist schneller bearbeitet als die normale.
 * @code
 * biker --[simple-]parse=datei.osm[.pbf] --dbfile=datei.db [--dbbackend=sqlite|spatialite]
 * @endcode
 * Angaben in eckigen Klammern sind optional. Standardmäßig wird Spatialite
 * als Datenbankbackend verwendet, wenn Unterstützung dafür eincompiliert wurde,
 * ansonsten wird SQLite verwendet.
 * 
 * \todo Datenaufbereitung aufschreiben (->Benutzung)
 * 
 * @subsubsection serverstart Starten des Servers
 * Zum Starten des Servers
 * \todo Serverstart aufschreiben
 * 
 * @subsection tests Tests
 * Um alle Tests auszuführen, tippt man im build-Verzeichnis des Programms:
 * @code
 * make test
 * @endcode
 * Dann werden alle Tests ausgeführt.
 * 
 * Einzelne Tests werden aufgerufen, indem man das Programm mit
 * dem Parameter "--test" aufruft. Beispiel:
 * 
 * @code
 * biker --test=srtmprovider
 * biker --test srtmprovider
 * @endcode
 * 
 * @section todo Todo
 * lalala
 * @todo Text schreiben, der das Projekt beschreibt.
 * @todo Programm von Anfang an mit i18n/l10n ausstatten?
 */

/**
 * @defgroup database Datenbank
 * @brief Hier werden alle Klassen abgelegt, die mit der Datenbank zu tun haben
 */

/**
 * @defgroup dataprimitives Datenprimitive
 * @brief Hier werden alle Klassen abgelegt, die Datenprimitive sind und in den anderen Modulen verwendet werden.
 */

/**
 * @defgroup routing Routing
 * @brief Hier werden alle Klassen abgelegt, die für das Routing zuständig sind.
 */
 
 /**
 * @defgroup network Netzwerk
 * @brief Hier werden alle Klassen und Funktionen abgelegt,
 *      die für Netzwerksachen zuständig sind.
 */
#endif //MAIN_HPP
