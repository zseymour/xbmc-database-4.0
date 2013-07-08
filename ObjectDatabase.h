/*
 * ObjectDatabase.h
 *
 *  Created on: Jun 19, 2013
 *      Author: zachary
 */

#ifndef OBJECTDATABASE_H_
#define OBJECTDATABASE_H_

#include "dbwrappers/Database.h"
#include "utils/StdString.h"
#include "utils/log.h"
#include "utils/StringUtils.h"
#include "Bookmark.h"

#include <memory>
#include <set>
#include <map>

namespace dbiplus
{
  class field_value;
  typedef std::vector<field_value> sql_record;
}

typedef enum {
	STRING_ATTRIBUTE = 0,
	NUMBER_ATTRIBUTE = 1,
	BLOB_ATTRIBUTE = 2
} ATTRIBUTE_DATA_TYPE;

class CObjectDatabase: public CDatabase {
public:
	class CAttributeType
	{
	public:
		int idAttributeType;
		int idObjectType;
		CStdString stub;
		CStdString name;
		ATTRIBUTE_DATA_TYPE type;
		int precision;
		bool inheritable;
	};
	class CAttribute
	{
	public:
		CAttributeType type;
		CStdString strValue;
		int intValue;
	};
	CObjectDatabase();
	virtual ~CObjectDatabase();
	virtual bool Open();

	bool GetChildObjectTypes(int idObjectType, std::vector<int>& ids);
	bool GetParentObjectTypes(int idObjectType, std::vector<int>& ids);
	bool GetAllDescendentObjectTypes(int idObjectType, std::vector<int>& ids);
	bool GetAllAncestorObjectTypes(int idObjectType, std::vector<int>& ids);
	bool GetAllAttributeTypesForObjectType(int idObjectType, std::vector<CAttributeType>& types);
	bool GetAllAttributeTypeIDsForObjectType(int idObjectType, std::vector<int>& types);
	bool GetAllArtworkTypesForObjectType(int idObjectType, std::vector<int>& ids);

	int AddObjectType(int idParentObjectType,const CStdString& stub,const CStdString& name);
	int GetObjectTypeId(const CStdString& stub);

	int AddAttributeType(int idObjectType, CStdString& stub, CStdString& name, int dataType = 0, int dataPrecision = 0, int inheritable = 0);
	int GetAttributeTypeId(const CStdString& stub);

	int AddPath(const CStdString& strPath);
	int GetPathId(const CStdString& strPath);
	bool GetPathHash(const CStdString &path, CStdString &hash);
	bool SetPathHash(const CStdString &path, const CStdString &hash);
	void SplitPath(const CStdString& strFileNameAndPath, CStdString& strPath, CStdString& strFileName);

	int AddScraper(const CStdString& scraper, const CStdString& content);
	int GetScraperId(const CStdString& scraper);
	bool LinkScraperToPath(int& idScraper, int& idPath);
	bool LinkScraperToPath(CStdString& scraper, CStdString& path);

	int AddDirEnt(const CStdString& strFileNameAndPath);

	int AddObject(const int& idObjectType, const CStdString& stub, const CStdString& name);
	bool LinkObjectToDirent(int& idObject, int& idDirent);
	int GetObjectType(int idObject);

	bool AddAttributesForObject(const int& idObject, std::map<int, CAttribute> attributes);
	bool AddAttributesForObject(const int& idObject, std::map<CStdString, CAttribute> attributes);
	int GetAttributeId(int idObject, int idAttributeType);
	bool SetAttribute(const int idObject, CAttributeType attrType, CAttribute attr, int idAttribute=-1);
	bool GetAttribute(const int idAttribute, CAttribute& attribute);

	int GetRelationshipTypeId(const CStdString stub);
	bool GetAllRelationshipTypeIDsForObjectType(int idObjectType, std::vector<std::pair <int,int> >& types);
	int GetRelationshipId(int idRelationshipType, int idObject1, int idObject2, CStdString link, int index = 0);
	int LinkObjectToObject(int idRelationshipType, int idObject1, int idObject2, CStdString link, int index = 0);

	bool GetAllArtworkTypeIDsForObjectType(int idObjectType, std::vector<int>& types);

	int AddProfile(CStdString name);
	int GetProfileId(CStdString name);

	bool GetStackTimes(const CStdString &filePath, std::vector<int> &times);
	void SetStackTimes(const CStdString& filePath, std::vector<int> &times);

	void GetBookMarksForFile(const CStdString& strFilenameAndPath, int idProfile, VECBOOKMARKS& bookmarks, CBookmark::EType type /*= CBookmark::STANDARD*/, bool bAppend);
	bool GetResumeBookMark(const CStdString& strFilenameAndPath, int idProfile, CBookmark &bookmark);
	void DeleteResumeBookMark(const CStdString &strFilenameAndPath, int idProfile);
	void AddBookMarkToFile(const CStdString& strFilenameAndPath, int idProfile, const CBookmark &bookmark, CBookmark::EType type /*= CBookmark::STANDARD*/);
	void ClearBookMarkOfFile(const CStdString& strFilenameAndPath, int idProfile, CBookmark& bookmark, CBookmark::EType type /*= CBookmark::STANDARD*/);
	void ClearBookMarksOfFile(const CStdString& strFilenameAndPath, int idProfile, CBookmark::EType type /*= CBookmark::STANDARD*/);
private:
	DatabaseSettings settings;
	const char *GetBaseDBName() const { return "database"; };
	virtual int GetMinVersion() const;
	bool isValidAttributeType(int idObject, int idAttributeType);
	bool isValidRelationshipType(int idRelationshipType, int idObject1, int idObject2);
};

#endif /* OBJECTDATABASE_H_ */
