#Quelle: http://developer.qt.nokia.com/quarterly/view/using_cmake_to_build_qt_projects

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#Projektname
PROJECT(biker)

#Hier Versionsnummer setzen
SET (biker_VERSION_MAJOR 0)
SET (biker_VERSION_MINOR 1)

#Hier CMake-Module speichern, die zum Auffinden von Libs nötig sind
SET(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_SOURCE_DIR}/cmake/Modules/"
    )

#Qt-Einstellungen hier vornehmen.
SET(QT_USE_QTNETWORK TRUE)
SET(QT_USE_QTXML TRUE)

#Qt für ganz viel Krams
FIND_PACKAGE(Qt4 REQUIRED)
#Spatialite für die Datenbank
FIND_PACKAGE(SPATIALITE REQUIRED)
#Quazip zum Entpacken von ZIP-Archiven
FIND_PACKAGE(ZZIP REQUIRED)
#Boost für Kommandozeilenoptionen und so
FIND_PACKAGE(Boost COMPONENTS program_options REQUIRED)

#Hier Quelldateien eintragen
SET(biker_SOURCES
    src/main.cpp
    
    #src/DataPrimitives
    src/DataPrimitives/gpsposition.cpp
    )

#Hier Header eintragen
SET(biker_HEADERS
    src/main.hpp
    
    #src/DataPrimitives
    src/DataPrimitives/gpsposition.hpp
    src/DataPrimitives/DataPrimitiveDefines.hpp
    )

#Hier die Dateien eintragen, die durch den moc müssen
#Das sind vor allem jene, in denen Q_OBJECT steht.
QT4_WRAP_CPP(
    biker_HEADERS_MOC
    ${biker_HEADERS}
    )

INCLUDE(
    ${QT_USE_FILE}
    )

ADD_DEFINITIONS(
    ${QT_DEFINITIONS}
    )

INCLUDE_DIRECTORIES(
    ${SPATIALITE_INCLUDE_DIR}
    ${ZZIP_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
    )

#Hier ausführbare Dateien hinschreiben, und welche Dateien dazugehören
ADD_EXECUTABLE(
    biker
    ${biker_SOURCES}
    ${biker_HEADERS_MOC}
    )

#Hier hinschreiben, welche Libs zu der ausführbaren Datei gelinkt werden
#müssen.
TARGET_LINK_LIBRARIES(biker
    ${QT_LIBRARIES}
    ${QT_QTNETWORK_LIBRARIES}
    ${QT_QTXML_LIBRARIES}
    ${SPATIALITE_LIBRARY}
    ${ZZIP_LIBRARIES}
    ${Boost_LIBRARIES}
    )

INSTALL (TARGETS biker
    DESTINATION bin
    )
