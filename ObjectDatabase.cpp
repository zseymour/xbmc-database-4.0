/*
 * ObjectDatabase.cpp
 *
 *  Created on: Jun 19, 2013
 *      Author: zachary
 */

#include "ObjectDatabase.h"
#include "dbwrappers/dataset.h"
#include "utils/URIUtils.h"
#include "URL.h"

using namespace std;

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

bool CObjectDatabase::GetAllAttributesForObjectType(int idObjectType, std::vector<CAttributeType>& types)
{
	try
	{
		if (NULL == m_pDB.get()) return false;
		if (NULL == m_pDS.get()) return false;

		std::vector<int> objectIds;



		if(GetAllAncestorObjectTypes(idObjectType,objectIds))
		{

			CStdString strSQL=PrepareSQL("select * from attributeTypes where idObjectType IN (%s) AND inheritable = 1 OR idObjectType = %i", StringUtils::Join(objectIds,",").c_str(),idObjectType);
			m_pDS2->query(strSQL.c_str());
			while (!m_pDS2->eof())
			{
				CAttributeType t;
				t.idAttributeType = m_pDS2->fv("idAttributeType").get_asInt();
				t.idObjectType = m_pDS2->fv("idObjectType").get_asInt();
				t.inheritable = m_pDS2->fv("inheritable").get_asBool();
				t.name = m_pDS2->fv("name").get_asString();
				t.stub = m_pDS2->fv("stub").get_asString();
				t.type = ATTRIBUTE_DATA_TYPE(m_pDS2->fv("dataType").get_asInt());
				t.precision = m_pDS2->fv("dataPrecision").get_asInt();


				types.push_back(t);
				//types.push_back(m_pDS2->fv(0).get_asInt());
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

void CObjectDatabase::SplitPath(const CStdString& strFileNameAndPath, CStdString& strPath, CStdString& strFileName)
{
  if (URIUtils::IsStack(strFileNameAndPath) || strFileNameAndPath.Mid(0,6).Equals("rar://") || strFileNameAndPath.Mid(0,6).Equals("zip://"))
  {
    URIUtils::GetParentPath(strFileNameAndPath,strPath);
    strFileName = strFileNameAndPath;
  }
  else if (URIUtils::IsPlugin(strFileNameAndPath))
  {
    CURL url(strFileNameAndPath);
    strPath = url.GetWithoutFilename();
    strFileName = strFileNameAndPath;
  }
  else
    URIUtils::Split(strFileNameAndPath,strPath, strFileName);
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
	if(idPath < 0)
		return false;

	return LinkScraperToPath(idScraper, idPath);
}

int CObjectDatabase::AddDirEnt(const CStdString& strFileNameAndPath)
{

	CStdString strSQL = "";
	try
	{
		int idDirent;
		if (NULL == m_pDB.get()) return -1;
		if (NULL == m_pDS.get()) return -1;

		CStdString strFileName, strPath;
		SplitPath(strFileNameAndPath,strPath,strFileName);

		int idPath = AddPath(strPath);
		if (idPath < 0)
			return -1;

		CStdString strSQL=PrepareSQL("select idDirent from dirents where filename='%s' and idPath=%i", strFileName.c_str(),idPath);

		m_pDS->query(strSQL.c_str());
		if (m_pDS->num_rows() > 0)
		{
			idDirent = m_pDS->fv("idDirent").get_asInt() ;
			m_pDS->close();
			return idDirent;
		}
		m_pDS->close();

		strSQL=PrepareSQL("insert into dirents (idDirent, idPath, filename) values(NULL, %i, '%s')", idPath, strFileName.c_str());
		m_pDS->exec(strSQL.c_str());
		idDirent = (int)m_pDS->lastinsertid();
		return idDirent;
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s unable to adddirent (%s)", __FUNCTION__, strSQL.c_str());
	}
	return -1;

}

int CObjectDatabase::AddObject(const int& idObjectType, const CStdString& stub, const CStdString& name)
{

	CStdString strSQL = "";
	try
	{
		int idObject;
		if (NULL == m_pDB.get()) return -1;
		if (NULL == m_pDS.get()) return -1;

		CStdString strSQL=PrepareSQL("select idObject from objects where stub='%s' and idObjectType=%i", stub.c_str(),idObjectType);

		m_pDS->query(strSQL.c_str());
		if (m_pDS->num_rows() > 0)
		{
			idObject = m_pDS->fv("idObject").get_asInt() ;
			m_pDS->close();
			return idObject;
		}
		m_pDS->close();

		strSQL=PrepareSQL("insert into objects (idObjectType, stub, name) values(%i, '%s', '%s')", idObjectType, stub.c_str(), name.c_str());
		m_pDS->exec(strSQL.c_str());
		idObject = (int)m_pDS->lastinsertid();
		return idObject;
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s unable to addobject (%s)", __FUNCTION__, strSQL.c_str());
	}
	return -1;
}

int CObjectDatabase::GetObjectType(int idObject)
{
	CStdString strSQL;
	try
	{
		int idObjectType=-1;
		if (NULL == m_pDB.get()) return -1;
		if (NULL == m_pDS.get()) return -1;

		strSQL=PrepareSQL("select idObjectType from objects where idObject=%i",idObject);
		m_pDS->query(strSQL.c_str());
		if (!m_pDS->eof())
			idObjectType = m_pDS->fv("idObjectType").get_asInt();

		m_pDS->close();
		return idObjectType;
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s unable to getobjecttype (%s)", __FUNCTION__, strSQL.c_str());
	}
	return -1;
}

/**
 * Adds attributes for the given object
 * attributes is of the form
 *
 * attributes[idAttributeType] = CAttribute attr;
 */
bool CObjectDatabase::AddAttributesForObject(const int& idObject, map<int, CAttribute> attributes)
{
	CStdString strSQL;
	try
	{
		int idObjectType = GetObjectType(idObject);
		if(idObjectType < 0)
			return false;

		if (NULL == m_pDB.get()) return false;
		if (NULL == m_pDS.get()) return false;

		vector<CAttributeType> types;
		if(GetAllAttributesForObjectType(idObjectType, types))
		{
			for(vector<CAttributeType>::iterator it = types.begin(); it != types.end(); ++it)
			{
				CAttribute attr;
				try
				{
					attr = attributes.at((*it).idAttributeType);
				}
				catch(...)
				{
					continue;
				}
				switch (it->type) {
				case STRING_ATTRIBUTE:
					strSQL=PrepareSQL("INSERT INTO attributes (idObject, idAttributeType, valueString) VALUES (%i, %i '%s')",idObject, it->idAttributeType, attr.strValue);
					break;
				case NUMBER_ATTRIBUTE:
					strSQL=PrepareSQL("INSERT INTO attributes (idObject, idAttributeType, valueNumber) VALUES (%i, %i '%s')",idObject, it->idAttributeType, attr.intValue);
					break;
				case BLOB_ATTRIBUTE:
					break;
				}

				m_pDS->exec(strSQL.c_str());
			}
			return true;
		}


	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s unable to addattributes (%s)", __FUNCTION__, strSQL.c_str());
	}
	return false;
}


