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
#include "utils/StreamDetails.h"
#include "utils/XBMCTinyXML.h"
#include "utils/XMLUtils.h"
#include "settings/VideoSettings.h"
#include "Bookmark.h"
#include "Attribute.h"
#include "Relationship.h"
#include "ObjectInfoTag.h"

#include <memory>
#include <set>
#include <map>

namespace dbiplus
{
  class field_value;
  typedef std::vector<field_value> sql_record;
}

typedef enum
{
	FIRST_OBJECT,
	SECOND_OBJECT
} OBJECT_RELATIONSHIP_POSITION;

enum ObjectTypeID
{
	OBJ_OBJECT = 1,
	OBJ_CONTENT,
	OBJ_VIDEO,
	OBJ_MOVIE,
	OBJ_MUSICVIDEO,
	OBJ_TVSHOW,
	OBJ_EPISODE,
	OBJ_SONG,
	OBJ_PICTURE,
	OBJ_ORGANISATION,
	OBJ_PERSON,
	OBJ_ACTOR,
	OBJ_DIRECTOR,
	OBJ_WRITER,
	OBJ_MUSICIAN,
	OBJ_BAND,
	OBJ_STUDIO,
	OBJ_COUNTRY,
	OBJ_GROUPING,
	OBJ_MOVIESET,
	OBJ_SEASON,
	OBJ_GENRE,
	OBJ_YEAR,
	OBJ_ALBUM,
	OBJ_PLAYLIST,
	OBJ_TAG,
	OBJ_ADDON,
	OBJ_REPO
};

enum AttributeTypeID
{
	FILENAME_STR = 1,
	RELEASEDATE_STR,
	USERRATING_NUM,
	ONLINEID_STR,
	CONTENT_SORTTITLE_STR,
	CONTENT_ORIGINALTITLE_STR,
	VIDEO_SUMMARY_STR,
	VOTES_STR,
	CONTENTRATING_STR,
	ONLINERATING_NUM,
	TAGLINE_STR,
	MOVIE_PLOT_STR,
	MOVIE_PLOTOUTLINE_STR,
	MOVIE_RANKING_NUM,
	MOVIE_TRAILER_URL_STR,
	TVSHOW_STATUS_STR,
	TVSHOW_EPISODEGUIDE_STR,
	EPISODE_PLOT_STR,
	EPISODE_PRODUCTIONCODE_STR,
	EPISODE_SEASONSORT_NUM,
	EPISODE_EPISODESORT_NUM,
	HEIGHT_NUM,
	WIDTH_NUM,
	BIOGRAPHY_STR,
	DATE_OF_BIRTH_STR,
	GROUP_DESCRIPTION_STR,
	ADDON_TYPE_STR,
	ADDON_SUMMARY_STR,
	ADDON_DESCRIPTION_STR,
	ADDON_RATING_NUM,
	ADDON_ID_STR,
	ADDON_VERSION_STR,
	ADDON_AUTHOR_STR,
	REPO_ID_STR
};

enum RelationshipTypeID
{
	OBJECT_HAS_TAG = 1,
	CONTENT_HAS_YEAR,
	VIDEO_HAS_DIRECTOR,
	VIDEO_HAS_STUDIO,
	VIDEO_HAS_GENRE,
	MOVIE_HAS_ACTOR,
	MOVIE_HAS_WRITER,
	MOVIE_HAS_COUNTRY,
	MOVIE_LINK_TVSHOW,
	TVSHOW_HAS_ACTOR,
	TVSHOW_HAS_SEASON,
	EPISODE_HAS_ACTOR,
	EPISODE_HAS_WRITER,
	MUSICVIDEO_HAS_MUSICIAN,
	MUSICVIDEO_HAS_BAND,
	MUSICIAN_HAS_SONG,
	MUSICIAN_HAS_ALBUM,
	BAND_HAS_SONG,
	BAND_HAS_ALBUM,
	BAND_HAS_MUSICIAN,
	SEASON_HAS_EPISODE,
	MOVIESET_HAS_MOVIE,
	ALBUM_HAS_STUDIO,
	ALBUM_HAS_SONG,
	ALBUM_HAS_MUSICVIDEO,
	ALBUM_HAS_GENRE,
	REPO_HAS_ADDON
};

enum ArtworkTypeID
{
	THUMBNAIL = 1,
	FANART,
	BANNER,
	LANDSCAPE,
	CLEARLOGO,
	CLEARART,
	DISCART,
	CDART
};

class CObjectDatabase: public CDatabase {
public:
	CObjectDatabase();
	CObjectDatabase(DatabaseSettings s);
	virtual ~CObjectDatabase();
	virtual bool Open();

	static int ArtworkTypeToID(CStdString type)
	  {
		  if(type=="thumb")
		  {
			  return THUMBNAIL;
		  }
		  else if(type=="fanart")
		  {
			  return FANART;
		  }
		  else if(type=="banner")
		  {
		      return BANNER;
		  }
		  else if(type=="landscape")
		  {
			  return LANDSCAPE;
		  }
		  else if(type=="clearlogo")
		  {
			  return CLEARLOGO;
		  }
		  else if(type=="clearart")
		  {
			  return CLEARART;
		  }
		  else if(type=="discart")
		  {
			  return DISCART;
		  }
		  else if(type=="cdart")
		  {
			  return CDART;
		  }
		  else
		  {
			  return -1;
		  }
	  }

	int RunQuery(const CStdString &sql);
	int RunQuery(std::auto_ptr<dbiplus::Dataset> &pDS, const CStdString &sql);
	void GetDataSet(std::auto_ptr<dbiplus::Dataset> &pDS);

	bool GetChildObjectTypes(int idObjectType, std::vector<int>& ids);
	bool GetParentObjectTypes(int idObjectType, std::vector<int>& ids);
	bool GetAllDescendentObjectTypes(int idObjectType, std::vector<int>& ids);
	bool GetAllAncestorObjectTypes(int idObjectType, std::vector<int>& ids);

	bool GetAllAttributeTypesForObjectType(int idObjectType, std::vector<CAttributeType>& types);
	bool GetAllAttributeTypeIDsForObjectType(int idObjectType, std::vector<int>& types);
	bool GetAllArtworkTypesForObjectType(int idObjectType, std::vector<int>& ids);

	int AddObjectType(int idParentObjectType,const CStdString& stub,const CStdString& name);
	int GetObjectTypeId(const CStdString& stub);
	void DeleteObjectType(int idObjectType);

	int AddAttributeType(int idObjectType, CStdString& stub, CStdString& name, int dataType = 0, int dataPrecision = 0, int inheritable = 0);
	int GetAttributeTypeId(const CStdString& stub);

	int AddPath(const CStdString& strPath);
	int GetPathId(const CStdString& strPath);
	bool GetFilePathById(const int idObject, CStdString& path);
	bool GetSubPaths(const CStdString &basepath, std::vector< std::pair<int,std::string> >& subpaths);
	bool GetPaths(std::set<CStdString> &paths, int idObjectType);
	bool GetPathsForObject(const int idObject, std::set<int>& paths);
	bool GetPathHash(const CStdString &path, CStdString &hash);
	bool SetPathHash(const CStdString &path, const CStdString &hash);
	void InvalidatePathHash(const CStdString& strPath);
	void SplitPath(const CStdString& strFileNameAndPath, CStdString& strPath, CStdString& strFileName);

	int AddScraper(const CStdString& scraper, const CStdString& content);
	int AddScraper(const CStdString& scraper, const CStdString& content, const CStdString& settings, int recurse, bool useFolderNames, bool noUpdate);
	int GetScraperId(const CStdString& scraper);
	bool LinkScraperToPath(int idScraper, int idPath);
	bool LinkScraperToPath(CStdString& scraper, CStdString& path);
	bool ScraperInUse(const int idScraper);
	void ExcludePathFromScraping(const int idPath);
	bool GetScraperForPath(const int idPath, CStdString& strScraper, CStdString& content, int& scanRecursive, bool& useFolderNames, CStdString& settings, bool& noUpdate, bool& exclude);

	int GetFileId(const CStdString& strFileNameAndPath);
	int AddDirEnt(const CStdString& strFileNameAndPath);
	void UpdateDirentDateAdded(const int idDirent, CDateTime dateAdded);

	bool GetObjectDetails(int idObject, CObjectInfoTag& details);
	bool GetObjectDetails(CObjectInfoTag& details);
	CStdString GetObjectName(int idObject);
	void GetAllAttributesForObject(CObjectInfoTag& tag);
	void GetAllRelationships(CObjectInfoTag& tag, int idRelationshipType = 0);
	int AddObject(const int& idObjectType, const CStdString& stub, const CStdString& name);
	void UpdateObjectName(const int idObject, CStdString name);
	void DeleteObject(int idObject, bool bKeep = false);
	void DeleteObject(CStdString strFileNameAndPath, int idObject, bool bKeep = false);
	bool LinkObjectToDirent(int& idObject, int& idDirent);
	void RemoveObjectDirentLink(int idObject);
	int GetObjectType(int idObject);
	int GetObjectId(CStdString strFileNameAndPath);
	bool GetObjectPath(int idObject, CStdString& strFileNameAndPath);
	bool GetObjectByAttribute(CStdString attr, const int idAttributeType, std::vector<int>& object_ids);
	bool GetObjectByAttribute(const int attr, const int idAttributeType, std::vector<int>& object_ids);

	bool AddAttributesForObject(const int& idObject, std::map<int, CAttribute> attributes);
	bool AddAttributesForObject(const int& idObject, std::map<CStdString, CAttribute> attributes);
	void DeleteAttributesForObject(int idObject);
	int GetAttributeId(int idObject, int idAttributeType);
	bool SetAttribute(const int idObject, CAttributeType attrType, CAttribute attr, int idAttribute=-1);
	bool GetAttribute(const int idAttribute, CAttribute& attribute);
	bool SetAttribute(const int idObject, int idAttributeType, CStdString attrValue, int idAttribute=-1);
	bool SetAttribute(const int idObject, int idAttributeType, float attrValue, int idAttribute=-1);
	void GetAllAttributesForObject(const int idObject, AttributeList& attributes);
	bool GetAttributeType(const int idAttributeType, CAttributeType& attributeType);

	int AddRelationshipType(CStdString stub, int idObjectType1, int idObjectType2, int inheritableType1, int inheritableType2, int sequenced);
	int GetRelationshipTypeId(const CStdString stub);
	bool GetAllRelationshipTypeIDsForObjectType(int idObjectType, std::vector<std::pair <int,int> >& types);
	int GetRelationshipId(int idRelationshipType, int idObject1, int idObject2, CStdString link, int index = 0);
	int LinkObjectToObject(int idRelationshipType, int idObject1, int idObject2, CStdString link="", int index = 0, bool remove = false);
	void DeleteObjectLinks(int idObject, int idRelationshipType = 0);
	bool GetLinksForObject(int idObject, int idRelationshipType, std::vector<std::pair <int,int> >& objects, OBJECT_RELATIONSHIP_POSITION position = FIRST_OBJECT, int index = -1, bool sort = false);
	bool GetRelationship(const int idRelationship, CRelationship& relationship);
	bool GetAllRelationships(const int idObject, std::vector<CRelationship>& relations, int idRelationshipType = 0);
	bool HasRelationship(const int idObject, const int idRelationshipType = 0);
	bool HasRelations(const int idRelationshipType);
	int GetNextSequenceIndex(const int idObject1, const int idRelationshipType);


	int AddArtworkType(int idObjectType, CStdString stub, CStdString name, int inheritable);
	bool GetAllArtworkTypeIDsForObjectType(int idObjectType, std::vector<int>& types);
	CStdString GetArtworkTypeStub(int idArtworkType);
	CStdString GetArtForItem(int idObject, const int &idArtworkType);
	bool GetArtForItem(int idObject, std::map<std::string, std::string> &art);
	void SetArtForItem(int idObject, const int idArtworkType, const CStdString &url);
	void SetArtForItem(int idObject, const std::map<int, std::string> &art);

	int AddProfile(CStdString name);
	int GetProfileId(CStdString name);

	bool GetStackTimes(const CStdString &filePath, std::vector<int> &times);
	void SetStackTimes(const CStdString& filePath, std::vector<int> &times);

	void GetBookMarksForFile(const CStdString& strFilenameAndPath, int idProfile, VECBOOKMARKS& bookmarks, CBookmark::EType type /*= CBookmark::STANDARD*/, bool bAppend);
	bool GetResumeBookMark(const CStdString& strFilenameAndPath, int idProfile, CBookmark &bookmark);
	bool GetResumePoint(CObjectInfoTag& tag);
	void DeleteResumeBookMark(const CStdString &strFilenameAndPath, int idProfile);
	void AddBookMarkToFile(const CStdString& strFilenameAndPath, int idProfile, const CBookmark &bookmark, CBookmark::EType type /*= CBookmark::STANDARD*/);
	void ClearBookMarkOfFile(const CStdString& strFilenameAndPath, int idProfile, CBookmark& bookmark, CBookmark::EType type /*= CBookmark::STANDARD*/);
	void ClearBookMarksOfFile(const CStdString& strFilenameAndPath, int idProfile, CBookmark::EType type /*= CBookmark::STANDARD*/);

	void SetStreamDetailsForFile(const CStreamDetails& details, const CStdString &strFileNameAndPath);
	void SetStreamDetailsForFileId(const CStreamDetails& details, int idFile);
	void SetStreamDetailsForFileId(const CStdString detailsXML, int idFile);
	bool GetStreamDetails(CStreamDetails& details, int idFile);
	bool GetStreamDetails(CObjectInfoTag& tag);

	void SetVideoSettingsForFile(const CVideoSettings& settings, const CStdString &strFileNameAndPath);
	void SetVideoSettingsForFileId(const CVideoSettings& settings, int idFile);
	void SetVideoSettingsForFileId(const CStdString settingsXML, int idFile);
	bool GetVideoSettings(CVideoSettings& settings, int idFile);
	bool GetVideoSettings(CObjectInfoTag& tag);
	void EraseVideoSettings();

	int GetPlayCount(const int idObject, const int idProfile);
	void SetPlayCount(const int idObject, const int idProfile, int count, const CDateTime &date = CDateTime());
	void IncrementPlayCount(const int idObject, const int idProfile);
	void UpdateLastPlayed(const int idObject, const int idProfile);
	CDateTime GetLastPlayed(const int idObject, const int idProfile);

	bool HasContent(const int idObjectType);
	int GetObjectTypeCount(const int idObjectType);

	void ImportVideoFromXML(CStdString &path);
private:
	DatabaseSettings settings;
	const char *GetBaseDBName() const { return "MyObject"; };
	virtual int GetMinVersion() const;
	virtual bool CreateTables();
	virtual void CreateViews();
	virtual void InsertDefaults();
	void ConstructPath(CStdString& strDest, const CStdString& strPath, const CStdString& strFileName);
	bool isValidAttributeType(int idObject, int idAttributeType);
	bool isValidRelationshipType(int idRelationshipType, int idObject1, int idObject2);
	bool isValidArtworkType(int idObject, int idArtworkType);
	bool ParseVideoSettings(CStdString xml, CVideoSettings& settings);
	bool ParseStreamDetails(CStdString xml, CStreamDetails& details);
	CStdString LoadFile(CStdString &path);
};

#endif /* OBJECTDATABASE_H_ */
