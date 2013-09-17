/*
 * ObjectInfoTag.cpp
 *
 *  Created on: Jul 27, 2013
 *      Author: zachary
 */

#include "ObjectInfoTag.h"

CObjectInfoTag::CObjectInfoTag() {
	m_playCount = 0;
	m_duration = 0;
	m_profileId = 1;
	m_parentPathId = 0;
	m_idObject = 0;

}

CObjectInfoTag::~CObjectInfoTag() {
	// TODO Auto-generated destructor stub
}

bool CObjectInfoTag::IsEmpty()
{
	return (m_name.IsEmpty() && m_strPath.IsEmpty() && m_strFile.IsEmpty());
}

