/*
 * Relationship.h
 *
 *  Created on: Jul 25, 2013
 *      Author: zachary
 */

#ifndef RELATIONSHIP_H_
#define RELATIONSHIP_H_

#include "utils/StdString.h"

class CRelationship {
public:
	CRelationship();
	virtual ~CRelationship();

	bool HasLink();

	CStdString m_type;
	int m_rtID;
	int m_o1ID;
	CStdString m_o1Stub;
	CStdString m_o1Name;
	int m_o1TypeID;
	CStdString m_o1TypeName;
	int m_o2ID;
	CStdString m_o2Stub;
	CStdString m_o2Name;
	int m_o2TypeID;
	CStdString m_o2TypeName;
	CStdString m_link;
	int m_index;
};

#endif /* RELATIONSHIP_H_ */
