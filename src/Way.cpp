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
#include "Way.h"
#include "Tag.h"
#include "Node.h"

namespace osm
{

Way::Way( long long id, bool visible, long long osm_id )
:
	id(id),
	visible(visible),
	length(0),
	oneway(false),
	osm_id(osm_id)
{
}

Way::~Way()
{
}

void Way::AddNodeRef( Node* pNode )
{
	if( pNode ) m_NodeRefs.push_back(pNode);
}

void Way::AddTag( Tag* t )
{
	m_Tags.push_back( t );
}

void Way::printstr()
{
	std::cout << "Way id: " << id << " osm_id: " << osm_id << " visible: " << visible << " length: " << length << " oneway: " << oneway << std::endl;
	std::vector<Node*>::const_iterator it_node( m_NodeRefs.begin());	
	std::vector<Node*>::const_iterator last_node( m_NodeRefs.end());

	while(it_node!=last_node)
	{
		Node* node = *it_node++;
		node->printstr();
	}

	std::cout << std::endl;
}

} // end namespace osm

