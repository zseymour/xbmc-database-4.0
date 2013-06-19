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
private:
	DatabaseSettings settings;
	const char *GetBaseDBName() const { return "database"; };
	virtual int GetMinVersion() const;
	int AddToTable(const CStdString& table, const CStdString& firstField, const CStdString& secondField, const CStdString& value);

};

#endif /* OBJECTDATABASE_H_ */
