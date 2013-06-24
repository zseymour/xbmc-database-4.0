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
	bool GetAllAttributesForObjectType(int idObjectType, std::vector<CAttributeType>& types);
	bool GetAllArtworkTypesForObjectType(int idObjectType, std::vector<int>& ids);

	int AddObjectType(int idParentObjectType,const CStdString& stub,const CStdString& name);
	int GetObjectTypeID(const CStdString& stub);

	int AddAttributeType(int idObjectType, CStdString& stub, CStdString& name, int dataType = 0, int dataPrecision = 0, int inheritable = 0);
	int GetAttributeTypeID(const CStdString& stub);

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
	int GetObjectType(int idObject);
	bool AddAttributesForObject(const int& idObject, std::map<int, CAttribute> attributes);
private:
	DatabaseSettings settings;
	const char *GetBaseDBName() const { return "database"; };
	virtual int GetMinVersion() const;

};

#endif /* OBJECTDATABASE_H_ */
