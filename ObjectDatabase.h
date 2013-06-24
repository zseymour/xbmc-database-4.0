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

namespace dbiplus
{
  class field_value;
  typedef std::vector<field_value> sql_record;
}

class CObjectDatabase: public CDatabase {
public:
	CObjectDatabase();
	virtual ~CObjectDatabase();
	virtual bool Open();

	bool GetChildObjectTypes(int idObjectType, std::vector<int>& ids);
	bool GetParentObjectTypes(int idObjectType, std::vector<int>& ids);
	bool GetAllDescendentObjectTypes(int idObjectType, std::vector<int>& ids);
	bool GetAllAncestorObjectTypes(int idObjectType, std::vector<int>& ids);
	bool GetAllAttributesForObjectType(int idObjectType, std::vector<int>& ids);
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
private:
	DatabaseSettings settings;
	const char *GetBaseDBName() const { return "database"; };
	virtual int GetMinVersion() const;

};

#endif /* OBJECTDATABASE_H_ */
