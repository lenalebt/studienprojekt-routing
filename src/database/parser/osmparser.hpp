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
#include <boost/shared_ptr.hpp>
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
 * @copyright GNU GPL v3
 */
class OSMParser : public QXmlDefaultHandler
{
private:
    //TODO: OSMDatabaseWriter& dbWriter;

    NodeType nodeType;

    boost::shared_ptr<OSMNode> node;
    boost::shared_ptr<OSMWay> way;
    boost::shared_ptr<OSMEdge> edge;
    boost::shared_ptr<OSMTurnRestriction> relation;

    int nodeCount;
    int wayCount;
    int relationCount;

    bool ready;
    bool invalidRestriction;

    BlockingQueue<boost::shared_ptr<OSMNode> >* _nodeQueue;
    BlockingQueue<boost::shared_ptr<OSMWay> >* _wayQueue;
    BlockingQueue<boost::shared_ptr<OSMTurnRestriction> >* _turnRestrictionQueue;

public:
    OSMParser(BlockingQueue<boost::shared_ptr<OSMNode> >* nodeQueue, BlockingQueue<boost::shared_ptr<OSMWay> >* wayQueue, BlockingQueue<boost::shared_ptr<OSMTurnRestriction> >* turnRestrictionQueue);
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
    /**
     * @ingroup tests
     */
    int testOSMParser();
}

#endif // OSMPARSER_HPP
