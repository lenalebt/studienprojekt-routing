#include "osmparser.hpp"
#include <iostream>

OSMParser::OSMParser()//TODO: OSMDatabaseWriter& dbWriter)
    : //TODO: dbWriter(dbWriter), 
    nodeType(NONE), nodeCount(0), wayCount(0), relationCount(0)
{

}

OSMParser::~OSMParser()
{

}

bool OSMParser::parse(QString filename)
{
    QFile file(filename);
    QXmlInputSource inputSource(&file);
    QXmlSimpleReader reader;
    reader.setContentHandler(this);
    reader.setErrorHandler(this);
    return reader.parse(inputSource);
}

bool OSMParser::fatalError ( const QXmlParseException & exception )
{
    std::cerr << "FatalError" << std::endl;
    std::cerr << exception.lineNumber() << " " << exception.columnNumber() << " " << exception.message().toStdString() << std::endl;
    return false;
}

bool OSMParser::startDocument()
{
    nodeType = NONE;
    //std::cerr << "StartDocument" << std::endl;
    return true;
}

bool OSMParser::startElement ( const QString & /*namespaceURI*/, const QString & /*localName*/, const QString & qName, const QXmlAttributes & atts )
{
    if (nodeType == NONE)
    {
        if (qName == "node")
        {
            nodeCount++;
            nodeType = NODE;

            boost::uint64_t id=0;
            double lon=0.0, lat=0.0;

            for (int i=0; i<atts.length(); i++)
            {
                if (atts.qName(i) == "id")
                    id = atts.value(i).toLong();
                else if (atts.qName(i) == "lon")
                    lon = atts.value(i).toDouble();
                else if (atts.qName(i) == "lat")
                    lat = atts.value(i).toDouble();
            }
            node = new OSMNode(id, GPSPosition(lon, lat), QVector<OSMProperty>());

        }
        else if (qName == "way")
        {
            wayCount++;
            nodeType = WAY;

            boost::uint64_t id=0;

            for (int i=0; i<atts.length(); i++)
            {
                if (atts.qName(i) == "id")
                    id = atts.value(i).toLong();
            }
            way = new OSMWay(id, QVector<OSMProperty>());
        }
        else if (qName == "relation")
        {
            relationCount++;
            nodeType = RELATION;
        }
    }
    else if (nodeType == NODE)
    {
        if (qName == "tag") //Eigenschaft
        {
            QString key, value;
            for (int i=0; i<atts.length(); i++)
            {
                if (atts.qName(i) == "k")
                    key = atts.value(i);
                else if (atts.qName(i) == "v")
                    value = atts.value(i);
            }
            if (key != "created_by")
                node->addProperty(OSMProperty(key, value));
        }
    }
    else if (nodeType == WAY)
    {
        if (qName == "tag") //Eigenschaft
        {
            QString key, value;
            for (int i=0; i<atts.length(); i++)
            {
                if (atts.qName(i) == "k")
                    key = atts.value(i);
                else if (atts.qName(i) == "v")
                    value = atts.value(i);
            }
            if (key != "created_by")
                way->addProperty(OSMProperty(key, value));
        }
        else if (qName == "nd") //Elementknoten
        {
            way->addMember(atts.value("ref").toLong());
        }
    }
    else if (nodeType == RELATION)
    {
        if (qName == "tag") //Eigenschaft
        {
            //TODO
        }
    }

    return true;
}

bool OSMParser::endElement ( const QString & /*namespaceURI*/, const QString & /*localName*/, const QString & qName )
{
    if (nodeType == NONE)
    {

    }
    else if (nodeType == NODE)
    {
        if (qName == "node")
        {
            nodeType = NONE;
            //TODO: dbWriter.addNode(node);
        }
    }
    else if (nodeType == WAY)
    {
        if (qName == "way")
        {
            nodeType = NONE;
            //TODO: dbWriter.addWay(way);
        }
    }
    else if (nodeType == RELATION)
    {
        if (qName == "relation")
        {
            nodeType = NONE;
            //TODO: dbWriter.addRelation(relation);

            delete relation;
        }
    }

    return true;
}

bool OSMParser::endDocument ()
{
    //TODO: dbWriter.finished();
    std::cerr << "EndDocument. NodeCount: " << nodeCount << " WayCount: " << wayCount
        << " RelationCount: " << relationCount << std::endl;
    return true;
}

namespace biker_tests
{
    int testOSMParser()
    {
        //return EXIT_SUCCESS;
        return EXIT_FAILURE;
    }
}