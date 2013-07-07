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

bool CObjectDatabase::GetAllAttributeTypeIDsForObjectType(int idObjectType, std::vector<int>& types)
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

				types.push_back(m_pDS2->fv("idAttributeType").get_asInt());
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

bool CObjectDatabase::GetAllAttributeTypesForObjectType(int idObjectType, std::vector<CAttributeType>& types)
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

int CObjectDatabase::GetObjectTypeId(const CStdString& stub)
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

int CObjectDatabase::GetAttributeTypeId(const CStdString& stub)
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

    int parentId = GetPathId(URIUtils::GetParentPath(strPath1));
    if(parentId < 0)
    	parentId = 0;

    strSQL=PrepareSQL("insert into paths (idPath, idParent, path) values (NULL,%i,'%s')", parentId, strPath1.c_str());
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

    strSQL=PrepareSQL("select idPath from paths where path='%s'",strPath1.c_str());
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
	      return idScraper; // already have the scraper

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
		if (m_pDS->num_rows() > 0)
			idScraper = m_pDS->fv("idScraper").get_asInt();

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

bool CObjectDatabase::LinkObjectToDirent(int& idObject, int& idDirent)
{
	CStdString strSQL;
	try
	{
		if (NULL == m_pDB.get()) return -1;
		if (NULL == m_pDS.get()) return -1;

		strSQL=PrepareSQL("INSERT INTO objectlinkdirent (idObject, idDirent) VALUES (%i, %i)",idObject,idDirent);
		m_pDS->exec(strSQL.c_str());

		return true;
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s unable to linkobject (%s)", __FUNCTION__, strSQL.c_str());
	}
	return false;
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
 *
 * Loops through all possible attribute types for the given object,
 * checking if they are in the map
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
		if(GetAllAttributeTypesForObjectType(idObjectType, types))
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
				int attrId = GetAttributeId(idObject,it->idAttributeType);

				SetAttribute(idObject, *it, attr, attrId);

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

/**
 * Converts attribute type stubs to IDs and passes them along
 */
bool CObjectDatabase::AddAttributesForObject(const int& idObject, map<CStdString, CAttribute> attributes)
{
	typedef map<CStdString, CAttribute>::iterator it_type;
	map<int, CAttribute> newAttributes;
	for(it_type iterator = attributes.begin(); iterator != attributes.end(); ++iterator)
	{
		int idKey = GetAttributeTypeId(iterator->first);
		if(idKey >= 0)
			newAttributes[idKey] = iterator->second;
	}

	return AddAttributesForObject(idObject, newAttributes);
}

bool CObjectDatabase::SetAttribute(const int idObject, CAttributeType attrType, CAttribute attr, int idAttribute)
{
	CStdString strSQL;
	try
	{

		if(!isValidAttributeType(idObject, attrType.idAttributeType)) return false;

		switch (attrType.type) {
		case STRING_ATTRIBUTE:
			if(idAttribute < 0)
			{
				strSQL=PrepareSQL("INSERT INTO attributes (idObject, idAttributeType, valueString) VALUES (%i, %i, '%s')",idObject, attrType.idAttributeType, attr.strValue.c_str());
			}
			else
			{
				strSQL=PrepareSQL("UPDATE attributes SET valueString='%s' WHERE idAttribute=%i", attr.strValue.c_str(), idAttribute);
			}
			break;
		case NUMBER_ATTRIBUTE:
			if(idAttribute < 0)
			{
				strSQL=PrepareSQL("INSERT INTO attributes (idObject, idAttributeType, valueNumber) VALUES (%i, %i, %i)",idObject, attrType.idAttributeType, attr.intValue);
			}
			else
			{
				strSQL=PrepareSQL("UPDATE attributes SET valueNumber=%i WHERE idAttribute=%i", attr.intValue, idAttribute);
			}
			break;
		case BLOB_ATTRIBUTE:
			break;
		}

		m_pDS->exec(strSQL.c_str());

		return true;
	}
	catch(...)
	{
		CLog::Log(LOGERROR, "%s unable to setattribute (%s)", __FUNCTION__, strSQL.c_str());
	}

	return false;
}


bool CObjectDatabase::isValidAttributeType(int idObject, int idAttributeType)
{
	CStdString strSQL;
	try
	{
		if (NULL == m_pDB.get()) return false;
		if (NULL == m_pDS.get()) return false;

		int idObjectType = GetObjectType(idObject);
		vector<int> attributeTypes;
		if(GetAllAttributeTypeIDsForObjectType(idObjectType, attributeTypes))
		{
			return std::find(attributeTypes.begin(), attributeTypes.end(), idAttributeType) != attributeTypes.end();
		}

	}
	catch(...)
	{
		CLog::Log(LOGERROR, "%s unable to checkvalidity (%s)", __FUNCTION__, strSQL.c_str());
	}

	return false;
}


int CObjectDatabase::GetAttributeId(int idObject, int idAttributeType)
{
	CStdString strSQL;
	try
	{
		int idAttribute=-1;
		if (NULL == m_pDB.get()) return -1;
		if (NULL == m_pDS.get()) return -1;

		strSQL=PrepareSQL("select idAttribute from attributes where idObject=%i AND idAttributeType=%i",idObject,idAttributeType);
		m_pDS->query(strSQL.c_str());
		if (!m_pDS->eof())
			idAttribute = m_pDS->fv("idAttribute").get_asInt();

		m_pDS->close();
		return idAttribute;
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s unable to getattributeid (%s)", __FUNCTION__, strSQL.c_str());
	}
	return -1;
}

bool CObjectDatabase::GetAttribute(const int idAttribute, CAttribute& attribute)
{
	CStdString strSQL;
		try
		{

			if (NULL == m_pDB.get()) return false;
			if (NULL == m_pDS.get()) return false;

			strSQL=PrepareSQL("SELECT * FROM attributes AS a INNER JOIN attributeTypes AS at ON a.idAttributeType = at.idAttributeType WHERE idAttribute=%i", idAttribute);
			m_pDS2->query(strSQL.c_str());
			if(!m_pDS2->eof())
			{
				CAttributeType t;
				t.idAttributeType = m_pDS2->fv("at.idAttributeType").get_asInt();
				t.idObjectType = m_pDS2->fv("at.idObjectType").get_asInt();
				t.inheritable = m_pDS2->fv("at.inheritable").get_asBool();
				t.name = m_pDS2->fv("at.name").get_asString();
				t.stub = m_pDS2->fv("at.stub").get_asString();
				t.type = ATTRIBUTE_DATA_TYPE(m_pDS2->fv("at.dataType").get_asInt());
				t.precision = m_pDS2->fv("at.dataPrecision").get_asInt();

				attribute.intValue = m_pDS2->fv("a.valueNumber").get_asInt();
				attribute.strValue = m_pDS2->fv("a.valueString").get_asString();
				attribute.type = t;

				m_pDS2->close();
				return true;
			}



		}
		catch (...)
		{
			CLog::Log(LOGERROR, "%s unable to getattribute (%s)", __FUNCTION__, strSQL.c_str());
		}
		return false;
}

bool CObjectDatabase::isValidRelationshipType(int idRelationshipType, int idObject1, int idObject2)
{
	CStdString strSQL;
	try
	{
		if (NULL == m_pDB.get()) return false;
		if (NULL == m_pDS.get()) return false;

		int idObjectType1 = GetObjectType(idObject1);
		int idObjectType2 = GetObjectType(idObject2);
		vector< pair<int,int> > relationships;
		if(GetAllRelationshipTypeIDsForObjectType(idObjectType1, relationships))
		{
			return std::find(relationships.begin(), relationships.end(), make_pair(idRelationshipType,idObjectType2)) != relationships.end();
		}

	}
	catch(...)
	{
		CLog::Log(LOGERROR, "%s unable to checkvalidity (%s)", __FUNCTION__, strSQL.c_str());
	}

	return false;
}

int CObjectDatabase::GetRelationshipTypeId(const CStdString stub)
{
	try
	{
		if (NULL == m_pDB.get()) return -1;
		if (NULL == m_pDS.get()) return -1;
		int idRelationshipType = -1;
		CStdString strSQL = PrepareSQL("SELECT idRelationshipType FROM relationshipTypes WHERE stub LIKE '%s'",stub.c_str());
		m_pDS->query(strSQL.c_str());
		if (m_pDS->num_rows() == 1)
		{
			idRelationshipType = m_pDS->fv("idRelationshipType").get_asInt();
			m_pDS->close();
		}

		return idRelationshipType;
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s (%s) failed", __FUNCTION__, stub.c_str() );
	}

	return -1;
}

bool CObjectDatabase::GetAllRelationshipTypeIDsForObjectType(int idObjectType, std::vector<pair <int,int> >& types)
{

	try
	{
		if (NULL == m_pDB.get()) return false;
		if (NULL == m_pDS.get()) return false;

		std::vector<int> objectIds;



		if(GetAllAncestorObjectTypes(idObjectType,objectIds))
		{

			CStdString strSQL=PrepareSQL("select idRelationshipType, idObjectType2 from relationshipTypes where idObjectType1 IN (%s) AND inheritableType1 = 1 OR idObjectType1 = %i", StringUtils::Join(objectIds,",").c_str(),idObjectType);
			m_pDS2->query(strSQL.c_str());
			while (!m_pDS2->eof())
			{
				int idRelationshipType = m_pDS2->fv("idRelationshipType").get_asInt();
				int idObjectType2 = m_pDS2->fv("idObjectType2").get_asInt();
				types.push_back(make_pair(idRelationshipType, idObjectType2));
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

int CObjectDatabase::GetRelationshipId(int idRelationshipType, int idObject1, int idObject2, CStdString link, int index)
{
	CStdString strSQL;
	try
	{
		int idRelationship=-1;
		if (NULL == m_pDB.get()) return -1;
		if (NULL == m_pDS.get()) return -1;

		strSQL = PrepareSQL("SELECT idRelationship FROM relationships WHERE idRelationshipType=%i AND idObject1=%i AND idObject2=%i AND link LIKE '%s' AND sequenceIndex=%i",
				idRelationshipType, idObject1, idObject2, link.c_str(), index);

		m_pDS->query(strSQL.c_str());
		if (!m_pDS->eof())
			idRelationship = m_pDS->fv("idRelationship").get_asInt();

		m_pDS->close();
		return idRelationship;
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s unable to getrelationship (%s)", __FUNCTION__, strSQL.c_str());
	}
	return -1;
}

int CObjectDatabase::LinkObjectToObject(int idRelationshipType, int idObject1, int idObject2, CStdString link, int index)
{
	CStdString strSQL;
		try
		{
			if (NULL == m_pDB.get()) return -1;
			if (NULL == m_pDS.get()) return -1;

			int idRelationship = GetRelationshipId(idRelationshipType, idObject1, idObject2, link, index);

			if(idRelationship >= 0)
				return idRelationship;


			if(isValidRelationshipType(idRelationshipType, idObject1, idObject2))
			{
				strSQL=PrepareSQL("INSERT INTO relationships (idRelationshipType, idObject1, idObject2,link, sequenceIndex) VALUES (%i, %i, %i, '%s', %i)",idRelationshipType,idObject1,idObject2,link.c_str(),index);
				m_pDS->exec(strSQL.c_str());

				return m_pDS->lastinsertid();
			}


		}
		catch (...)
		{
			CLog::Log(LOGERROR, "%s unable to linkobject (%s)", __FUNCTION__, strSQL.c_str());
		}
		return -1;
}

bool CObjectDatabase::GetAllArtworkTypeIDsForObjectType(int idObjectType, std::vector<int>& types)
{

	try
	{
		if (NULL == m_pDB.get()) return false;
		if (NULL == m_pDS.get()) return false;

		std::vector<int> objectIds;



		if(GetAllAncestorObjectTypes(idObjectType,objectIds))
		{

			CStdString strSQL=PrepareSQL("select idArtworkType from attributeTypes where idObjectType IN (%s) AND inheritable = 1 OR idObjectType = %i", StringUtils::Join(objectIds,",").c_str(),idObjectType);
			m_pDS2->query(strSQL.c_str());
			while (!m_pDS2->eof())
			{

				types.push_back(m_pDS2->fv("idArtworkType").get_asInt());
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

int CObjectDatabase::AddProfile(CStdString name)
{
	CStdString strSQL;
	try
	{
		if (NULL == m_pDB.get()) return -1;
		if (NULL == m_pDS.get()) return -1;

		int idProfile = GetProfileId(name);

		if(idProfile >= 0)
			return idProfile;



		strSQL=PrepareSQL("INSERT INTO profile (name) VALUES ('%s')", name.c_str());
		m_pDS->exec(strSQL.c_str());

		return m_pDS->lastinsertid();



	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s unable to addprofile (%s)", __FUNCTION__, strSQL.c_str());
	}
	return -1;
}

int CObjectDatabase::GetProfileId(CStdString name)
{
	CStdString strSQL;
	try
	{
		int idProfile=-1;
		if (NULL == m_pDB.get()) return -1;
		if (NULL == m_pDS.get()) return -1;

		strSQL = PrepareSQL("SELECT idProfile FROM profile WHERE name LIKE '%s'", name.c_str());

		m_pDS->query(strSQL.c_str());
		if (!m_pDS->eof())
			idProfile = m_pDS->fv("idRelationship").get_asInt();

		m_pDS->close();
		return idProfile;
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s unable to getprofile (%s)", __FUNCTION__, strSQL.c_str());
	}
	return -1;
}

/// \brief GetStackTimes() obtains any saved video times for the stacked file
/// \retval Returns true if the stack times exist, false otherwise.
bool CObjectDatabase::GetStackTimes(const CStdString &filePath, vector<int> &times)
{
  try
  {
    // obtain the FileID (if it exists)
    int idFile = AddDirEnt(filePath);
    if (idFile < 0) return false;
    if (NULL == m_pDB.get()) return false;
    if (NULL == m_pDS.get()) return false;
    // ok, now obtain the settings for this file
    CStdString strSQL=PrepareSQL("select times from stacktimes where idDirent=%i\n", idFile);
    m_pDS->query( strSQL.c_str() );
    if (m_pDS->num_rows() > 0)
    { // get the video settings info
      CStdStringArray timeString;
      int timeTotal = 0;
      StringUtils::SplitString(m_pDS->fv("times").get_asString(), ",", timeString);
      times.clear();
      for (unsigned int i = 0; i < timeString.size(); i++)
      {
        times.push_back(atoi(timeString[i].c_str()));
        timeTotal += atoi(timeString[i].c_str());
      }
      m_pDS->close();
      return (timeTotal > 0);
    }
    m_pDS->close();
  }
  catch (...)
  {
    CLog::Log(LOGERROR, "%s failed", __FUNCTION__);
  }
  return false;
}

/// \brief Sets the stack times for a particular video file
void CObjectDatabase::SetStackTimes(const CStdString& filePath, vector<int> &times)
{
  try
  {
    if (NULL == m_pDB.get()) return ;
    if (NULL == m_pDS.get()) return ;
    int idFile = AddDirEnt(filePath);
    if (idFile < 0)
      return;

    // delete any existing items
    m_pDS->exec( PrepareSQL("delete from stacktimes where idDirent=%i", idFile) );

    // add the items
    CStdString timeString;
    timeString.Format("%i", times[0]);
    for (unsigned int i = 1; i < times.size(); i++)
    {
      CStdString time;
      time.Format(",%i", times[i]);
      timeString += time;
    }
    m_pDS->exec( PrepareSQL("insert into stacktimes (idDirent,times) values (%i,'%s')\n", idFile, timeString.c_str()) );
  }
  catch (...)
  {
    CLog::Log(LOGERROR, "%s (%s) failed", __FUNCTION__, filePath.c_str());
  }
}


