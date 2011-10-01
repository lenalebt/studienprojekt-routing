#Quelle: http://developer.qt.nokia.com/quarterly/view/using_cmake_to_build_qt_projects

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#Projektname
PROJECT(helloworld)

#Hier Versionsnummer setzen
SET (helloworld_VERSION_MAJOR 0)
SET (helloworld_VERSION_MINOR 1)

#Hier CMake-Module speichern, die zum Auffinden von Libs nötig sind
SET(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_SOURCE_DIR}/cmake/Modules/"
    )

#Qt-Einstellungen hier vornehmen.
SET(QT_USE_QTNETWORK TRUE)
SET(QT_USE_QTXML TRUE)
FIND_PACKAGE(Qt4 REQUIRED)

#Hier Quelldateien eintragen
SET(helloworld_SOURCES
    src/main.cpp
    src/hellowindow.cpp
    src/test.cpp
    )

#Hier Header eintragen
SET(helloworld_HEADERS
    src/hellowindow.h
    )

#Hier die Dateien eintragen, die durch den moc müssen
#Das sind vor allem jene, in denen Q_OBJECT steht.
QT4_WRAP_CPP(helloworld_HEADERS_MOC ${helloworld_HEADERS})

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

#Hier ausführbare Dateien hinschreiben, und welche Dateien dazugehören
ADD_EXECUTABLE(helloworld ${helloworld_SOURCES} 
    ${helloworld_HEADERS_MOC})

#Hier hinschreiben, welche Libs zu der ausführbaren Datei gelinkt werden
#müssen.
TARGET_LINK_LIBRARIES(helloworld
    ${QT_LIBRARIES}
    ${QT_QTNETWORK_LIBRARIES}
    ${QT_QTXML_LIBRARIES}
    )

INSTALL (TARGETS helloworld
    DESTINATION bin
    )
