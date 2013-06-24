/*
 * ObjectDatabase.cpp
 *
 *  Created on: Jun 19, 2013
 *      Author: zachary
 */

#include "ObjectDatabase.h"
#include "dbwrappers/dataset.h"
#include "utils/URIUtils.h"



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

			CStdString strSQL=PrepareSQL("select idAttributeType from attributeTypes where idObjectType IN (%s) AND inheritable = 1 OR idObjectType = %i", StringUtils::Join(objectIds,",").c_str(),idObjectType);
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

bool CObjectDatabase::GetAllArtworkTypesForObjectType(int idObjectType, std::vector<int>& ids)
{
	try
		{
			if (NULL == m_pDB.get()) return false;
			if (NULL == m_pDS.get()) return false;

			std::vector<int> objectIds;



			if(GetAllAncestorObjectTypes(idObjectType,objectIds))
			{

				CStdString strSQL=PrepareSQL("select idArtworkType from artworkTypes where idObjectType IN (%s) AND inheritable = 1 OR idObjectType = %i", StringUtils::Join(objectIds,",").c_str(),idObjectType);
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


int CObjectDatabase::AddObjectType(int idParentObjectType,const CStdString& stub,const CStdString& name)
{
	try
	{
		if (NULL == m_pDB.get()) return -1;
		if (NULL == m_pDS.get()) return -1;
		int idObjectType = -1;
		CStdString strSQL = PrepareSQL("SELECT idObjectType FROM objectTypes WHERE stub LIKE '%s' AND idParentObjectType = %i",stub.c_str(), idParentObjectType);
		m_pDS->query(strSQL.c_str());
		if (m_pDS->num_rows() == 0)
		{
			m_pDS->close();
			// doesnt exists, add it
			strSQL=PrepareSQL("insert into objectTypes (idObjectType, idParentObjectType, stub, name) values( NULL, %i, '%s','%s')", idParentObjectType, stub.c_str(), name.c_str());
			m_pDS->exec(strSQL.c_str());
			idObjectType = (int)m_pDS->lastinsertid();
		}
		else
		{
			idObjectType = m_pDS->fv("idObjectType").get_asInt();
			m_pDS->close();
		}

		return idObjectType;
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s (%s) failed", __FUNCTION__, name.c_str() );
	}

	return -1;
}

int CObjectDatabase::AddAttributeType(int idObjectType, CStdString& stub, CStdString& name, int dataType, int dataPrecision, int inheritable)
{
	try
		{
			if (NULL == m_pDB.get()) return -1;
			if (NULL == m_pDS.get()) return -1;
			int idAttributeType = -1;
			CStdString strSQL = PrepareSQL("SELECT idAttributeType FROM attributeTypes WHERE stub LIKE '%s' AND idObjectType = %i",stub.c_str(), idObjectType);
			m_pDS->query(strSQL.c_str());
			if (m_pDS->num_rows() == 0)
			{
				m_pDS->close();
				// doesnt exists, add it
				strSQL=PrepareSQL("insert into attributeTypes (idAttributeType, idObjectType, stub, name, dataType, dataPrecision, inheritable) values( NULL, %i, '%s','%s', %i, %i, %i)", idObjectType, stub.c_str(), name.c_str(), dataType, dataPrecision, inheritable);
				m_pDS->exec(strSQL.c_str());
				idAttributeType = (int)m_pDS->lastinsertid();
			}
			else
			{
				idAttributeType = m_pDS->fv("idAttributeType").get_asInt();
				m_pDS->close();
			}

			return idAttributeType;
		}
		catch (...)
		{
			CLog::Log(LOGERROR, "%s (%s) failed", __FUNCTION__, name.c_str() );
		}

		return -1;
}

int CObjectDatabase::GetObjectTypeID(const CStdString& stub)
{
	try
	{
		if (NULL == m_pDB.get()) return -1;
		if (NULL == m_pDS.get()) return -1;
		int idObjectType = -1;
		CStdString strSQL = PrepareSQL("SELECT idObjectType FROM objectTypes WHERE stub LIKE '%s'",stub.c_str());
		m_pDS->query(strSQL.c_str());
		if (m_pDS->num_rows() == 1)
		{
			idObjectType = m_pDS->fv("idObjectType").get_asInt();
			m_pDS->close();
		}

		return idObjectType;
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s (%s) failed", __FUNCTION__, stub.c_str() );
	}

	return -1;
}

int CObjectDatabase::GetAttributeTypeID(const CStdString& stub)
{
	try
	{
		if (NULL == m_pDB.get()) return -1;
		if (NULL == m_pDS.get()) return -1;
		int idAttributeType = -1;
		CStdString strSQL = PrepareSQL("SELECT idAttributeType FROM attributeTypes WHERE stub LIKE '%s'",stub.c_str());
		m_pDS->query(strSQL.c_str());
		if (m_pDS->num_rows() == 1)
		{
			idAttributeType = m_pDS->fv("idAttributeType").get_asInt();
			m_pDS->close();
		}

		return idAttributeType;
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s (%s) failed", __FUNCTION__, stub.c_str() );
	}

	return -1;
}

int CObjectDatabase::AddPath(const CStdString& strPath)
{
  CStdString strSQL;
  try
  {
    int idPath = GetPathId(strPath);
    if (idPath >= 0)
      return idPath; // already have the path

    if (NULL == m_pDB.get()) return -1;
    if (NULL == m_pDS.get()) return -1;

    CStdString strPath1(strPath);
    if (URIUtils::IsStack(strPath) || strPath.Mid(0,6).Equals("rar://") || strPath.Mid(0,6).Equals("zip://"))
      URIUtils::GetParentPath(strPath,strPath1);

    URIUtils::AddSlashAtEnd(strPath1);

    strSQL=PrepareSQL("insert into path (idPath, path) values (NULL,'%s')", strPath1.c_str());
    m_pDS->exec(strSQL.c_str());
    idPath = (int)m_pDS->lastinsertid();
    return idPath;
  }
  catch (...)
  {
    CLog::Log(LOGERROR, "%s unable to addpath (%s)", __FUNCTION__, strSQL.c_str());
  }
  return -1;
}

int CObjectDatabase::GetPathId(const CStdString& strPath)
{
  CStdString strSQL;
  try
  {
    int idPath=-1;
    if (NULL == m_pDB.get()) return -1;
    if (NULL == m_pDS.get()) return -1;

    CStdString strPath1(strPath);
    if (URIUtils::IsStack(strPath) || strPath.Mid(0,6).Equals("rar://") || strPath.Mid(0,6).Equals("zip://"))
      URIUtils::GetParentPath(strPath,strPath1);

    URIUtils::AddSlashAtEnd(strPath1);

    strSQL=PrepareSQL("select idPath from paths where strPath='%s'",strPath1.c_str());
    m_pDS->query(strSQL.c_str());
    if (!m_pDS->eof())
      idPath = m_pDS->fv("idPath").get_asInt();

    m_pDS->close();
    return idPath;
  }
  catch (...)
  {
    CLog::Log(LOGERROR, "%s unable to getpath (%s)", __FUNCTION__, strSQL.c_str());
  }
  return -1;
}

bool CObjectDatabase::GetPathHash(const CStdString &path, CStdString &hash)
{
  try
  {
    if (NULL == m_pDB.get()) return false;
    if (NULL == m_pDS.get()) return false;

    CStdString strSQL=PrepareSQL("select hash from paths where path='%s'", path.c_str());
    m_pDS->query(strSQL.c_str());
    if (m_pDS->num_rows() == 0)
      return false;
    hash = m_pDS->fv("hash").get_asString();
    return true;
  }
  catch (...)
  {
    CLog::Log(LOGERROR, "%s (%s) failed", __FUNCTION__, path.c_str());
  }

  return false;
}

bool CObjectDatabase::SetPathHash(const CStdString &path, const CStdString &hash)
{
  try
  {
    if (NULL == m_pDB.get()) return false;
    if (NULL == m_pDS.get()) return false;

    if (hash.IsEmpty())
    { // this is an empty folder - we need only add it to the path table
      // if the path actually exists
//      if (!CDirectory::Exists(path))
//        return false;
    }
    int idPath = AddPath(path);
    if (idPath < 0) return false;

    CStdString strSQL=PrepareSQL("update paths set hash='%s' where idPath=%ld", hash.c_str(), idPath);
    m_pDS->exec(strSQL.c_str());

    return true;
  }
  catch (...)
  {
    CLog::Log(LOGERROR, "%s (%s, %s) failed", __FUNCTION__, path.c_str(), hash.c_str());
  }

  return false;
}

int CObjectDatabase::AddScraper(const CStdString& scraper, const CStdString& content )
{
	CStdString strSQL;
	  try
	  {
	    int idScraper = GetScraperId(scraper);
	    if (idScraper >= 0)
	      return idScraper; // already have the path

	    if (NULL == m_pDB.get()) return -1;
	    if (NULL == m_pDS.get()) return -1;

	    strSQL=PrepareSQL("insert into scrapers (scraper, content) values ('%s','%s')", scraper.c_str(), content.c_str());
	    m_pDS->exec(strSQL.c_str());
	    idScraper = (int)m_pDS->lastinsertid();
	    return idScraper;
	  }
	  catch (...)
	  {
	    CLog::Log(LOGERROR, "%s unable to addscraper (%s)", __FUNCTION__, strSQL.c_str());
	  }
	  return -1;
}

int CObjectDatabase::GetScraperId(const CStdString& scraper)
{
	CStdString strSQL;
	try
	{
		int idScraper=-1;
		if (NULL == m_pDB.get()) return -1;
		if (NULL == m_pDS.get()) return -1;

		strSQL=PrepareSQL("select idScraper from scrapers where scraper='%s'",scraper.c_str());
		m_pDS->query(strSQL.c_str());
		if (!m_pDS->eof())
			idScraper = m_pDS->fv("scraper").get_asInt();

		m_pDS->close();
		return idScraper;
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s unable to getscraper (%s)", __FUNCTION__, strSQL.c_str());
	}
	return -1;
}

bool CObjectDatabase::LinkScraperToPath(int& idScraper, int& idPath)
{
	CStdString strSQL;
	try
	{
		if (NULL == m_pDB.get()) return -1;
		if (NULL == m_pDS.get()) return -1;

		strSQL=PrepareSQL("INSERT INTO pathlinkscraper (idPath, idScraper) VALUES (%i, %i)",idPath,idScraper);
		m_pDS->exec(strSQL.c_str());

		return true;
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s unable to linkscraper (%s)", __FUNCTION__, strSQL.c_str());
	}
	return false;
}

bool CObjectDatabase::LinkScraperToPath(CStdString& scraper, CStdString& path)
{
	int idScraper = GetScraperId(scraper);
	if(idScraper < 0)
		return false;

	int idPath = GetPathId(path);
	if(idPath < 0)
		idPath = AddPath(path);

	return LinkScraperToPath(idScraper, idPath);
}



