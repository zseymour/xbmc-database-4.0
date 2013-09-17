/*
 * ObjectInfoTag.h
 *
 *  Created on: Jul 27, 2013
 *      Author: zachary
 */

#ifndef OBJECTINFOTAG_H_
#define OBJECTINFOTAG_H_

#include "utils/StdString.h"
#include "Attribute.h"
#include "Relationship.h"
#include "Bookmark.h"
#include "utils/StreamDetails.h"
#include "settings/VideoSettings.h"
#include "XBDateTime.h"


class CObjectInfoTag {
public:
	CObjectInfoTag();
	virtual ~CObjectInfoTag();
	bool IsEmpty();

	int m_idObject;
	int m_profileId;

	CStdString m_name;
	CStdString m_basePath;
	int m_parentPathId;
	int m_fileId;
	CStdString m_strFile;
	CStdString m_strPath;
	CStdString m_fileNameAndPath;
	CStdString m_url;

	AttributeList m_attributes;
	std::vector<CRelationship> m_relations;

	CDateTime m_dateAdded;
	CBookmark m_resumePoint;
	CStreamDetails m_streams;
	CVideoSettings m_settings;

	int m_playCount;
	CDateTime m_lastPlayed;

	int m_duration;


};

#endif /* OBJECTINFOTAG_H_ */
