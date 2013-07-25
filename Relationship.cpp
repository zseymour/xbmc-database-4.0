/*
 * Relationship.cpp
 *
 *  Created on: Jul 25, 2013
 *      Author: zachary
 */

#include "Relationship.h"

CRelationship::CRelationship() {
 m_index = 0;
 m_o1ID = 0;
 m_o1TypeID = 0;
 m_o2ID = 0;
 m_o2TypeID = 0;
}

CRelationship::~CRelationship() {
	// TODO Auto-generated destructor stub
}

bool CRelationship::HasLink()
{
	return !m_link.IsEmpty();
}

