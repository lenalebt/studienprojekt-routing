#Quelle: http://developer.qt.nokia.com/quarterly/view/using_cmake_to_build_qt_projects

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#Projektname
PROJECT(biker)

#Hier Versionsnummer setzen
SET (biker_VERSION_MAJOR 0)
SET (biker_VERSION_MINOR 1)

#Bissl mit cmake rumspielen... Hauptziel: Verständnis :)
MESSAGE ( "Choose build type by typing \"cmake -DCMAKE_BUILD_TYPE=Debug\" or \"cmake -DCMAKE_BUILD_TYPE=Release\".")
IF (CMAKE_BUILD_TYPE STREQUAL Debug)
    MESSAGE("CMAKE_BUILD_TYPE set to Debug.")
ELSEIF (CMAKE_BUILD_TYPE STREQUAL Release)
    MESSAGE("CMAKE_BUILD_TYPE set to Release.")
ENDIF ()

#Programmversion dem Quelltext bekannt machen
ADD_DEFINITIONS(-DVERSION=${biker_VERSION_MAJOR}.${biker_VERSION_MINOR})

#Evtl muss man mal für die Compiler verschiedene Einstellungen machen: Wenn, dann hier.
IF ( CMAKE_COMPILER_IS_GNUCXX )
    MESSAGE ("Using Gnu g++ compiler...")
    #Der gcc soll bei mehr Sachen warnen, und pedantischer reagieren
    ADD_DEFINITIONS(-Wall -pedantic -Wno-long-long)
ELSE ( MSVC )
    MESSAGE ("Using MS Visual Studio compiler...")
ENDIF()

#Hier CMake-Module speichern, die zum Auffinden von Libs nötig sind
SET(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${CMAKE_SOURCE_DIR}/cmake/Modules/"
    )

#Qt-Einstellungen hier vornehmen.
SET(QT_USE_QTNETWORK TRUE)
SET(QT_USE_QTXML TRUE)
SET(QT_USE_QTSQL TRUE)

#Qt für ganz viel Krams
FIND_PACKAGE(Qt4 4.7 REQUIRED)
#Spatialite für die Datenbank
FIND_PACKAGE(SPATIALITE REQUIRED)
#ZZipLib zum Entpacken von ZIP-Archiven
FIND_PACKAGE(ZZIP REQUIRED)
#Boost für Kommandozeilenoptionen und so
FIND_PACKAGE(Boost 1.42.0
    COMPONENTS
    program_options
    REQUIRED)
#Google Protocol Buffers (->Protobuf) zum Lesen von .pbf-Dateien
FIND_PACKAGE(ProtocolBuffers REQUIRED)
#zlib zum Entpacken von PBF-Dateien
FIND_PACKAGE(ZLIB REQUIRED)
#Doxygen zum Erzeugen der Doku. Nicht notwendig zum Kompilieren.
FIND_PACKAGE(Doxygen)

#ProtocolBuffers-Compiler ausführen, um Quelldateien zu erstellen
PROTOBUF_GENERATE_CPP(OSMFORMAT_PROTO_SOURCES OSMFORMAT_PROTO_HEADERS PROTOFILES src/database/parser/pbf/osmformat.proto)
PROTOBUF_GENERATE_CPP(FILEFORMAT_PROTO_SOURCES FILEFORMAT_PROTO_HEADERS PROTOFILES src/database/parser/pbf/fileformat.proto)

#Hier Quelldateien eintragen
SET(
    biker_SOURCES
    src/main.cpp
    src/tests.cpp
    
    #src/dataprimitives
    src/dataprimitives/gpsposition.cpp
    src/dataprimitives/gpsroute.cpp
    src/dataprimitives/routingedge.cpp
    src/dataprimitives/routingnode.cpp
    
    #src/database/
    src/database/altitudeprovider.cpp
    src/database/blockingqueue.cpp
    src/database/spatialitedatabase.cpp
    
    #src/routing/
    src/routing/routingmetric.cpp
    src/routing/router.cpp
    src/routing/heap.cpp
    src/routing/closedlist.cpp
    )

#Hier Header eintragen
SET(
    biker_HEADERS
    src/main.hpp
    src/tests.hpp
    
    #src/dataprimitives
    src/dataprimitives/gpsposition.hpp
    src/dataprimitives/gpsroute.hpp
    src/dataprimitives/osmproperty.hpp
    src/dataprimitives/osmnode.hpp
    src/dataprimitives/osmrelation.hpp
    src/dataprimitives/osmway.hpp
    src/dataprimitives/routingnode.hpp
    src/dataprimitives/routingedge.hpp
    
    #src/database/
    src/database/altitudeprovider.hpp
    src/database/database.hpp
    src/database/blockingqueue.hpp
    src/database/spatialitedatabase.hpp
    
    
    #src/routing/
    src/routing/routingmetric.hpp
    src/routing/potentialfunction.hpp
    src/routing/router.hpp
    src/routing/heap.hpp
    src/routing/closedlist.hpp
    )

#Alle Dateien - automatisch erzeugte und selbstgeschriebene
SET(
    biker_ALL_SOURCES
    #Selbst geschriebene Dateien
    ${biker_SOURCES}
    
    #PROTO-SOURCES
    ${OSMFORMAT_PROTO_SOURCES}
    ${FILEFORMAT_PROTO_SOURCES}
    )

SET(
    biker_ALL_HEADERS
    #Selbst geschriebene Dateien
    ${biker_HEADERS}
    
    #PROTO-SOURCES
    ${OSMFORMAT_PROTO_HEADERS}
    ${FILEFORMAT_PROTO_HEADERS}
    )    

#Hier UI-Dateien eintragen (für die GUI, falls es eine gibt [->config, etc])
SET(
    biker_UIS
    )

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${SPATIALITE_INCLUDE_DIR}
    ${ZZIP_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
    ${PROTOBUF_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
    src/
    src/routing/
    src/dataprimitives/
    src/database/
    src/database/parser/
    src/database/parser/pbf/
    )

IF(DOXYGEN_FOUND)
    #Semikolon-getrennte Liste in Whitespace-getrennte Liste konvertieren
    FOREACH(ARG ${biker_HEADERS})
        SET(biker_HEADERS_WHITESPACE "${biker_HEADERS_WHITESPACE} ${CMAKE_CURRENT_SOURCE_DIR}/${ARG}")
    ENDFOREACH(ARG ${biker_HEADERS})
    FOREACH(ARG ${biker_SOURCES})
        SET(biker_SOURCES_WHITESPACE "${biker_SOURCES_WHITESPACE} ${CMAKE_CURRENT_SOURCE_DIR}/${ARG}")
    ENDFOREACH(ARG ${biker_SOURCES})
    GET_DIRECTORY_PROPERTY(DIRINC INCLUDE_DIRECTORIES)
    FOREACH(ARG ${DIRINC})
        SET(biker_INCLUDES_WHITESPACE "${biker_INCLUDES_WHITESPACE} ${ARG}")
    ENDFOREACH(ARG ${DIRINC})
    
    #Konfigurationsdatei hinzufügen
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    #Doxygen als neues Target hinzufügen
    ADD_CUSTOM_TARGET(doxygen ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    #"make clean" soll auch Doxygen-Sachen entfernen
    SET_PROPERTY(DIRECTORY APPEND PROPERTY
	     ADDITIONAL_MAKE_CLEAN_FILES api-doc)
    GET_TARGET_PROPERTY(DOC_TARGET doc TYPE)
    IF(NOT DOC_TARGET)
        ADD_CUSTOM_TARGET(doc)
    ENDIF()
    ADD_DEPENDENCIES(doc doxygen)
ENDIF()

#Hier die Dateien eintragen, die durch den moc müssen
#Das sind vor allem jene, in denen Q_OBJECT steht.
QT4_WRAP_CPP(
    biker_HEADERS_MOC
    ${biker_HEADERS}
    )

QT4_WRAP_UI(
    biker_UIS_H
    ${biker_UIS}
    )

INCLUDE(
    ${QT_USE_FILE}
    )

ADD_DEFINITIONS(
    ${QT_DEFINITIONS}
    )

#Hier ausführbare Dateien hinschreiben, und welche Dateien dazugehören
ADD_EXECUTABLE(
    biker
    ${biker_ALL_SOURCES}
    ${biker_ALL_HEADERS_MOC}
    )

# Tests werden in src/tests.cpp verwaltet. Wenn man dort einen Test hinzufügt,
# sollte er auch hier hinzugefügt werden: Dann kann man automatisiertes Testen
# veranlassen.
ENABLE_TESTING()
ADD_TEST(uint64_t2string                  "biker" "--test" "uint64_t2string")
ADD_TEST(basename                         "biker" "--test" "basename")
ADD_TEST(routingedge                      "biker" "--test" "routingedge")
ADD_TEST(gpsposition                      "biker" "--test" "gpsposition")
ADD_TEST(routingnode                      "biker" "--test" "routingnode")
ADD_TEST(spatialitedatabaseconnection     "biker" "--test" "spatialitedatabaseconnection")


#Zum Erstellen von automatischen Installationspaketen
IF(UNIX)
    SET(CPACK_SOURCE_GENERATOR
        TGZ
        TBZ2
    )
    SET(CPACK_GENERATOR
        TGZ
        TBZ2
        RPM
        DEB
    )
ELSE(UNIX)
    SET(CPACK_SOURCE_GENERATOR
        ZIP
    )
    SET(CPACK_GENERATOR
        NSIS
    )
ENDIF(UNIX)

#Metadaten
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "Biker is a bike route planner"
    )
SET(CPACK_PACKAGE_VENDOR
    "-"
    )
SET(CPACK_PACKAGE_DESCRIPTION_FILE
    #"${CMAKE_CURRENT_SOURCE_DIR}/README.TXT"
    )
SET(CPACK_RESOURCE_FILE_LICENSE
    #"${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.TXT"
    )
SET(CPACK_PACKAGE_VERSION_MAJOR
    ${biker_VERSION_MAJOR}
    )
SET(CPACK_PACKAGE_VERSION_MINOR
    ${biker_VERSION_MINOR}
    )
SET(CPACK_PACKAGE_CONTACT
    "Lena.Brueder@rub.de"
    )
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER
    "Lena Brueder"
    )
#Was sinnvolleres setzen - Internet ist wegen der Kartendaten ;D.
SET(CPACK_PACKAGE_SECTION
    "internet"
    )
SET(CPACK_SOURCE_IGNORE_FILES
    ${CMAKE_BINARY_DIR}/*
    ${CMAKE_CURRENT_BINARY_DIR}/*
    ${CMAKE_CURRENT_SOURCE_DIR}/.gitignore
    ${CMAKE_CURRENT_SOURCE_DIR}/.git
    )
INCLUDE(CPack)


#Hier hinschreiben, welche Libs zu der ausführbaren Datei gelinkt werden
#müssen.
TARGET_LINK_LIBRARIES(biker
    ${QT_LIBRARIES}
    ${QT_QTNETWORK_LIBRARIES}
    ${QT_QTXML_LIBRARIES}
    ${SPATIALITE_LIBRARY}
    ${ZZIP_LIBRARIES}
    ${Boost_LIBRARIES}
    ${PROTOBUF_LIBRARIES}
    ${ZLIB_LIBRARIES}
    )

INSTALL (TARGETS biker
    DESTINATION bin
    )
