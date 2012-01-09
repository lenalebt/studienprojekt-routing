/*
    Biker wants to be a routing software, intended to be useful for planning bike tours.
    Copyright (C) 2011  Lena Brueder

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/ 

#ifndef OSMPARSER_HPP
#define OSMPARSER_HPP

#include "osmturnrestriction.hpp"
#include "osmway.hpp"
#include "osmedge.hpp"
#include "osmnode.hpp"
#include "osmproperty.hpp"
#include <QtXml>
#include "database.hpp"
#include <boost/cstdint.hpp>
#include "tests.hpp"
#include "blockingqueue.hpp"

enum NodeType {NODE, WAY, RELATION, NONE};

/**
 * @brief Nimmt eine .osm-Datei und liest sie ein, überführt sie in ein temporäres Datenbankformat.
 * 
 * 
 * 
 * @ingroup name
 * @author Lena Brueder
 * @date 2011-12-05
 * @todo Doxygen-Kommentare, generell Implementierung. Dies ist das
 *      Gerüst aus dem alten Programm! Noch funktioniert.... nix!
 * @copyright GNU GPL v3
 */
class OSMParser : public QXmlDefaultHandler
{
private:
    //TODO: OSMDatabaseWriter& dbWriter;

    NodeType nodeType;

    OSMNode* node;
    OSMWay* way;
    OSMEdge* edge;
    OSMTurnRestriction* relation;

    int nodeCount;
    int wayCount;
    int relationCount;

    BlockingQueue<OSMNode>* _nodeQueue;
    BlockingQueue<OSMEdge>* _edgeQueue;
    BlockingQueue<OSMTurnRestriction>* _turnRestrictionQueue;

public:
    OSMParser(BlockingQueue<OSMNode>* nodeQueue, BlockingQueue<OSMEdge>* edgeQueue, BlockingQueue<OSMTurnRestriction>* turnRestrictionQueue);
    //TODO: OSMDatabaseWriter& dbWriter);
    ~OSMParser();
    bool startDocument();
    bool endElement ( const QString & namespaceURI, const QString & localName, const QString & qName );
    bool startElement ( const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts );
    bool endDocument ();
    bool fatalError ( const QXmlParseException & exception );

    bool parse(QString filename);
};


namespace biker_tests
{
    int testOSMParser();
}

#endif // OSMPARSER_HPP
