#Quelle: http://developer.qt.nokia.com/quarterly/view/using_cmake_to_build_qt_projects

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#Projektname
PROJECT(biker)

#Hier Versionsnummer setzen
SET (biker_VERSION_MAJOR 0)
SET (biker_VERSION_MINOR 06)

#Programmversion dem Quelltext bekannt machen
ADD_DEFINITIONS(-DVERSION=${biker_VERSION_MAJOR}.${biker_VERSION_MINOR})

#Bissl mit cmake rumspielen... Hauptziel: Verständnis :)
MESSAGE ( "Choose build type by typing \"cmake -DCMAKE_BUILD_TYPE=Debug\" or \"cmake -DCMAKE_BUILD_TYPE=Release\" or \"cmake -DCMAKE_BUILD_TYPE=Profiling\".")
IF (CMAKE_BUILD_TYPE STREQUAL Debug)
    MESSAGE("CMAKE_BUILD_TYPE set to Debug.")
ELSEIF (CMAKE_BUILD_TYPE STREQUAL Release)
    MESSAGE("CMAKE_BUILD_TYPE set to Release.")
ELSEIF (CMAKE_BUILD_TYPE STREQUAL Profiling)
    MESSAGE("CMAKE_BUILD_TYPE set to Profiling.")
    SET(CMAKE_CXX_FLAGS_PROFILING "-pg")
#    SET(CMAKE_EXE_LINKER_FLAGS_PROFILING "-pg")
ENDIF ()

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
SET(QT_USE_QTGUI FALSE)
#SET(QT_USE_QTSQL TRUE)

#Qt für ganz viel Krams
FIND_PACKAGE(Qt4 4.7 REQUIRED)
#Spatialite für die Datenbank
FIND_PACKAGE(SPATIALITE)
FIND_PACKAGE(SQLITE3 REQUIRED)
#ZZipLib zum Entpacken von ZIP-Archiven
FIND_PACKAGE(ZZIP)
#Boost für Kommandozeilenoptionen und so
FIND_PACKAGE(Boost 1.42.0
    COMPONENTS
    program_options
    REQUIRED)
#Google Protocol Buffers (->Protobuf) zum Lesen von .pbf-Dateien
FIND_PACKAGE(ProtocolBuffers)
#zlib zum Entpacken von PBF-Dateien
FIND_PACKAGE(ZLIB REQUIRED)
#Doxygen zum Erzeugen der Doku. Nicht notwendig zum Kompilieren.
FIND_PACKAGE(Doxygen)

#ProtocolBuffers-Compiler ausführen, um Quelldateien zu erstellen
IF(PROTOBUF_FOUND)
    PROTOBUF_GENERATE_CPP(OSMFORMAT_PROTO_SOURCES OSMFORMAT_PROTO_HEADERS PROTOFILES src/database/parser/pbf/osmformat.proto)
    PROTOBUF_GENERATE_CPP(FILEFORMAT_PROTO_SOURCES FILEFORMAT_PROTO_HEADERS PROTOFILES src/database/parser/pbf/fileformat.proto)
ELSE()
    SET(OSMFORMAT_PROTO_SOURCES
        )
    SET(FILEFORMAT_PROTO_SOURCES
        )
ENDIF()

#Hier Quelldateien eintragen
SET(
    biker_SOURCES
    src/main.cpp
    src/tests.cpp
    src/programoptions.cpp
    
    #src/dataprimitives
    src/dataprimitives/bloomfilter.cpp
    src/dataprimitives/gpsposition.cpp
    src/dataprimitives/gpsroute.cpp
    src/dataprimitives/routingedge.cpp
    src/dataprimitives/routingnode.cpp
    src/dataprimitives/osmnode.cpp
    src/dataprimitives/osmway.cpp
    src/dataprimitives/osmedge.cpp
    src/dataprimitives/osmturnrestriction.cpp
    src/dataprimitives/osmproperty.cpp
      
    #src/preprocessing
    src/preprocessing/datapreprocessing.cpp
    src/preprocessing/simpledatapreprocessing.cpp

    #src/database/
    src/database/altitudeprovider.cpp
    src/database/srtmprovider.cpp
    src/database/blockingqueue.cpp
    src/database/spatialitedatabase.cpp
    src/database/sqlitedatabase.cpp
    src/database/temporarydatabase.cpp
    src/database/zip.cpp
    src/database/sqlite_functions.cpp
    src/database/databaseramcache.cpp
    src/database/database.cpp
    src/database/spacefillingcurves.cpp
    
    #src/database/parser
    src/database/parser/osmparser.cpp
    src/database/parser/pbfparser.cpp
    
    #src/routing/
    src/routing/routingmetric.cpp
    src/routing/potentialfunction.cpp
    src/routing/router.cpp
    src/routing/heap.cpp
    src/routing/closedlist.cpp
    src/routing/dijkstra.cpp
    src/routing/astar.cpp
    
    #src/network/
    src/network/webserver.cpp
    src/network/filedownloader.cpp
    )

#Hier Header eintragen
SET(
    biker_HEADERS
    src/main.hpp
    src/tests.hpp
    src/programoptions.hpp
    
    #src/dataprimitives
    src/dataprimitives/bloomfilter.hpp
    src/dataprimitives/gpsposition.hpp
    src/dataprimitives/gpsroute.hpp
    src/dataprimitives/osmproperty.hpp
    src/dataprimitives/osmnode.hpp
    src/dataprimitives/osmturnrestriction.hpp
    src/dataprimitives/osmway.hpp
    src/dataprimitives/osmedge.hpp
    src/dataprimitives/routingnode.hpp
    src/dataprimitives/routingedge.hpp

    #src/preprocessing
    src/preprocessing/datapreprocessing.hpp
    src/preprocessing/simpledatapreprocessing.hpp

    #src/database/
    src/database/altitudeprovider.hpp
    src/database/srtmprovider.hpp
    src/database/database.hpp
    src/database/blockingqueue.hpp
    src/database/spatialitedatabase.hpp
    src/database/sqlitedatabase.hpp
    src/database/temporarydatabase.hpp
    src/database/zip.hpp
    src/database/sqlite_functions.hpp
    src/database/databaseramcache.hpp
    src/database/spacefillingcurves.hpp
    
    #src/database/parser
    src/database/parser/osmparser.hpp
    src/database/parser/pbfparser.hpp
    
    #src/routing/
    src/routing/routingmetric.hpp
    src/routing/potentialfunction.hpp
    src/routing/router.hpp
    src/routing/heap.hpp
    src/routing/closedlist.hpp
    src/routing/dijkstra.hpp
    src/routing/astar.hpp
    
    #src/network/
    src/network/webserver.hpp
    src/network/filedownloader.hpp
    )

#Hier die Dateien eintragen, die durch den moc müssen
#Das sind vor allem jene, in denen Q_OBJECT steht.
QT4_WRAP_CPP(
    biker_HEADERS_MOC
    #src/network/webserver.hpp
    )

QT4_WRAP_UI(
    biker_UIS_H
    ${biker_UIS}
    )

#Alle Dateien - automatisch erzeugte und selbstgeschriebene
SET(
    biker_ALL_SOURCES
    #Selbst geschriebene Dateien
    ${biker_SOURCES}
    
    #PROTO-SOURCES
    ${OSMFORMAT_PROTO_SOURCES}
    ${FILEFORMAT_PROTO_SOURCES}
    
    #MOC-Dateien
    ${biker_HEADERS_MOC}
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

IF (SPATIALITE_FOUND)
    INCLUDE_DIRECTORIES(${SPATIALITE_INCLUDE_DIR})
ENDIF()

IF(ZZIP_FOUND)
    INCLUDE_DIRECTORIES(${ZZIP_INCLUDE_DIR})
ENDIF()

IF(PROTOBUF_FOUND)
    INCLUDE_DIRECTORIES(${PROTOBUF_INCLUDE_DIRS})
ENDIF()

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Boost_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIRS}
    ${SQLITE_INCLUDE_DIRS}
    src/
    src/routing/
    src/dataprimitives/
    src/preprocessing/
    src/database/
    src/database/parser/
    src/database/parser/pbf/
    src/network/
    ${CMAKE_CURRENT_BINARY_DIR}/src/database/parser/pbf/
    )

#CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/data/rub.osm ${CMAKE_CURRENT_BINARY_DIR}/data/rub.osm COPYONLY)
#CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/data/rub.pbf ${CMAKE_CURRENT_BINARY_DIR}/data/rub.pbf COPYONLY)
#CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/data/bochum_city.osm ${CMAKE_CURRENT_BINARY_DIR}/data/bochum_city.osm COPYONLY)
#CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/data/bochum_city.pbf ${CMAKE_CURRENT_BINARY_DIR}/data/bochum_city.pbf COPYONLY)
#CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/gui/gui.html ${CMAKE_CURRENT_BINARY_DIR}/gui/gui.html COPYONLY)
#CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/gui/style.css ${CMAKE_CURRENT_BINARY_DIR}/gui/style.css COPYONLY)
#CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/gui/img/marker-red.png ${CMAKE_CURRENT_BINARY_DIR}/gui/img/marker-red.png COPYONLY)
#CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/gui/img/marker-blue.png ${CMAKE_CURRENT_BINARY_DIR}/gui/img/marker-blue.png COPYONLY)
#CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/gui/img/marker-cyan.png ${CMAKE_CURRENT_BINARY_DIR}/gui/img/marker-cyan.png COPYONLY)
#CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/gui/img/marker-green.png ${CMAKE_CURRENT_BINARY_DIR}/gui/img/marker-green.png COPYONLY)
#CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/gui/img/marker-purple.png ${CMAKE_CURRENT_BINARY_DIR}/gui/img/marker-purple.png COPYONLY)
#CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/gui/img/marker-yellow.png ${CMAKE_CURRENT_BINARY_DIR}/gui/img/marker-yellow.png COPYONLY)
FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/gui DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

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

#Dem Quelltext sagen, wo die spatialite-library ist
IF(SPATIALITE_FOUND)
    ADD_DEFINITIONS(-DSPATIALITE_LIB=${SPATIALITE_LIBRARY})
    ADD_DEFINITIONS(-DSPATIALITE_FOUND)
ENDIF()
IF(ZZIP_FOUND)
    ADD_DEFINITIONS(-DZZIP_FOUND)
ENDIF()
IF(PROTOBUF_FOUND)
    ADD_DEFINITIONS(-DPROTOBUF_FOUND)
ENDIF()

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
ADD_TEST(osmnode                          "biker" "--test" "osmnode")
ADD_TEST(osmway                           "biker" "--test" "osmway")
ADD_TEST(osmedge                          "biker" "--test" "osmedge")
ADD_TEST(osmproperty                      "biker" "--test" "osmproperty")
ADD_TEST(osmturnrestriction               "biker" "--test" "osmturnrestriction")
ADD_TEST(osmparser                        "biker" "--test" "osmparser")
IF(PROTOBUF_FOUND)
    ADD_TEST(pbfparser                        "biker" "--test" "pbfparser")
ENDIF()
ADD_TEST(gpsroute                         "biker" "--test" "gpsroute")
ADD_TEST(potentialfunction                "biker" "--test" "potentialfunction")
IF(ZZIP_FOUND)
    ADD_TEST(srtmprovider                     "biker" "--test" "srtmprovider")
ENDIF()
ADD_TEST(filedownloader                   "biker" "--test" "filedownloader")
IF(SPATIALITE_FOUND)
    ADD_TEST(spatialitedatabaseconnection     "biker" "--test" "spatialitedatabaseconnection")
ENDIF()
ADD_TEST(sqlitedatabaseconnection         "biker" "--test" "sqlitedatabaseconnection")
ADD_TEST(temporaryosmdatabaseconnection   "biker" "--test" "temporaryosmdatabaseconnection")
ADD_TEST(databaseramcache                 "biker" "--test" "databaseramcache")
ADD_TEST(blockingqueue                    "biker" "--test" "blockingqueue")
ADD_TEST(binaryheap                       "biker" "--test" "binaryheap")
ADD_TEST(hashclosedlist                   "biker" "--test" "hashclosedlist")
ADD_TEST(multithreadedhashclosedlist      "biker" "--test" "multithreadedhashclosedlist")
ADD_TEST(datapreprocessing                "biker" "--test" "datapreprocessing")
ADD_TEST(simpledatapreprocessing          "biker" "--test" "simpledatapreprocessing")
ADD_TEST(webserver                        "biker" "--test" "webserver")
ADD_TEST(dijkstrarouter                   "biker" "--test" "dijkstrarouter")
ADD_TEST(multithreadeddijkstrarouter      "biker" "--test" "multithreadeddijkstrarouter")
ADD_TEST(astarrouter                      "biker" "--test" "astarrouter")
ADD_TEST(multithreadedastarrouter         "biker" "--test" "multithreadedastarrouter")

SET_TESTS_PROPERTIES(dijkstrarouter              PROPERTIES DEPENDS simpledatapreprocessing)
SET_TESTS_PROPERTIES(astarrouter                 PROPERTIES DEPENDS simpledatapreprocessing)
SET_TESTS_PROPERTIES(multithreadeddijkstrarouter PROPERTIES DEPENDS simpledatapreprocessing)
SET_TESTS_PROPERTIES(multithreadedastarrouter    PROPERTIES DEPENDS simpledatapreprocessing)


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
IF (SPATIALITE_FOUND)
    TARGET_LINK_LIBRARIES(biker ${SPATIALITE_LIBRARY})
ENDIF()
IF (ZZIP_FOUND)
    TARGET_LINK_LIBRARIES(biker ${ZZIP_LIBRARIES})
ENDIF()
IF(PROTOBUF_FOUND)
    TARGET_LINK_LIBRARIES(biker ${PROTOBUF_LIBRARIES})
ENDIF()
TARGET_LINK_LIBRARIES(biker
    ${QT_LIBRARIES}
    ${QT_QTNETWORK_LIBRARIES}
    ${QT_QTXML_LIBRARIES}
    ${Boost_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${SQLITE3_LIBRARIES}
    )

INSTALL (TARGETS biker
    DESTINATION bin
    )

