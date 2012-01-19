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

/* Uses code from MoNav. Most code was originally taken from there,
 * and changed to fit my purposes.
 */

#include "pbfparser.hpp"


PBFParser::PBFParser(BlockingQueue<boost::shared_ptr<OSMNode> >* nodeQueue,
    BlockingQueue<boost::shared_ptr<OSMWay> >* wayQueue,
    BlockingQueue<boost::shared_ptr<OSMTurnRestriction> >* turnRestrictionQueue)
    : _nodeQueue(nodeQueue), _wayQueue(wayQueue), _turnRestrictionQueue(turnRestrictionQueue)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
}


PBFParser::~PBFParser()
{
    
}


bool PBFParser::parse(QString filename)
{
    m_file.setFileName( filename );

    if ( !openQFile( &m_file, QIODevice::ReadOnly ) )
        return false;

    if ( !readBlockHeader() )
        return false;

    if ( m_blockHeader.type() != "OSMHeader" ) {
        qCritical() << "OSMHeader missing, found" << m_blockHeader.type().data() << "instead";
        return false;
    }

    if ( !readBlob() )
        return false;

    if ( !m_headerBlock.ParseFromArray( m_buffer.data(), m_buffer.size() ) ) {
        qCritical() << "failed to parse HeaderBlock";
        return false;
    }
    for ( int i = 0; i < m_headerBlock.required_features_size(); i++ ) {
        const std::string& feature = m_headerBlock.required_features( i );
        bool supported = false;
        if ( feature == "OsmSchema-V0.6" )
            supported = true;
        else if ( feature == "DenseNodes" )
            supported = true;

        if ( !supported ) {
            qCritical() << "required feature not supported:" << feature.data();
            return false;
        }
    }
    m_loadBlock = true;
    
    boost::shared_ptr<OSMWay> inputWay = boost::shared_ptr<OSMWay>(new OSMWay());
    boost::shared_ptr<OSMNode> inputNode = boost::shared_ptr<OSMNode>(new OSMNode());
    Relation inputRelation;
    while ( true )
    {
        EntityType type = getEntitiy( inputNode, inputWay, &inputRelation );

        if ( type == EntityNone )
            break;

        if ( type == EntityNode ) {
            this->nodeCount++;
            //TODO: dbWriter.addNode(inputNode);
            _nodeQueue->enqueue(inputNode);
            inputNode = boost::shared_ptr<OSMNode>(new OSMNode());
            
            continue;
        }

        if ( type == EntityWay ) {
            
            //TODO: dbWriter.addWay(inputWay);
            _wayQueue->enqueue(inputWay);
            inputWay = boost::shared_ptr<OSMWay>(new OSMWay());

            continue;
        }

        if ( type == EntityRelation ) {
            
            bool ready = false;
            bool invalidRestriction = false;
            std::vector< RelationMember >  C = inputRelation.members;
            for (std::vector< RelationMember >::const_iterator constIt = C.begin(); constIt != C.end(); ++constIt) {
                RelationMember relationMemberOne = *constIt;
                long ref = relationMemberOne.ref;
                int t = relationMemberOne.type;
                QString role = relationMemberOne.role;
                if(t == 0)
                {
                    if(role == "from")
                    {
                        relation->setFromId(ref);
                    }
                    else if(role == "to")
                    {
                        relation->setToId(ref);
                    }
                }
                else if (t == 1)
                {
                    if(role == "via")
                    {
                        relation->setViaId(ref);
                    }
                }
            }
            std::vector< Tag >  D = inputRelation.tags;
            for (std::vector< Tag > ::const_iterator constIt = D.begin(); constIt != D.end(); ++constIt) {
                Tag relationTagOne = *constIt;
                QString value = relationTagOne.value;
                QString key = relationTagOne.key;
                if (key == "type")
                {
                    if(value == "restriction")
                    {
                        if(!invalidRestriction)
                        {
                            ready = true;
                        }
                    }
                }
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
            if (ready == true)
            {
                if(!invalidRestriction)
                {
                    _turnRestrictionQueue ->enqueue(relation);
                }
            }
            
            relation = boost::shared_ptr<OSMTurnRestriction>(new OSMTurnRestriction());
            //
            continue;
        }
    }
    return true;
}


PBFParser::EntityType PBFParser::getEntitiy( boost::shared_ptr<OSMNode> node, boost::shared_ptr<OSMWay> way, Relation* relation )
{
    if ( m_loadBlock ) {
        if ( !readNextBlock() )
            return EntityNone;
        loadBlock();
        loadGroup();
    }

    switch ( m_mode ) {
    case ModeNode:
        parseNode( node );
        return EntityNode;
    case ModeWay:
        parseWay( way );
        return EntityWay;
    case ModeRelation:
        parseRelation( relation );
        return EntityRelation;
    case ModeDense:
        parseDense( node );
        return EntityNode;
    }

    return EntityNone;
}


int PBFParser::convertNetworkByteOrder( char data[4] )
{
    return ( ( ( unsigned ) data[0] ) << 24 ) | ( ( ( unsigned ) data[1] ) << 16 ) | ( ( ( unsigned ) data[2] ) << 8 ) | ( unsigned ) data[3];
}


void PBFParser::parseNode( boost::shared_ptr<OSMNode> node )
{
    const OSMPBF::Node& inputNode = m_primitiveBlock.primitivegroup( m_currentGroup ).nodes( m_currentEntity );
    node->setID(inputNode.id());
    node->setLat(( ( double ) inputNode.lat() * m_primitiveBlock.granularity() + m_primitiveBlock.lat_offset() ) / NANO);
    node->setLon(( ( double ) inputNode.lon() * m_primitiveBlock.granularity() + m_primitiveBlock.lon_offset() ) / NANO);
    QList<OSMProperty> props;
    for ( int tag = 0; tag < inputNode.keys_size(); tag++ ) {
        OSMProperty newTag;
        newTag.setKey(QString::fromUtf8( m_primitiveBlock.stringtable().s( inputNode.keys( tag ) ).data() ));
        newTag.setValue(QString::fromUtf8( m_primitiveBlock.stringtable().s( inputNode.vals( tag ) ).data() ));
        node->addProperty(newTag);
    }

    m_currentEntity++;
    if ( m_currentEntity >= m_primitiveBlock.primitivegroup( m_currentGroup ).nodes_size() ) {
        m_currentEntity = 0;
        m_currentGroup++;
        if ( m_currentGroup >= m_primitiveBlock.primitivegroup_size() )
            m_loadBlock = true;
        else
            loadGroup();
    }
}


void PBFParser::parseWay( boost::shared_ptr<OSMWay> way )
{
    const OSMPBF::Way& inputWay = m_primitiveBlock.primitivegroup( m_currentGroup ).ways( m_currentEntity );
    way->setID(inputWay.id());
    for ( int tag = 0; tag < inputWay.keys_size(); tag++ ) {
        OSMProperty newTag;
        newTag.setKey(QString::fromUtf8( m_primitiveBlock.stringtable().s( inputWay.keys( tag ) ).data() ));
        newTag.setValue(QString::fromUtf8( m_primitiveBlock.stringtable().s( inputWay.vals( tag ) ).data() ));
        way->addProperty(newTag);
    }

    long long lastRef = 0;
    for ( int i = 0; i < inputWay.refs_size(); i++ ) {
        lastRef += inputWay.refs( i );
        way->addMember(lastRef);
    }

    m_currentEntity++;
    if ( m_currentEntity >= m_primitiveBlock.primitivegroup( m_currentGroup ).ways_size() ) {
        m_currentEntity = 0;
        m_currentGroup++;
        if ( m_currentGroup >= m_primitiveBlock.primitivegroup_size() )
            m_loadBlock = true;
        else
            loadGroup();
    }
}


void PBFParser::parseRelation( Relation* relation )
{
    relation->tags.clear();
    relation->members.clear();

    const OSMPBF::Relation& inputRelation = m_primitiveBlock.primitivegroup( m_currentGroup ).relations( m_currentEntity );
    relation->id = inputRelation.id();
    for ( int tag = 0; tag < inputRelation.keys_size(); tag++ ) {
        Tag newTag;
        newTag.key = QString::fromUtf8( m_primitiveBlock.stringtable().s( inputRelation.keys( tag ) ).data() );
        newTag.value = QString::fromUtf8( m_primitiveBlock.stringtable().s( inputRelation.vals( tag ) ).data() );
        relation->tags.push_back( newTag );
    }

    long long lastRef = 0;
    for ( int i = 0; i < inputRelation.types_size(); i++ ) {
        RelationMember member;
        switch ( inputRelation.types( i ) ) {
        case OSMPBF::Relation::NODE:
            member.type = RelationMember::Node;
            break;
        case OSMPBF::Relation::WAY:
            member.type = RelationMember::Way;
            break;
        case OSMPBF::Relation::RELATION:
            member.type = RelationMember::Relation;
        }
        lastRef += inputRelation.memids( i );
        member.ref = lastRef;
        member.role = m_primitiveBlock.stringtable().s( inputRelation.roles_sid( i ) ).data();
        relation->members.push_back( member );
    }

    m_currentEntity++;
    if ( m_currentEntity >= m_primitiveBlock.primitivegroup( m_currentGroup ).relations_size() ) {
        m_currentEntity = 0;
        m_currentGroup++;
        if ( m_currentGroup >= m_primitiveBlock.primitivegroup_size() )
            m_loadBlock = true;
        else
            loadGroup();
    }
}


void PBFParser::parseDense( boost::shared_ptr<OSMNode> node )
{
    const OSMPBF::DenseNodes& dense = m_primitiveBlock.primitivegroup( m_currentGroup ).dense();
    m_lastDenseID += dense.id( m_currentEntity );
    m_lastDenseLatitude += dense.lat( m_currentEntity );
    m_lastDenseLongitude += dense.lon( m_currentEntity );
    node->setID(m_lastDenseID);
    node->setLat(( ( double ) m_lastDenseLatitude * m_primitiveBlock.granularity() + m_primitiveBlock.lat_offset() ) / NANO);
    node->setLon(( ( double ) m_lastDenseLongitude * m_primitiveBlock.granularity() + m_primitiveBlock.lon_offset() ) / NANO);

    while ( true ){
        if ( m_lastDenseTag >= dense.keys_vals_size() )
            break;

        int tagValue = dense.keys_vals( m_lastDenseTag );
        if ( tagValue == 0 ) {
            m_lastDenseTag++;
            break;
        }
        
        OSMProperty newTag;
        newTag.setKey(QString::fromUtf8( m_primitiveBlock.stringtable().s( dense.keys_vals( m_lastDenseTag ) ).data() ));
        newTag.setValue(QString::fromUtf8( m_primitiveBlock.stringtable().s( dense.keys_vals( m_lastDenseTag + 1 ) ).data() ));
        node->addProperty(newTag);
        m_lastDenseTag += 2;
    }

    m_currentEntity++;
    if ( m_currentEntity >= dense.id_size() ) {
        m_currentEntity = 0;
        m_currentGroup++;
        if ( m_currentGroup >= m_primitiveBlock.primitivegroup_size() )
            m_loadBlock = true;
        else
            loadGroup();
    }
}


void PBFParser::loadGroup()
{
    const OSMPBF::PrimitiveGroup& group = m_primitiveBlock.primitivegroup( m_currentGroup );
    if ( group.nodes_size() != 0 ) {
        m_mode = ModeNode;
    } else if ( group.ways_size() != 0 ) {
        m_mode = ModeWay;
    } else if ( group.relations_size() != 0 ) {
        m_mode = ModeRelation;
    } else if ( group.has_dense() )  {
        m_mode = ModeDense;
        m_lastDenseID = 0;
        m_lastDenseTag = 0;
        m_lastDenseLatitude = 0;
        m_lastDenseLongitude = 0;
        assert( group.dense().id_size() != 0 );
    } else
        assert( false );
}


void PBFParser::loadBlock()
{
    m_loadBlock = false;
    m_currentGroup = 0;
    m_currentEntity = 0;
/*    int stringCount = m_primitiveBlock.stringtable().s_size();
    // precompute all strings that match a necessary tag
    m_nodeTagIDs.resize( m_primitiveBlock.stringtable().s_size() );
    for ( int i = 1; i < stringCount; i++ )
        m_nodeTagIDs[i] = m_nodeTags.value( m_primitiveBlock.stringtable().s( i ).data(), -1 );
    m_wayTagIDs.resize( m_primitiveBlock.stringtable().s_size() );
    for ( int i = 1; i < stringCount; i++ )
        m_wayTagIDs[i] = m_wayTags.value( m_primitiveBlock.stringtable().s( i ).data(), -1 );
    m_relationTagIDs.resize( m_primitiveBlock.stringtable().s_size() );
    for ( int i = 1; i < stringCount; i++ )
        m_relationTagIDs[i] = m_relationTags.value( m_primitiveBlock.stringtable().s( i ).data(), -1 );*/
}


bool PBFParser::readNextBlock()
{
    if ( !readBlockHeader() )
        return false;

    if ( m_blockHeader.type() != "OSMData" ) {
        qCritical() << "invalid block type, found" << m_blockHeader.type().data() << "instead of OSMData";
        return false;
    }

    if ( !readBlob() )
        return false;

    if ( !m_primitiveBlock.ParseFromArray( m_buffer.data(), m_buffer.size() ) ) {
        qCritical() << "failed to parse PrimitiveBlock";
        return false;
    }
    return true;
}


bool PBFParser::readBlockHeader()
{
    char sizeData[4];
    if ( m_file.read( sizeData, 4 * sizeof( char ) ) != 4 * sizeof( char ) )
        return false; // end of stream?

    int size = convertNetworkByteOrder( sizeData );
    if ( size > MAX_BLOCK_HEADER_SIZE || size < 0 ) {
        qCritical() << "BlockHeader size invalid:" << size;
        return false;
    }
    m_buffer.resize( size );
    int readBytes = m_file.read( m_buffer.data(), size );
    if ( readBytes != size ) {
        qCritical() << "failed to read BlockHeader";
        return false;
    }
    if ( !m_blockHeader.ParseFromArray( m_buffer.constData(), size ) ) {
        qCritical() << "failed to parse BlockHeader";
        return false;
    }
    return true;
}


bool PBFParser::readBlob()
{
    int size = m_blockHeader.datasize();
    if ( size < 0 || size > MAX_BLOB_SIZE ) {
        qCritical() << "invalid Blob size:" << size;
        return false;
    }
    m_buffer.resize( size );
    int readBytes = m_file.read( m_buffer.data(), size );
    if ( readBytes != size ) {
        qCritical() << "failed to read Blob";
        return false;
    }
    if ( !m_blob.ParseFromArray( m_buffer.constData(), size ) ) {
        qCritical() << "failed to parse blob";
        return false;
    }

    if ( m_blob.has_raw() ) {
        const std::string& data = m_blob.raw();
        m_buffer.resize( data.size() );
        for ( unsigned i = 0; i < data.size(); i++ )
            m_buffer[i] = data[i];
    } else if ( m_blob.has_zlib_data() ) {
        if ( !unpackZlib() )
            return false;
/*    } else if ( m_blob.has_bzip2_data() ) {
        if ( !unpackBzip2() )
            return false;
    } else if ( m_blob.has_lzma_data() ) {
        if ( !unpackLzma() )
            return false;*/
    } else {
        qCritical() << "Blob contains no data";
        return false;
    }

    return true;
}


bool PBFParser::unpackZlib()
{
    m_buffer.resize( m_blob.raw_size() );
    z_stream compressedStream;
    compressedStream.next_in = ( unsigned char* ) m_blob.zlib_data().data();
    compressedStream.avail_in = m_blob.zlib_data().size();
    compressedStream.next_out = ( unsigned char* ) m_buffer.data();
    compressedStream.avail_out = m_blob.raw_size();
    compressedStream.zalloc = Z_NULL;
    compressedStream.zfree = Z_NULL;
    compressedStream.opaque = Z_NULL;
    int ret = inflateInit( &compressedStream );
    if ( ret != Z_OK ) {
        qCritical() << "failed to init zlib stream";
        return false;
    }
    ret = inflate( &compressedStream, Z_FINISH );
    if ( ret != Z_STREAM_END ) {
        qCritical() << "failed to inflate zlib stream";
        return false;
    }
    ret = inflateEnd( &compressedStream );
    if ( ret != Z_OK ) {
        qCritical() << "failed to deinit zlib stream";
        return false;
    }
    return true;
}

/*

bool PBFParser::unpackBzip2()
{
    unsigned size = m_blob.raw_size();
    m_buffer.resize( size );
    m_bzip2Buffer.resize( m_blob.bzip2_data().size() );
    for ( unsigned i = 0; i < m_blob.bzip2_data().size(); i++ )
        m_bzip2Buffer[i] = m_blob.bzip2_data()[i];
    int ret = BZ2_bzBuffToBuffDecompress( m_buffer.data(), &size, m_bzip2Buffer.data(), m_bzip2Buffer.size(), 0, 0 );
    if ( ret != BZ_OK ) {
        qCritical() << "failed to unpack bzip2 stream";
        return false;
    }
    return true;
}*/


void* PBFParser::SzAlloc( void *p, size_t size)
{
    p = p;
    return malloc( size );
}


void PBFParser::SzFree( void *p, void *address)
{
    p = p;
    free( address );
}

