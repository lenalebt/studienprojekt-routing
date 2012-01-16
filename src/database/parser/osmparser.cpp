#include "osmparser.hpp"
#include <iostream>

OSMParser::OSMParser(BlockingQueue<boost::shared_ptr<OSMNode> >* nodeQueue,
    BlockingQueue<boost::shared_ptr<OSMWay> >* wayQueue,
    BlockingQueue<boost::shared_ptr<OSMTurnRestriction> >* turnRestrictionQueue)
    : nodeType(NONE), nodeCount(0), wayCount(0), relationCount(0),
      _nodeQueue(nodeQueue), _wayQueue(wayQueue), _turnRestrictionQueue(turnRestrictionQueue)
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
            node = boost::shared_ptr<OSMNode>(new OSMNode(id, GPSPosition(lon, lat), QVector<OSMProperty>()));

        }
        else if (qName == "way")
        {
            wayCount++;
            nodeType = WAY;
            
            //Zerstört die Queue, damit keine Blockierung auftreten kann
            if (wayCount == 1)
                _nodeQueue->destroyQueue();

            boost::uint64_t id=0;

            for (int i=0; i<atts.length(); i++)
            {
                if (atts.qName(i) == "id")
                    id = atts.value(i).toLong();
            }
            way = boost::shared_ptr<OSMWay>(new OSMWay(id, QVector<OSMProperty>()));
        }
        else if (qName == "relation")
        {
            relationCount++;
            nodeType = RELATION;
            
            //Zerstört die Queue, damit keine Blockierung auftreten kann
            if (relationCount == 1)
                _wayQueue->destroyQueue();

            relation = boost::shared_ptr<OSMTurnRestriction>(new OSMTurnRestriction());
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
        invalidRestriction = false;
        ready = false;
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
            if(key == "restriction")
            {
                if(value == "no_left_turn")
                {
                    relation->setLeft(true);
                }
                else if(value == "no_right_turn ")
                {
                    relation->setRight(true);
                }
                else if(value == "no_straight_on")
                {
                    relation->setStraight(true);
                }
                else if(value == "no_u_turn")
                {
                    relation->setUTurn(true);
                }
                else if(value == "only_right_turn ")
                {
                    relation->setLeft(true);
                    relation->setStraight(true);
                    relation->setUTurn(true);
                }
                else if(value == "only_left_turn ")
                {
                    relation->setRight(true);
                    relation->setStraight(true);
                }
                else if(value == "only_straight_on")
                {
                    relation->setLeft(true);
                    relation->setRight(true);
                    relation->setUTurn(true);
                }
                // die Fälle von no_exit & no_entry + deren Unterscheidung verstehen wir nicht so ganz und daher
                // werden diese beiden fälle nicht behandelt & für die weitere bearbeitung ignoriert.
                else if(value == "no_entry")
                {
                    ready = false;
                    invalidRestriction = true;
                }
                else if(value == "no_exit")
                {
                    ready = false;
                    invalidRestriction = true;
                }

            }
            else if (key == "type")
            {
                if(value == "restriction")
                {
                    if(!invalidRestriction)
                    {
                        ready = true;
                    }
                }
            }
        }
        else  if (qName == "member") //Knoten & Kanten IDs
        {
            QString type, ref, role;
            for (int i=0; i<atts.length(); i++)
            {
                if (atts.qName(i) == "type")
                    type = atts.value(i);
                else if (atts.qName(i) == "ref")
                    ref = atts.value(i);
                else if (atts.qName(i) == "role")
                    role = atts.value(i);
            }
            if(type == "way")
            {
                if(role == "from")
                {
                    relation->setFromId(ref.toLong());
                }
                else if(role == "to")
                {
                    relation->setToId(ref.toLong());
                }
            }
            else if (type == "node")
             {
                if(role == "via")
                {
                    relation->setViaId(ref.toLong());
                }
            }
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
            _nodeQueue->enqueue(node);
            //TODO: dbWriter.addNode(node);
        }
    }
    else if (nodeType == WAY)
    {
        if (qName == "way")
        {
            nodeType = NONE;
            _wayQueue->enqueue(way);
            //TODO: dbWriter.addWay(way);
        }
    }
    else if (nodeType == RELATION)
    {
        if (qName == "relation")
        {
            nodeType = NONE;
            if (ready == true)
                _turnRestrictionQueue ->enqueue(relation);
            //else
            //    delete relation;
            ready = false;
            invalidRestriction = false;
        }
    }

    return true;
}

bool OSMParser::endDocument ()
{
    _turnRestrictionQueue->destroyQueue();
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
