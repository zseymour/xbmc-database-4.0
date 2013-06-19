/*
 * ObjectDatabase.cpp
 *
 *  Created on: Jun 19, 2013
 *      Author: zachary
 */

#include "ObjectDatabase.h"
#include "dbwrappers/dataset.h"



CObjectDatabase::CObjectDatabase()
{
	CStdString host = "/home/zachary/workspace/xbmc-database-4.0/Debug";
	settings.host = host;
	settings.name = "database";
	settings.type = "sqlite3";

}

CObjectDatabase::~CObjectDatabase()
{
	// TODO Auto-generated destructor stub
}

int CObjectDatabase::GetMinVersion() const
{
  return 4;
}

bool CObjectDatabase::Open()
{
  return CDatabase::Open(settings);
}

int CObjectDatabase::AddToTable(const CStdString& table, const CStdString& firstField, const CStdString& secondField, const CStdString& value)
{
  try
  {
    if (NULL == m_pDB.get()) return -1;
    if (NULL == m_pDS.get()) return -1;

    CStdString strSQL = PrepareSQL("select %s from %s where %s like '%s'", firstField.c_str(), table.c_str(), secondField.c_str(), value.c_str());
    m_pDS->query(strSQL.c_str());
    if (m_pDS->num_rows() == 0)
    {
      m_pDS->close();
      // doesnt exists, add it
      strSQL = PrepareSQL("insert into %s (%s, %s) values(NULL, '%s')", table.c_str(), firstField.c_str(), secondField.c_str(), value.c_str());
      m_pDS->exec(strSQL.c_str());
      int id = (int)m_pDS->lastinsertid();
      return id;
    }
    else
    {
      int id = m_pDS->fv(firstField).get_asInt();
      m_pDS->close();
      return id;
    }
  }
  catch (...)
  {
    CLog::Log(LOGERROR, "%s (%s) failed", __FUNCTION__, value.c_str() );
  }

  return -1;
}

bool CObjectDatabase::GetChildObjectTypes(int idObjectType, std::vector<int>& ids)
{
	try
	  {
	    if (NULL == m_pDB.get()) return false;
	    if (NULL == m_pDS.get()) return false;

	    CStdString strSQL=PrepareSQL("select idObjectType from objectTypes where idParentObjectType=%i", idObjectType);
	    m_pDS2->query(strSQL.c_str());
	    while (!m_pDS2->eof())
	    {
	      ids.push_back(m_pDS2->fv(0).get_asInt());
	      m_pDS2->next();
	    }

	    m_pDS2->close();
	    return true;
	  }
	  catch (...)
	  {
	    CLog::Log(LOGERROR, "%s (%i) failed", __FUNCTION__, idObjectType);
	  }

	  return false;
}

bool CObjectDatabase::GetAllDescendentObjectTypes(int idObjectType, std::vector<int>& ids)
{
	try
		  {
		    if (NULL == m_pDB.get()) return false;
		    if (NULL == m_pDS.get()) return false;

		    std::vector<int> childIds;
		    if(GetChildObjectTypes(idObjectType, childIds))
		    {
		    	while(!childIds.empty())
		    	{
		    		ids.insert(ids.end(),childIds.begin(),childIds.end());
		    		std::vector<int> tempIds = childIds;
		    		childIds.clear();
		    		for(std::vector<int>::iterator it = tempIds.begin(); it != tempIds.end(); ++it)
		    		{
		    			GetChildObjectTypes(*it,childIds);
		    		}
		    	}
		    	return true;
		    }

		  }
		  catch (...)
		  {
		    CLog::Log(LOGERROR, "%s (%i) failed", __FUNCTION__, idObjectType);
		  }

		  return false;
}

bool CObjectDatabase::GetParentObjectTypes(int idObjectType, std::vector<int>& ids)
{
	try
	  {
	    if (NULL == m_pDB.get()) return false;
	    if (NULL == m_pDS.get()) return false;

	    CStdString strSQL=PrepareSQL("select idParentObjectType from objectTypes where idObjectType=%i", idObjectType);
	    m_pDS2->query(strSQL.c_str());
	    while (!m_pDS2->eof())
	    {
	      ids.push_back(m_pDS2->fv(0).get_asInt());
	      m_pDS2->next();
	    }

	    m_pDS2->close();
	    return true;
	  }
	  catch (...)
	  {
	    CLog::Log(LOGERROR, "%s (%i) failed", __FUNCTION__, idObjectType);
	  }

	  return false;
}

bool CObjectDatabase::GetAllAncestorObjectTypes(int idObjectType, std::vector<int>& ids)
{
	try
		  {
		    if (NULL == m_pDB.get()) return false;
		    if (NULL == m_pDS.get()) return false;

		    std::vector<int> parentIds;
		    if(GetParentObjectTypes(idObjectType, parentIds))
		    {
		    	while(!parentIds.empty())
		    	{
		    		ids.insert(ids.end(),parentIds.begin(),parentIds.end());
		    		std::vector<int> tempIds = parentIds;
		    		parentIds.clear();
		    		for(std::vector<int>::iterator it = tempIds.begin(); it != tempIds.end(); ++it)
		    		{
		    			GetParentObjectTypes(*it,parentIds);
		    		}
		    	}
		    	return true;
		    }

		  }
		  catch (...)
		  {
		    CLog::Log(LOGERROR, "%s (%i) failed", __FUNCTION__, idObjectType);
		  }

		  return false;
}

bool CObjectDatabase::GetAllAttributesForObjectType(int idObjectType, std::vector<int>& ids)
{
	try
	{
		if (NULL == m_pDB.get()) return false;
		if (NULL == m_pDS.get()) return false;

		std::vector<int> objectIds;



		if(GetAllAncestorObjectTypes(idObjectType,objectIds))
		{
			objectIds.push_back(idObjectType);
			CStdString strSQL=PrepareSQL("select idAttributeType from attributeTypes where idObjectType IN (%s)", StringUtils::Join(objectIds,",").c_str());
			m_pDS2->query(strSQL.c_str());
			while (!m_pDS2->eof())
			{
				ids.push_back(m_pDS2->fv(0).get_asInt());
				m_pDS2->next();
			}

			m_pDS2->close();
			return true;
		}
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s (%i) failed", __FUNCTION__, idObjectType);
	}

	return false;
}



