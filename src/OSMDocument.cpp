/***************************************************************************
 *   Copyright (C) 2008 by Daniel Wendt   								   *
 *   gentoo.murray@gmail.com   											   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "stdafx.h"
#include "OSMDocument.h"
#include "Configuration.h"
#include "Tag.h"
#include "Node.h"
#include "Relation.h"
#include "Way.h"
#include "math_functions.h"


namespace osm
{

OSMDocument::OSMDocument( Configuration& config ) : m_rConfig( config )
{
}

OSMDocument::~OSMDocument()
{
	ez_mapdelete( m_Nodes );
	ez_vectordelete( m_Ways );		
	ez_vectordelete( m_Relations );		
	ez_vectordelete( m_SplittedWays );
}
void OSMDocument::AddNode( Node* n )
{
	m_Nodes[n->id] = n;
}

void OSMDocument::AddWay( Way* w )
{
	m_Ways.push_back( w );
}

void OSMDocument::AddRelation( Relation* r )
{
	m_Relations.push_back( r );
}

Node* OSMDocument::FindNode( long long nodeRefId ) 
const
{
	std::map<long long, Node*>::const_iterator  it = m_Nodes.find( nodeRefId );
	return (it!=m_Nodes.end() ) ? it->second : 0;
}

Node* OSMDocument::FindNodeAt( double lat, double lon )
const
{
	std::map<long long, Node*>::const_iterator it = std::find_if( m_Nodes.begin(), m_Nodes.end(), Node_Comparator(lat, lon, 0.000001) );
	return (it!=m_Nodes.end() ) ? it->second : NULL;
}

std::vector<Way*> OSMDocument::FindWaysWithRef( std::vector<Way*> ws, Node* n )
const
{
	std::vector<Way*>::const_iterator it(ws.begin());
	std::vector<Way*>::const_iterator last(ws.end());

	std::vector<Way*> rs;
	
	while(it!=last)
	{
		Way* w = *it++;
		std::vector<Node*>::const_iterator  it_node = std::find(w->m_NodeRefs.begin(), w->m_NodeRefs.end(), n );
		if (it_node != w->m_NodeRefs.end())
		{
			rs.push_back(w);
		}
	}
	return rs;
}

void OSMDocument::SplitWays()
{
	std::vector<Way*>::const_iterator it(m_Ways.begin());
	std::vector<Way*>::const_iterator last(m_Ways.end());

	//splitted ways get a new ID
	long long id=0;

	while(it!=last)
	{
		Way* currentWay = *it++;
		
		// ITERATE THROUGH THE NODES
		std::vector<Node*>::const_iterator it_node( currentWay->m_NodeRefs.begin());	
		std::vector<Node*>::const_iterator last_node( currentWay->m_NodeRefs.end());
		
		Node* backNode = currentWay->m_NodeRefs.back();

		while(it_node!=last_node)
		{
			
			Node* node = *it_node++;
			Node* secondNode=0;
			Node* lastNode=0;
			
			Way* splitted_way = new Way( ++id, currentWay->visible, currentWay->osm_id );
			splitted_way->name=currentWay->name;
			splitted_way->type=currentWay->type;
			splitted_way->clss=currentWay->clss;
			splitted_way->oneway=currentWay->oneway;
			
			std::vector<Tag*>::iterator it_tag( currentWay->m_Tags.begin() );
			std::vector<Tag*>::iterator last_tag( currentWay->m_Tags.end() );
//			std::cout << "Number of tags: " << currentWay->m_Tags.size() << std::endl;
//			std::cout << "First tag: " << currentWay->m_Tags.front()->key << ":" << currentWay->m_Tags.front()->value << std::endl;
		
			// ITERATE THROUGH THE TAGS
		
			while(it_tag!=last_tag)
			{
				Tag* tag = *it_tag++;

				splitted_way->AddTag(tag);
				
			}
			
			
			
			

	//GeometryFromText('LINESTRING('||x1||' '||y1||','||x2||' '||y2||')',4326);
			
			splitted_way->geom="LINESTRING("+ boost::lexical_cast<std::string>(node->lon) + " " + boost::lexical_cast<std::string>(node->lat) +",";
			
			splitted_way->AddNodeRef(node);
			
			bool found=false;
			
			if(it_node!=last_node)
			{
				while(it_node!=last_node && !found)
				{
					splitted_way->AddNodeRef(*it_node);
					if((*it_node)->numsOfUse>1)
					{
						found=true;
						secondNode = *it_node;
						splitted_way->AddNodeRef(secondNode);
						double length = getLength(node,secondNode);
						if(length<0)
							length*=-1;
						splitted_way->length+=length;
						splitted_way->geom+= boost::lexical_cast<std::string>(secondNode->lon) + " " + boost::lexical_cast<std::string>(secondNode->lat) + ")";
						
					}
					else if(backNode==(*it_node))
					{
						lastNode=*it_node++;
						splitted_way->AddNodeRef(lastNode);
						double length = getLength(node,lastNode);
						if(length<0)
							length*=-1;
						splitted_way->length+=length;
						splitted_way->geom+= boost::lexical_cast<std::string>(lastNode->lon) + " " + boost::lexical_cast<std::string>(lastNode->lat) + ")";
					}
					else
					{
						splitted_way->geom+= boost::lexical_cast<std::string>((*it_node)->lon) + " " + boost::lexical_cast<std::string>((*it_node)->lat) + ",";
						*it_node++;
					}
				}
			}
				
			if(splitted_way->m_NodeRefs.front()!=splitted_way->m_NodeRefs.back())
				m_SplittedWays.push_back(splitted_way);
			else
			{
				delete splitted_way;
				splitted_way=0;
			}
				
		}

	}

} // end SplitWays

bool Node_Comparator::operator () ( const std::pair<long long, Node*> &pair )
const
{	
	return ( 
	(((pair.second)->lat - m_lat >= 0 && (pair.second)->lat - m_lat < m_precision) || (m_lat - (pair.second)->lat >= 0 && m_lat - (pair.second)->lat < m_precision)) && 
	(((pair.second)->lon - m_lon >= 0 && (pair.second)->lon - m_lon < m_precision) || (m_lon - (pair.second)->lon >= 0 && m_lon - (pair.second)->lon < m_precision)) 
	);
}




void OSMDocument::test()
{
	//Node* tn = this->FindNodeAt(39.7814771, 117.0522314);
	//Node* tn = this->FindNodeAt(39.7814111, 117.0482403);
	//Node* tn = this->FindNodeAt(39.8027487,116.7982348);
	Node* tn = this->FindNode( 2272249421 );
	if ( tn == NULL )
	{
		std::cout << "no node found" << std::endl;
	}
	else
	{
		tn->printstr();
	}

	std::vector<Way*> ws = this->FindWaysWithRef( m_Ways, tn );

	std::vector<Way*>::const_iterator it(ws.begin());
	std::vector<Way*>::const_iterator last(ws.end());
	while(it!=last)
	{
		Way* w = *it++;
		w->printstr();
	}
}


} // end namespace osm
