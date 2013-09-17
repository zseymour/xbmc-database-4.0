/*
 * ObjectDatabase.cpp
 *
 *  Created on: Jun 19, 2013
 *      Author: zachary
 */

#include "ObjectDatabase.h"
#include <fstream>
#include "dbwrappers/dataset.h"
#include "utils/URIUtils.h"
#include "URL.h"
#include "VideoInfoTag.h"

using namespace std;
using namespace dbiplus;


CObjectDatabase::CObjectDatabase()
{

}

CObjectDatabase::CObjectDatabase(DatabaseSettings s)
{
	settings.host = s.host;
	settings.name = s.name;
	settings.type = s.type;
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

bool CObjectDatabase::CreateTables()
{
	BeginTransaction();
	try
	{
		CDatabase::CreateTables();
		CLog::Log(LOGINFO, "create object types table");
		m_pDS->exec("CREATE  TABLE IF NOT EXISTS objectTypes (\n"
				"  idObjectType        INTEGER       NOT NULL  PRIMARY KEY  AUTOINCREMENT,\n"
				"  idParentObjectType  INTEGER       NOT NULL  DEFAULT 0 ,\n"
				"  stub                VARCHAR(128)  NOT NULL ,\n"
				"  name                TEXT\n"
				")");

		CLog::Log(LOGINFO, "create attribute types table");
		m_pDS->exec("CREATE  TABLE IF NOT EXISTS attributeTypes (\n"
				"  idAttributeType  INTEGER       NOT NULL  PRIMARY KEY AUTOINCREMENT ,\n"
				"  idObjectType     INTEGER       NOT NULL ,\n"
				"  stub             VARCHAR(128)  NOT NULL ,\n"
				"  name             TEXT          NOT NULL ,\n"
				"  dataType         INTEGER       NOT NULL  DEFAULT 0 ,\n"
				"  dataPrecision    INTEGER       NOT NULL  DEFAULT 0 ,\n"
				"  inheritable      INTEGER       NOT NULL  DEFAULT 0 ,\n"
				"\n"
				"  FOREIGN KEY (idObjectType) REFERENCES objectTypes (idObjectType)\n"
				");");

		CLog::Log(LOGINFO, "create relationship types table");
		m_pDS->exec("CREATE  TABLE IF NOT EXISTS relationshipTypes (\n"
				"  idRelationshipType  INTEGER       NOT NULL  PRIMARY KEY  AUTOINCREMENT ,\n"
				"  stub                VARCHAR(128)  NOT NULL ,\n"
				"  idObjectType1       INTEGER       NOT NULL ,\n"
				"  idObjectType2       INTEGER       NOT NULL ,\n"
				"  inheritableType1    INTEGER       NOT NULL  DEFAULT 0,\n"
				"  inheritableType2    INTEGER       NOT NULL  DEFAULT 0,\n"
				"  sequenced           INTEGER       NOT NULL  DEFAULT 0 ,\n"
				"\n"
				"  FOREIGN KEY (idObjectType1) REFERENCES objectTypes(idObjectType) ,\n"
				"  FOREIGN KEY (idObjectType2) REFERENCES objectTypes(idObjectType)\n"
				");");

		CLog::Log(LOGINFO, "create artwork types table");
		m_pDS->exec("CREATE TABLE IF NOT EXISTS artworkTypes (\n"
				"  idArtworkType INTEGER       NOT NULL PRIMARY KEY AUTOINCREMENT ,\n"
				"  idObjectType  INTEGER       NOT NULL ,\n"
				"  stub          VARCHAR(128)  NOT NULL ,\n"
				"  name          TEXT          NOT NULL ,\n"
				"  inheritable   INTEGER       NOT NULL DEFAULT 0,\n"
				"\n"
				"  FOREIGN KEY (idObjectType) REFERENCES objectTypes(idObjectType)\n"
				");");

		CLog::Log(LOGINFO, "create objects table");
		m_pDS->exec("CREATE  TABLE IF NOT EXISTS objects (\n"
				"  idObject      INTEGER       NOT NULL  PRIMARY KEY AUTOINCREMENT ,\n"
				"  idObjectType  INTEGER       NOT NULL ,\n"
				"  stub          VARCHAR(128)  NOT NULL ,\n"
				"  name          TEXT          NULL ,\n"
				"         \n"
				"  FOREIGN KEY (idObjectType) REFERENCES objectTypes(idObjectType)\n"
				");");

		CLog::Log(LOGINFO, "create attributes table");
		m_pDS->exec("CREATE  TABLE IF NOT EXISTS attributes (\n"
				"  idAttribute      INTEGER       NOT NULL  PRIMARY KEY AUTOINCREMENT ,\n"
				"  idObject         INTEGER       NOT NULL ,\n"
				"  idAttributeType  INTEGER       NOT NULL ,\n"
				"  valueString      TEXT          NULL  DEFAULT \"\" ,\n"
				"  valueNumber      INTEGER       NULL  DEFAULT 0 ,\n"
				"  valueBlob        BLOB          NULL ,\n"
				"\n"
				"  FOREIGN KEY (idObject) REFERENCES objects(idObject) ,\n"
				"  FOREIGN KEY (idAttributeType) REFERENCES attributeTypes(idAttributeType)\n"
				");");

		CLog::Log(LOGINFO, "create relationships table");
		m_pDS->exec("CREATE  TABLE IF NOT EXISTS relationships (\n"
				"  idRelationship      INTEGER       NOT NULL  PRIMARY KEY  AUTOINCREMENT ,\n"
				"  idObject1           INTEGER       NOT NULL ,\n"
				"  idObject2           INTEGER       NOT NULL ,\n"
				"  idRelationshipType  INTEGER       NOT NULL ,\n"
				"  link                TEXT          NULL      DEFAULT NULL,-- used for actor roles\n"
				"  sequenceIndex       INTEGER       NOT NULL  DEFAULT 0 ,\n"
				"           \n"
				"  FOREIGN KEY (idObject1) REFERENCES objects(idObject) ,\n"
				"  FOREIGN KEY (idObject2) REFERENCES objects(idObject) ,\n"
				"  FOREIGN KEY (idRelationshipType) REFERENCES relationshipTypes(idRelationshipType)\n"
				");");

		CLog::Log(LOGINFO, "create artwork table");
		m_pDS->exec("CREATE TABLE IF NOT EXISTS artwork (\n"
				"  idArtwork          INTEGER       NOT NULL  PRIMARY KEY AUTOINCREMENT ,\n"
				"  idArtworkType      INTEGER       NOT NULL ,\n"
				"  idObject           INTEGER       NOT NULL ,\n"
				"  width              INTEGER       NOT NULL ,\n"
				"  height             INTEGER       NOT NULL ,\n"
				"  lastUpdated  VARCHAR(24)  NULL      DEFAULT NULL,\n"
				"  url                TEXT ,\n"
				"  thumb              BLOB, -- cached image content\n"
				"\n"
				"  FOREIGN KEY (idArtworkType) REFERENCES artworkTypes(idArtworkType),\n"
				"  FOREIGN KEY (idObject) REFERENCES objects(idObject)\n"
				");");

		CLog::Log(LOGINFO, "create queries table");
		m_pDS->exec("CREATE  TABLE IF NOT EXISTS queries (\n"
				"  idQuery     INTEGER     NOT NULL  PRIMARY KEY AUTOINCREMENT ,\n"
				"  stub        TEXT        NOT NULL ,\n"
				"  locked      INTEGER     NOT NULL  DEFAULT 0 ,\n"
				"  query       TEXT        NOT NULL\n"
				");");

		CLog::Log(LOGINFO, "create dirents table");
		m_pDS->exec("CREATE  TABLE IF NOT EXISTS dirents (\n"
				"  idDirent     INTEGER       NOT NULL  PRIMARY KEY  AUTOINCREMENT,\n"
				"  idPath       INTEGER       NOT NULL ,\n"
				"  filename     TEXT          NULL ,\n"
				"  url          TEXT          NULL ,\n"
				"  fileLength   INTEGER       NOT NULL  DEFAULT 0 ,\n"
				"  fileHash     VARCHAR(32)   NULL ,\n"
				"  contentHash  VARCHAR(32)   NULL ,\n"
				"  settings     TEXT          NULL ,\n"
				"  streams      TEXT          NULL ,\n"
				"  dateAdded    TEXT          NULL,\n"
				"\n"
				"  FOREIGN KEY (idPath) REFERENCES paths(idPath)\n"
				");");
		m_pDS->exec("CREATE INDEX ixDPath ON dirents(idPath);");
		m_pDS->exec("CREATE INDEX ixDPathFilename ON dirents(idPath ASC, filename ASC);");

		CLog::Log(LOGINFO, "create objectlinkdirent table");
		m_pDS->exec("CREATE  TABLE IF NOT EXISTS objectlinkdirent (\n"
				"  idDirent  INTEGER  NOT NULL ,\n"
				"  idObject  INTEGER  NOT NULL ,\n"
				"\n"
				"  FOREIGN KEY (idDirent) REFERENCES dirents(idDirent) ,\n"
				"  FOREIGN KEY (idObject) REFERENCES objects(idObject)\n"
				");");
		m_pDS->exec("CREATE INDEX ixOLDDirent ON objectlinkdirent(idDirent)");
		m_pDS->exec("CREATE INDEX ixOLDObject ON objectlinkdirent(idObject)");

		CLog::Log(LOGINFO, "create path table");
		m_pDS->exec("CREATE  TABLE IF NOT EXISTS paths (\n"
				"  idPath          INTEGER      NOT NULL  PRIMARY KEY   AUTOINCREMENT ,\n"
				"  idParent        INTEGER      NOT NULL  DEFAULT 0 ,\n"
				"  path            TEXT         NULL      DEFAULT NULL ,\n"
				"  hash            VARCHAR(32)  NULL      DEFAULT NULL ,\n"
				"  noUpdate        INTEGER      NULL      DEFAULT NULL\n"
				");");
		CLog::Log(LOGINFO, "create scrapers table");
		m_pDS->exec("CREATE  TABLE IF NOT EXISTS scrapers (\n"
				"  idScraper       INTEGER       NOT NULL  PRIMARY KEY   AUTOINCREMENT ,\n"
				"  content         TEXT          NULL      DEFAULT NULL ,\n"
				"  scraper         TEXT          NULL      DEFAULT NULL ,\n"
				"  scanRecursive   INTEGER       NULL      DEFAULT NULL ,\n"
				"  useFolderNames  INTEGER       NULL      DEFAULT NULL ,\n"
				"  settings        TEXT          NULL      DEFAULT NULL ,\n"
				"  noUpdate        INTEGER       NULL      DEFAULT NULL\n"
				");");


		CLog::Log(LOGINFO, "create pathlinkscraper table");
		m_pDS->exec("CREATE TABLE IF NOT EXISTS pathlinkscraper (\n"
				"  idPath  INTEGER  NOT NULL ,\n"
				"  idScraper  INTEGER  NOT NULL ,\n"
				"\n"
				"  FOREIGN KEY (idPath) REFERENCES paths(idPath) ,\n"
				"  FOREIGN KEY (idScraper) REFERENCES scrapers(idScraper)\n"
				");");
		m_pDS->exec("CREATE INDEX ixPLSpath ON pathlinkscraper(idPath);");
		m_pDS->exec("CREATE INDEX ixPLSscraper ON pathlinkscraper(idScraper);");

		CLog::Log(LOGINFO, "create profile table");
		m_pDS->exec("CREATE  TABLE IF NOT EXISTS profile (\n"
				"  idProfile  INTEGER  NOT NULL  PRIMARY KEY AUTOINCREMENT ,\n"
				"  name       TEXT     NOT NULL\n"
				");");

		CLog::Log(LOGINFO, "create bookmark table");
		m_pDS->exec("CREATE  TABLE IF NOT EXISTS bookmark (\n"
				"  idBookmark          INTEGER NOT NULL  PRIMARY KEY   AUTOINCREMENT ,\n"
				"  idDirent            INTEGER NOT NULL ,\n"
				"  idProfile           INTEGER NOT NULL ,\n"
				"  timeInSeconds       DOUBLE  NULL      DEFAULT NULL ,\n"
				"  totalTimeInSeconds  DOUBLE  NULL      DEFAULT NULL ,\n"
				"  thumbnailImage      TEXT    NULL      DEFAULT NULL ,\n"
				"  player              TEXT    NULL      DEFAULT NULL ,\n"
				"  playerState         TEXT    NULL      DEFAULT NULL ,\n"
				"  type                INTEGER NULL      DEFAULT NULL ,\n"
				"\n"
				"  FOREIGN KEY (idDirent) REFERENCES dirents(idDirent) ,\n"
				"  FOREIGN KEY (idProfile) REFERENCES profiles(idProfile)\n"
				");");
		m_pDS->exec("CREATE INDEX ixBDirent ON bookmark(idDirent);");
		m_pDS->exec("CREATE INDEX ixBProfile ON bookmark(idProfile);");

		CLog::Log(LOGINFO, "create settings table");
		m_pDS->exec("CREATE  TABLE IF NOT EXISTS settings (\n"
				"  idProfile   INTEGER      NOT NULL ,\n"
				"  idObject    INTEGER      NOT NULL ,\n"
				"  playCount   INTEGER      NULL      DEFAULT NULL ,\n"
				"  lastPlayed  VARCHAR(24)  NULL      DEFAULT NULL\n"
				");");
		m_pDS->exec("CREATE INDEX ixSProfileObject ON settings (idProfile ASC, idObject ASC);");
		m_pDS->exec("CREATE INDEX ixSObjectProfile ON settings (idObject ASC, idProfile ASC);");

		CLog::Log(LOGINFO, "create stacktimes table");
		m_pDS->exec("CREATE  TABLE IF NOT EXISTS stacktimes (\n"
				"  idDirent  INTEGER  NULL  DEFAULT NULL ,\n"
				"  times     TEXT     NULL  DEFAULT NULL ,\n"
				"\n"
				"  FOREIGN KEY (idDirent) REFERENCES dirents(idDirent)\n"
				");");
		m_pDS->exec("CREATE INDEX ixSTDirent ON stacktimes (idDirent ASC);");

		//CLog::Log(LOGINFO, "create version table");
		//m_pDS->exec("CREATE  TABLE IF NOT EXISTS version (\n"
		//		"  contentRevision       INTEGER  NULL  DEFAULT NULL ,\n"
		//		"  contentCompressCount  INTEGER  NULL  DEFAULT NULL\n"
		//		");");

		CreateViews();
		InsertDefaults();
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s unable to create tables", __FUNCTION__);
		RollbackTransaction();
		return false;
	}
	CommitTransaction();
	return true;
}

void CObjectDatabase::CreateViews()
{
	CLog::Log(LOGINFO, "create relationship view");
	m_pDS->exec("CREATE VIEW viewRelationshipsAll\n"
			"AS\n"
			"SELECT  r.idRelationship   AS rID,\n"
			"		 o1.idObject        AS o1ID,\n"
			"        o1.stub            AS o1Stub,\n"
			"        IFNULL(o1.name,\n"
			"               o1.stub)    AS o1Name,\n"
			"        o1.idObjectType    AS o1TypeID,\n"
			"        ot1.name           AS o1TypeName ,\n"
			"        o2.idObject        AS o2ID,\n"
			"        o2.stub            AS o2Stub,\n"
			"        IFNULL(o2.name,\n"
			"               o2.stub)    AS o2Name,\n"
			"        o2.idObjectType    AS o2TypeID,\n"
			"        ot2.name           AS o2TypeName,\n"
			"        rt.stub            AS rtName,\n"
			"        rt.idRelationshipType AS rtID,\n"
			"	     r.link             AS link,\n"
			"        r.sequenceIndex    AS seqIndex\n"
			"FROM    objects o1\n"
			"INNER JOIN\n"
			"        objectTypes ot1\n"
			"ON      o1.idObjectType=ot1.idObjectType\n"
			"INNER JOIN\n"
			"        relationships r\n"
			"ON      o1.idObject=r.idObject1\n"
			"INNER JOIN\n"
			"        objects o2\n"
			"ON      o2.idObject=r.idObject2\n"
			"INNER JOIN\n"
			"        objectTypes ot2\n"
			"ON      o2.idObjectType=ot2.idObjectType\n"
			"INNER JOIN\n"
			"        relationshipTypes rt\n"
			"ON      r.idRelationshipType=rt.idRelationshipType;");

	CLog::Log(LOGINFO, "create objectdirent view");
	m_pDS->exec("CREATE VIEW viewObjectDirentAll\n"
			"AS \n"
			"SELECT DISTINCT o.idObject         AS oID,\n"
			"       o.stub             AS oStub,\n"
			"       IFNULL(o.name,\n"
			"               o.stub)    AS oName,\n"
			"       o.idObjectType     as oType,\n"
			"       p.idPath           AS pID,\n"
			"       p.path             AS pPath,\n"
			"		p.idParent         AS pParent,\n"
			"       p.noUpdate         AS pNoUpdate,\n"
			"       d.idDirent         AS dID,\n"
			"       d.filename	  AS dFileName,\n"
			"       IFNULL(d.filename,\n"
			"               d.url)     AS dUrl,\n"
			"		d.dateAdded        AS dateAdded,\n"
			"       d.settings         AS dSettings,\n"
			"		d.streams          AS dStreams\n"
			"FROM\n"
			"       objects o\n"
			"INNER JOIN\n"
			"       objectlinkdirent old\n"
			"ON     \n"
			"       o.idObject=old.idObject\n"
			"INNER JOIN\n"
			"       dirents d\n"
			"ON\n"
			"       old.idDirent=d.idDirent\n"
			"INNER JOIN\n"
			"       paths p\n"
			"ON\n"
			"       d.idPath=p.idPath;");

	CLog::Log(LOGINFO, "creating attribute view");
	m_pDS->exec("CREATE VIEW viewObjectAttributes\n"
			"AS\n"
			"SELECT o.idObject                  AS oID,\n"
			"       o.name                      AS oName,\n"
			"       ot.idObjectType             AS otID,\n"
			"       IFNULL(ot.stub,\n"
			"				ot.name)            AS otName,\n"
			"       at.idAttributeType          AS atID,\n"
			"       a.valueString               AS aString,\n"
			"       a.valueNumber               AS aNumber,\n"
			"       a.ValueBlob                 AS aBlob,\n"
			"       IFNULL(at.name,\n"
			"               at.stub)            AS atName,\n"
			"       at.dataType                 AS atDataType,\n"
			"       at.dataPrecision            AS atPrecision\n"
			"FROM\n"
			"       attributes a\n"
			"INNER JOIN\n"
			"       attributeTypes at\n"
			"ON\n"
			"       a.idAttributeType=at.idAttributeType\n"
			"INNER JOIN\n"
			"       objects o\n"
			"ON\n"
			"       a.idObject=o.idObject\n"
			"INNER JOIN\n"
			"      objectTypes ot\n"
			"ON\n"
			"      o.idObjectType=ot.idObjectType;");
}

void CObjectDatabase::InsertDefaults()
{
	CStdString sql;
	CLog::Log(LOGINFO, "inserting object types");
	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
			"VALUES (%i, %i, '%s', '%s')", OBJ_OBJECT, 0, "object", "Object");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
			"VALUES (%i, %i, '%s', '%s')", OBJ_CONTENT, OBJ_OBJECT, "content", "Content");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
			"VALUES (%i, %i, '%s', '%s')", OBJ_VIDEO, OBJ_CONTENT, "video", "Videos");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
			"VALUES (%i, %i, '%s', '%s')", OBJ_MOVIE, OBJ_VIDEO, "movie", "Movies");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
			"VALUES (%i, %i, '%s', '%s')", OBJ_TVSHOW, OBJ_VIDEO, "tvshow", "TV Shows");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
			"VALUES (%i, %i, '%s', '%s')", OBJ_MUSICVIDEO, OBJ_VIDEO, "musicvideo", "Music Videos");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
			"VALUES (%i, %i, '%s', '%s')", OBJ_EPISODE, OBJ_VIDEO, "episode", "Episodes");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
			"VALUES (%i, %i, '%s', '%s')", OBJ_SONG, OBJ_CONTENT, "song", "Songs");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
			"VALUES (%i, %i, '%s', '%s')", OBJ_PICTURE, OBJ_OBJECT, "picture", "Pictures");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
			"VALUES (%i, %i, '%s', '%s')", OBJ_ORGANISATION, OBJ_OBJECT, "organisation", "Organisation");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
			"VALUES (%i, %i, '%s', '%s')", OBJ_PERSON, OBJ_ORGANISATION, "person", "People");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
			"VALUES (%i, %i, '%s', '%s')", OBJ_ACTOR, OBJ_PERSON, "actor", "Actors");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
				"VALUES (%i, %i, '%s', '%s')", OBJ_DIRECTOR, OBJ_PERSON, "director", "Directors");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
			"VALUES (%i, %i, '%s', '%s')", OBJ_WRITER, OBJ_PERSON, "writer", "Writers");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
			"VALUES (%i, %i, '%s', '%s')", OBJ_MUSICIAN, OBJ_PERSON, "musician", "Musicians");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
			"VALUES (%i, %i, '%s', '%s')", OBJ_BAND, OBJ_ORGANISATION, "band", "Bands");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
			"VALUES (%i, %i, '%s', '%s')", OBJ_STUDIO, OBJ_ORGANISATION, "studio", "Studios");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
			"VALUES (%i, %i, '%s', '%s')", OBJ_COUNTRY, OBJ_ORGANISATION, "country", "Countries");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
			"VALUES (%i, %i, '%s', '%s')", OBJ_GROUPING, OBJ_OBJECT, "grouping", "Grouping");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
			"VALUES (%i, %i, '%s', '%s')", OBJ_MOVIESET, OBJ_GROUPING, "movieset", "Movie Sets");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
			"VALUES (%i, %i, '%s', '%s')", OBJ_SEASON, OBJ_GROUPING, "season", "Seasons");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
			"VALUES (%i, %i, '%s', '%s')", OBJ_GENRE, OBJ_GROUPING, "genre", "Genres");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
			"VALUES (%i, %i, '%s', '%s')", OBJ_YEAR, OBJ_GROUPING, "year", "Years");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
			"VALUES (%i, %i, '%s', '%s')", OBJ_ALBUM, OBJ_GROUPING, "album", "Albums");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
			"VALUES (%i, %i, '%s', '%s')", OBJ_PLAYLIST, OBJ_GROUPING, "playlist", "Playlists");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
			"VALUES (%i, %i, '%s', '%s')", OBJ_TAG, OBJ_GROUPING, "tag", "Tags");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
			"VALUES (%i, %i, '%s', '%s')", OBJ_ADDON, OBJ_OBJECT, "addon", "Addons");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO objectTypes (idObjectType, idParentObjectType, stub, name) "
			"VALUES (%i, %i, '%s', '%s')", OBJ_REPO, OBJ_OBJECT, "repo", "Addon Repository");
	m_pDS->exec(sql.c_str());

	CLog::Log(LOGINFO, "inserting attribute types");
	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", FILENAME_STR, OBJ_CONTENT, "filename",
			"Filename", STRING_ATTRIBUTE, 0, 1);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", RELEASEDATE_STR, OBJ_CONTENT, "released",
			"Release Date", STRING_ATTRIBUTE, 0, 1);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", USERRATING_NUM, OBJ_OBJECT, "rating",
			"Rating", NUMBER_ATTRIBUTE, -1, 1);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", ONLINEID_STR, OBJ_OBJECT, "onlineid",
			"Online ID", STRING_ATTRIBUTE, 0, 1);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", CONTENT_SORTTITLE_STR, OBJ_CONTENT, "sorttitle",
			"Sort Title", STRING_ATTRIBUTE, 0, 1);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", CONTENT_ORIGINALTITLE_STR, OBJ_CONTENT, "originaltitle",
			"Original Title", STRING_ATTRIBUTE, 0, 1);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", VIDEO_SUMMARY_STR, OBJ_VIDEO, "summary",
			"Summary", STRING_ATTRIBUTE, 0, 1);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", VOTES_STR, OBJ_VIDEO, "votes",
			"Online Rating Votes", STRING_ATTRIBUTE, 0, 1);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", CONTENTRATING_STR, OBJ_VIDEO, "contentrating",
			"Content Rating", STRING_ATTRIBUTE, 0, 1);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", ONLINERATING_NUM, OBJ_VIDEO, "onlinerating",
			"Online Rating", NUMBER_ATTRIBUTE, -1, 1);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", TAGLINE_STR, OBJ_MOVIE, "tagline",
			"Tagline", STRING_ATTRIBUTE, 0, 0);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", MOVIE_PLOT_STR, OBJ_MOVIE, "plot",
			"Plot", STRING_ATTRIBUTE, 0, 0);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", MOVIE_PLOTOUTLINE_STR, OBJ_MOVIE, "plotoutline",
			"Plot Outline", STRING_ATTRIBUTE, 0, 0);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", MOVIE_RANKING_NUM, OBJ_MOVIE, "movierank",
			"Movie Rank", NUMBER_ATTRIBUTE, 0, 0);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", MOVIE_TRAILER_URL_STR, OBJ_MOVIE, "trailer",
			"Trailer", STRING_ATTRIBUTE, 0, 0);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", TVSHOW_EPISODEGUIDE_STR, OBJ_TVSHOW, "episodeguide",
			"Episode Guide", STRING_ATTRIBUTE, 0, 0);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", TVSHOW_STATUS_STR, OBJ_TVSHOW, "status",
			"Status", STRING_ATTRIBUTE, 0, 0);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", EPISODE_PLOT_STR, OBJ_EPISODE, "plot",
			"Plot", STRING_ATTRIBUTE, 0, 0);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", EPISODE_PRODUCTIONCODE_STR, OBJ_EPISODE, "productioncode",
			"Production Code", STRING_ATTRIBUTE, 0, 0);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", EPISODE_SEASONSORT_NUM, OBJ_EPISODE, "seasonsort",
			"Season Sort", NUMBER_ATTRIBUTE, 0, 0);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", EPISODE_EPISODESORT_NUM, OBJ_EPISODE, "episodesort",
			"Episode Sort", NUMBER_ATTRIBUTE, 0, 0);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", HEIGHT_NUM, OBJ_PICTURE, "height",
			"Height", NUMBER_ATTRIBUTE, 0, 1);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", WIDTH_NUM, OBJ_PICTURE, "width",
			"Width", NUMBER_ATTRIBUTE, 0, 1);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
				"name, dataType, dataPrecision, inheritable) "
				"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", BIOGRAPHY_STR, OBJ_ORGANISATION, "bio",
				"Biography", STRING_ATTRIBUTE, 0, 1);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
					"name, dataType, dataPrecision, inheritable) "
					"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", DATE_OF_BIRTH_STR, OBJ_PERSON, "dob",
					"Date of Birth", STRING_ATTRIBUTE, 0, 1);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
				"name, dataType, dataPrecision, inheritable) "
				"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", GROUP_DESCRIPTION_STR, OBJ_GROUPING, "description",
				"Description", STRING_ATTRIBUTE, 0, 1);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", ADDON_TYPE_STR, OBJ_ADDON, "type",
			"Type", STRING_ATTRIBUTE, 0, 0);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", ADDON_SUMMARY_STR, OBJ_ADDON, "summary",
			"Summary", STRING_ATTRIBUTE, 0, 0);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", ADDON_DESCRIPTION_STR, OBJ_ADDON, "description",
			"Description", STRING_ATTRIBUTE, 0, 0);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", ADDON_RATING_NUM, OBJ_ADDON, "rating",
			"Rating", NUMBER_ATTRIBUTE, -1, 0);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", ADDON_ID_STR, OBJ_ADDON, "addonid",
			"Addon ID", STRING_ATTRIBUTE, 0, 0);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", ADDON_VERSION_STR, OBJ_ADDON, "version",
			"Version", STRING_ATTRIBUTE, 0, 0);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", ADDON_AUTHOR_STR, OBJ_ADDON, "author",
			"Author", STRING_ATTRIBUTE, 0, 0);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO attributeTypes (idAttributeType, idObjectType, stub, "
			"name, dataType, dataPrecision, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i, %i, %i)", REPO_ID_STR, OBJ_REPO, "addonid",
			"Addon ID", STRING_ATTRIBUTE, 0, 0);
	m_pDS->exec(sql.c_str());

	CLog::Log(LOGINFO, "inserting relationship types");
	sql = PrepareSQL("INSERT INTO relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub, inheritableType1) "
			"VALUES (%i,%i, %i,'%s', %i)", OBJECT_HAS_TAG, OBJ_OBJECT, OBJ_TAG, "object_has_tag", 1);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub, inheritableType1) "
				"VALUES (%i,%i, %i,'%s', %i)", CONTENT_HAS_YEAR, OBJ_CONTENT, OBJ_YEAR, "content_has_year", 1);
		m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub, inheritableType1)"
			" VALUES (%i, %i, %i,'%s', 1)", VIDEO_HAS_DIRECTOR, OBJ_VIDEO, OBJ_DIRECTOR, "video_has_director");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub, inheritableType1)"
			" VALUES (%i, %i, %i,'%s', 1)", VIDEO_HAS_STUDIO, OBJ_VIDEO, OBJ_STUDIO, "video_has_studio");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub, inheritableType1)"
				" VALUES (%i, %i, %i,'%s', 1)", VIDEO_HAS_GENRE, OBJ_VIDEO, OBJ_GENRE, "video_has_genre");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub)"
			" VALUES (%i, %i, %i,'%s')", MOVIE_HAS_ACTOR, OBJ_MOVIE, OBJ_ACTOR, "movie_has_actor");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub)"
			" VALUES (%i, %i, %i,'%s')", MOVIE_HAS_WRITER, OBJ_MOVIE, OBJ_WRITER, "movie_has_writer");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub)"
				" VALUES (%i, %i, %i,'%s')", MOVIE_HAS_COUNTRY, OBJ_MOVIE, OBJ_COUNTRY, "movie_has_country");
		m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub)"
				" VALUES (%i, %i, %i,'%s')", MOVIE_LINK_TVSHOW, OBJ_MOVIE, OBJ_TVSHOW, "movie_link_tvshow");
		m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub)"
			" VALUES (%i, %i, %i,'%s')", TVSHOW_HAS_ACTOR, OBJ_TVSHOW, OBJ_ACTOR, "tvshow_has_actor");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub, sequenced)"
			" VALUES (%i, %i, %i,'%s', 1)", TVSHOW_HAS_SEASON, OBJ_TVSHOW, OBJ_SEASON, "tvshow_has_season");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub)"
			" VALUES (%i, %i, %i,'%s')", EPISODE_HAS_ACTOR, OBJ_EPISODE, OBJ_ACTOR, "episode_has_actor");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub)"
				" VALUES (%i, %i, %i,'%s')", EPISODE_HAS_WRITER, OBJ_EPISODE, OBJ_WRITER, "episode_has_writer");
		m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub)"
			" VALUES (%i, %i, %i,'%s')", MUSICVIDEO_HAS_MUSICIAN, OBJ_MUSICVIDEO, OBJ_MUSICIAN, "musicvideo_has_musician");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub)"
			" VALUES (%i, %i, %i,'%s')", MUSICVIDEO_HAS_BAND, OBJ_MUSICVIDEO, OBJ_BAND, "musicvideo_has_band");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub)"
			" VALUES (%i, %i, %i,'%s')", MUSICIAN_HAS_SONG, OBJ_MUSICIAN, OBJ_SONG, "musician_has_song");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub)"
			" VALUES (%i, %i, %i,'%s')", MUSICIAN_HAS_ALBUM, OBJ_MUSICIAN, OBJ_ALBUM, "musician_has_album");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub)"
			" VALUES (%i, %i, %i,'%s')", BAND_HAS_SONG, OBJ_BAND, OBJ_SONG, "band_has_song");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub)"
			" VALUES (%i, %i, %i,'%s')", BAND_HAS_ALBUM, OBJ_BAND, OBJ_ALBUM, "band_has_album");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub)"
			" VALUES (%i, %i, %i,'%s')", BAND_HAS_MUSICIAN, OBJ_BAND, OBJ_MUSICIAN, "band_has_musician");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub, sequenced)"
			" VALUES (%i, %i, %i,'%s', 1)", SEASON_HAS_EPISODE, OBJ_SEASON, OBJ_EPISODE, "season_has_episode");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub)"
			" VALUES (%i, %i, %i,'%s')", MOVIESET_HAS_MOVIE, OBJ_MOVIESET, OBJ_MOVIE, "movieset_has_movie");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub)"
			" VALUES (%i, %i, %i,'%s')", ALBUM_HAS_STUDIO, OBJ_ALBUM, OBJ_STUDIO, "album_has_studio");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub, sequenced)"
			" VALUES (%i, %i, %i,'%s', 1)", ALBUM_HAS_SONG, OBJ_ALBUM, OBJ_SONG, "album_has_song");
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub, sequenced)"
				" VALUES (%i, %i, %i,'%s', 1)", ALBUM_HAS_MUSICVIDEO, OBJ_ALBUM, OBJ_MUSICVIDEO, "album_has_musicvideo");
		m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub, sequenced)"
				" VALUES (%i, %i, %i,'%s', 1)", ALBUM_HAS_GENRE, OBJ_ALBUM, OBJ_GENRE, "album_has_genre");
		m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub)"
			" VALUES (%i, %i, %i,'%s')", REPO_HAS_ADDON, OBJ_REPO, OBJ_ADDON, "repo_has_addon");
	m_pDS->exec(sql.c_str());

	CLog::Log(LOGINFO, "inserting artwork types");
	sql = PrepareSQL("INSERT INTO artworkTypes (idArtworkType, idObjectType, stub, name, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i)", THUMBNAIL, OBJ_OBJECT, "thumb", "Thumbnail", 1);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO artworkTypes (idArtworkType, idObjectType, stub, name, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i)", FANART, OBJ_OBJECT, "fanart", "Fanart", 1);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO artworkTypes (idArtworkType, idObjectType, stub, name, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i)", BANNER, OBJ_VIDEO, "banner", "Banner", 1);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO artworkTypes (idArtworkType, idObjectType, stub, name, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i)", LANDSCAPE, OBJ_VIDEO, "landscape", "landscape", 1);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO artworkTypes (idArtworkType, idObjectType, stub, name, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i)", CLEARLOGO, OBJ_OBJECT, "clearlogo", "ClearLogo", 1);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO artworkTypes (idArtworkType, idObjectType, stub, name, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i)", CLEARART, OBJ_VIDEO, "clearart", "ClearArt", 1);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO artworkTypes (idArtworkType, idObjectType, stub, name, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i)", DISCART, OBJ_MOVIE, "discart", "Disc Art", 1);
	m_pDS->exec(sql.c_str());

	sql = PrepareSQL("INSERT INTO artworkTypes (idArtworkType, idObjectType, stub, name, inheritable) "
			"VALUES (%i, %i, '%s', '%s', %i)", CDART, OBJ_ALBUM, "cdart", "CDArt", 1);
	m_pDS->exec(sql.c_str());
}

int CObjectDatabase::RunQuery(const CStdString &sql)
{
	return RunQuery(m_pDS, sql);
}

int CObjectDatabase::RunQuery(auto_ptr<Dataset> &pDS, const CStdString &sql)
{
  //unsigned int time = XbmcThreads::SystemClockMillis();
  int rows = -1;
  if (pDS->query(sql.c_str()))
  {
    rows = pDS->num_rows();
    if (rows == 0)
      pDS->close();
  }
  CLog::Log(LOGDEBUG, "%s took %d ms for %d items query: %s", __FUNCTION__, 0, rows, sql.c_str());
  return rows;
}

void CObjectDatabase::GetDataSet(std::auto_ptr<dbiplus::Dataset> &pDS)
{
	pDS.reset(m_pDB->CreateDataset());
}

void CObjectDatabase::ConstructPath(CStdString& strDest, const CStdString& strPath, const CStdString& strFileName)
{
  if (URIUtils::IsStack(strFileName) ||
      URIUtils::IsInArchive(strFileName) || URIUtils::IsPlugin(strPath))
    strDest = strFileName;
  else
    strDest = URIUtils::AddFileToFolder(strPath, strFileName);
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

bool CObjectDatabase::GetFilePathById(const int idObject, CStdString& path)
{
	CStdString sql;
	try
	{
		if (NULL == m_pDB.get()) return false;
		if (NULL == m_pDS.get()) return false;

		sql = PrepareSQL("SELECT pPath, dFilename FROM viewObjectDirentAll WHERE oID=%i", idObject);



		if(!m_pDS->eof())
		{
			CStdString strFile = m_pDS->fv("dFilename").get_asString();
			CStdString strPath=m_pDS->fv("pPath").get_asString();

			ConstructPath(path, strPath, strFile);

		}
		m_pDS->close();
		return true;

	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s failed (%s)", __FUNCTION__, sql.c_str());
	}

	return false;
}

bool CObjectDatabase::GetPaths(set<CStdString> &paths, int idObjectType)
{
	CStdString sql;
	try
	  {
	    if (NULL == m_pDB.get()) return false;
	    if (NULL == m_pDS.get()) return false;

	    paths.clear();
	    vector<int> ids;
	    GetAllDescendentObjectTypes(idObjectType, ids);

	    sql = PrepareSQL("SELECT pPath, pNoUpdate FROM viewObjectDirentAll WHERE pPath NOT like 'multipath://%%'"
	    		" AND pID NOT IN (select idPath from dirents where filename like 'video_ts.ifo')" //omit dvd folders
	    		" AND pID NOT IN (select idPath from dirents where filename like 'index.bdmv')" //omit blu-ray folders
	    		" AND oType IN (%s)"
	    		" ORDER BY pPath", StringUtils::Join(ids, ",").c_str());

	    if(!m_pDS->query(sql.c_str()))
	    	return false;

	    while(!m_pDS->eof())
	    {
	    	if (!m_pDS->fv("pNoUpdate").get_asBool())
	    		paths.insert(m_pDS->fv("pPath").get_asString());
	    	m_pDS->next();
	    }
	    m_pDS->close();
	    return true;

	  }
	catch (...)
	{
		CLog::Log(LOGERROR, "%s failed (%s)", __FUNCTION__, sql.c_str());
	}

	return false;
}

bool CObjectDatabase::GetSubPaths(const CStdString &basepath, vector< pair<int,string> >& subpaths)
{
  CStdString sql;
  try
  {
    if (!m_pDB.get() || !m_pDS.get())
      return false;

    CStdString path(basepath);
    URIUtils::AddSlashAtEnd(path);
    sql = PrepareSQL("SELECT idPath,path FROM paths WHERE SUBSTR(path,1,%i)='%s'", StringUtils::utf8_strlen(path.c_str()), path.c_str());
    m_pDS->query(sql.c_str());
    while (!m_pDS->eof())
    {
      subpaths.push_back(make_pair(m_pDS->fv(0).get_asInt(), m_pDS->fv(1).get_asString()));
      m_pDS->next();
    }
    m_pDS->close();
    return true;
  }
  catch (...)
  {
    CLog::Log(LOGERROR, "%s error during query: %s",__FUNCTION__, sql.c_str());
  }
  return false;
}

bool CObjectDatabase::GetPathsForObject(const int idObject, set<int>& paths)
{
	CStdString strSQL;
	  try
	  {
	    if (NULL == m_pDB.get()) return false;
	    if (NULL == m_pDS.get()) return false;
	    strSQL = PrepareSQL("SELECT DISTINCT pID FROM viewObjectDirentAll WHERE oID=%i",idObject);
	    m_pDS->query(strSQL.c_str());
	    while (!m_pDS->eof())
	    {
	      paths.insert(m_pDS->fv(0).get_asInt());
	      m_pDS->next();
	    }
	    m_pDS->close();
	    return true;
	  }
	  catch (...)
	  {
	    CLog::Log(LOGERROR, "%s error during query: %s",__FUNCTION__, strSQL.c_str());
	  }
	  return false;
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

void CObjectDatabase::InvalidatePathHash(const CStdString& strPath)
{
	SetPathHash(strPath, "");
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

void CObjectDatabase::ExcludePathFromScraping(const int idPath)
{
	try
	{

		if (NULL == m_pDB.get()) return;
		if (NULL == m_pDS.get()) return;
		if( idPath < 0 ) return;

		m_pDS->exec(PrepareSQL("update paths set noUpdate=0 where idPath=%i", idPath).c_str());
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s unable to excludepath (%i)", __FUNCTION__, idPath);
	}
}

int CObjectDatabase::AddScraper(const CStdString& scraper, const CStdString& content )
{
	return AddScraper(scraper, content, "", 0, false, false);
}

int CObjectDatabase::AddScraper(const CStdString& scraper, const CStdString& content, const CStdString& settings, int recurse, bool useFolderNames, bool noUpdate)
{
	CStdString strSQL;
	try
	{

		if (NULL == m_pDB.get()) return -1;
		if (NULL == m_pDS.get()) return -1;


		int idScraper = GetScraperId(scraper);
		if (idScraper >= 0)
		{
			strSQL=PrepareSQL("update scrapers set content='%s', scanRecursive=%i, useFolderNames=%i, settings='%s', noUpdate=%i where idScraper=%i",
					content.c_str(), recurse, useFolderNames ? 1 : 0, settings.c_str(), noUpdate ? 1 : 0, idScraper);
			m_pDS->exec(strSQL.c_str());
		}
		else
		{
			strSQL=PrepareSQL("insert into scrapers (scraper, content, scanRecursive, useFolderNames, settings, noUpdate) values ('%s','%s', %i, %i, '%s', %i)", scraper.c_str(), content.c_str(), recurse, useFolderNames ? 1 : 0,
					settings.c_str(), noUpdate ? 1 : 0);
			m_pDS->exec(strSQL.c_str());
			idScraper = (int)m_pDS->lastinsertid();
		}
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

bool CObjectDatabase::LinkScraperToPath(int idScraper, int idPath)
{
	CStdString strSQL;
	try
	{
		if (NULL == m_pDB.get()) return -1;
		if (NULL == m_pDS.get()) return -1;

		if(idPath < 0)
			return false;

		if(idScraper > 0)
		{
			strSQL=PrepareSQL("INSERT INTO pathlinkscraper (idPath, idScraper) VALUES (%i, %i)",idPath,idScraper);

		}
		else
		{
			strSQL=PrepareSQL("DELETE FROM pathlinkscraper WHERE idPath=%i", idPath, idScraper);
		}
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

bool CObjectDatabase::GetScraperForPath(const int idPath, CStdString& strScraper, CStdString& content, int& scanRecursive, bool& useFolderNames, CStdString& settings, bool& noUpdate, bool& exclude)
{
	CStdString strSQL;
	try
	{
		if (NULL == m_pDB.get()) return false;
		if (NULL == m_pDS.get()) return false;
		if (idPath < 0) return false;

		strSQL=PrepareSQL("select s.scraper, s.content, s.scanRecursive, s.useFolderNames, s.settings, s.noUpdate, p.noUpdate"
				" from paths p join pathlinkscraper ps on p.idPath = ps.idPath join scrapers s on ps.idScraper=s.idScraper where p.idPath=%i", idPath);
		CLog::Log(LOGDEBUG, "%s, fetching scraper (%s)",__FUNCTION__, strSQL.c_str());
		m_pDS->query(strSQL.c_str());
		if (m_pDS->num_rows() > 0)
		{
			strScraper = m_pDS->fv("s.scraper").get_asString();
			content = m_pDS->fv("s.content").get_asString();
			scanRecursive = m_pDS->fv("s.scanRecursive").get_asBool();
			useFolderNames = m_pDS->fv("s.useFolderNames").get_asBool();
			settings = m_pDS->fv("s.settings").get_asString();
			noUpdate = m_pDS->fv("s.noUpdate").get_asBool();
			exclude = m_pDS->fv("p.noUpdate").get_asBool();

			m_pDS->close();

			CLog::Log(LOGDEBUG, "%s, returning scraper...",__FUNCTION__);
			return true;
		}

		CLog::Log(LOGDEBUG, "%s, returning false...",__FUNCTION__);

	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s unable to getscraper (%s)", __FUNCTION__, strSQL.c_str());
	}
	return false;
}

bool CObjectDatabase::ScraperInUse(const int idScraper)
{
	try
	{
		if (NULL == m_pDB.get()) return false;
		if (NULL == m_pDS.get()) return false;

		CStdString sql = PrepareSQL("select count(1) from pathlinkscraper where idScraper=%i", idScraper);
		if (!m_pDS->query(sql.c_str()) || m_pDS->num_rows() == 0)
			return false;
		bool found = m_pDS->fv(0).get_asInt() > 0;
		m_pDS->close();
		return found;
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s(%i) failed", __FUNCTION__, idScraper);
	}
	return false;
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

		strSQL=PrepareSQL("insert into dirents (idDirent, idPath, filename, dateAdded) values(NULL, %i, '%s', '%s')", idPath, strFileName.c_str(), CDateTime::GetCurrentDateTime().GetAsDBDateTime().c_str());
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

int CObjectDatabase::GetFileId(const CStdString& strFileNameAndPath)
{
	CStdString strSQL = "";
	try
	{
		int idDirent;
		if (NULL == m_pDB.get()) return -1;
		if (NULL == m_pDS.get()) return -1;

		CStdString strFileName, strPath;
		SplitPath(strFileNameAndPath,strPath,strFileName);

		int idPath = GetPathId(strPath);
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

	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s unable to adddirent (%s)", __FUNCTION__, strSQL.c_str());
	}
	return -1;
}

void CObjectDatabase::UpdateDirentDateAdded(const int idDirent, CDateTime dateAdded)
{
	if (idDirent < 0)
		return;

	if (!dateAdded.IsValid())
	      dateAdded = CDateTime::GetCurrentDateTime();

	CStdString strSQL = "";
	try
	{
		if (NULL == m_pDB.get()) return;
		if (NULL == m_pDS.get()) return;


		strSQL = PrepareSQL("update dirents set dateAdded='%s' where idDirent=%d", dateAdded.GetAsDBDateTime().c_str(), idDirent);
		m_pDS->exec(strSQL.c_str());
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s unable to update dateadded for file (%s)", __FUNCTION__, strSQL.c_str());
	}
}

bool CObjectDatabase::GetObjectDetails(CObjectInfoTag& details)
{
	return GetObjectDetails(details.m_idObject, details);
}

bool CObjectDatabase::GetObjectDetails(int idObject, CObjectInfoTag& details)
{
	try
	{
		if (NULL == m_pDB.get()) return false;
		if (NULL == m_pDS.get()) return false;

		details.m_idObject = idObject;

		CStdString strSQL=PrepareSQL("select * from viewObjectDirentAll where oID=%i", idObject);

		m_pDS->query(strSQL.c_str());
		if (m_pDS->num_rows() > 0)
		{
			details.m_name = m_pDS->fv("oName").get_asString();
			details.m_strFile = m_pDS->fv("dFileName").get_asString();
			details.m_strPath = m_pDS->fv("pPath").get_asString();
			ConstructPath(details.m_fileNameAndPath, details.m_strPath, details.m_strFile);
			details.m_url = m_pDS->fv("dUrl").get_asString();
			details.m_parentPathId = m_pDS->fv("pParent").get_asInt();
			details.m_fileId = m_pDS->fv("dID").get_asInt();

			details.m_dateAdded.SetFromDBDateTime(m_pDS->fv("dateAdded").get_asString());

		}
		m_pDS->close();

		//TODO: Set the profile here

		details.m_playCount = GetPlayCount(idObject, details.m_profileId);


	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s(%i) failed", __FUNCTION__, idObject);
	}

	return true;
}

void CObjectDatabase::GetAllAttributesForObject(CObjectInfoTag& tag)
{
	GetAllAttributesForObject(tag.m_idObject, tag.m_attributes);
}

void CObjectDatabase::GetAllRelationships(CObjectInfoTag& tag, int idRelationshipType)
{
	GetAllRelationships(tag.m_idObject, tag.m_relations, idRelationshipType);
}

int CObjectDatabase::GetNextSequenceIndex(const int idObject1, const int idRelationshipType)
{
	int retVal = -1;
	try
	{
		if (NULL == m_pDB.get()) return -1;
		if (NULL == m_pDS.get()) return -1;



		CStdString strSQL=PrepareSQL("select max(seqIndex) from viewRelationshipsAll where o1ID=%i and rtID=%i", idObject1, idRelationshipType);

		m_pDS->query(strSQL.c_str());
		if (m_pDS->num_rows() > 0)
		{
			retVal = m_pDS->fv(0).get_asInt()+1;
		}
		m_pDS->close();
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s(%i) failed", __FUNCTION__, idObject1);
	}

	return retVal;
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

void CObjectDatabase::UpdateObjectName(const int idObject, CStdString name)
{
	if (idObject < 0)
		return;

	try
	{
		if (NULL == m_pDB.get()) return ;
		if (NULL == m_pDS.get()) return ;

		CStdString strSQL = PrepareSQL("update objects set name='%s' where idObject=%i", name.c_str(), idObject);

		m_pDS->exec(strSQL.c_str());

	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s failed", __FUNCTION__);
	}
}

void CObjectDatabase::DeleteObject(int idObject, bool bKeep)
{
	if(idObject < 0)
		return;

	CStdString fileNameAndPath;
	if(GetObjectPath(idObject, fileNameAndPath))
	{
		DeleteObject(fileNameAndPath, idObject);
	} else {
		DeleteObject("", idObject, bKeep);
	}
}

void CObjectDatabase::DeleteObject(CStdString strFileNameAndPath, int idObject, bool bKeep)
{
	CStdString strSQL;
	try
	{
		if (NULL == m_pDB.get()) return ;
		if (NULL == m_pDS.get()) return ;
		if (idObject < 0)
		{
			idObject = GetObjectId(strFileNameAndPath);
			if (idObject < 0)
				return;
		}

		BeginTransaction();

		DeleteAttributesForObject(idObject);
		DeleteObjectLinks(idObject);
		RemoveObjectDirentLink(idObject);

		if(!bKeep)
		{
			strSQL=PrepareSQL("delete from objects where idObject=i%", idObject);
			m_pDS->exec(strSQL.c_str());
		}

		 CStdString strPath, strFileName;
		 SplitPath(strFileNameAndPath,strPath,strFileName);
		 InvalidatePathHash(strPath);

		CommitTransaction();
	}
	catch (...)
	{
		RollbackTransaction();
		CLog::Log(LOGERROR, "%s unable to deleteobject (%s)", __FUNCTION__, strSQL.c_str());
	}
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

void CObjectDatabase::RemoveObjectDirentLink(int idObject)
{

	CStdString strSQL;
	try
	{
		if (NULL == m_pDB.get()) return ;
		if (NULL == m_pDS.get()) return ;
		if (idObject < 0)
		{
			return;
		}


		strSQL=PrepareSQL("delete from objectlinkdirent where idObject=%i", idObject);
		m_pDS->exec(strSQL.c_str());

	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s unable to deleteobjectdirentlink (%s)", __FUNCTION__, strSQL.c_str());
	}
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

int CObjectDatabase::GetObjectId(CStdString strFileNameAndPath)
{
	CStdString strSQL = "";
	try
	{
		int idObject;
		if (NULL == m_pDB.get()) return -1;
		if (NULL == m_pDS.get()) return -1;

		CStdString strFileName, strPath;
		SplitPath(strFileNameAndPath,strPath,strFileName);

		CStdString strSQL=PrepareSQL("select oID from viewObjectDirentAll where dFileName='%s' and pPath LIKE '%%%s%%'", strFileName.c_str(),strPath.c_str());

		m_pDS->query(strSQL.c_str());
		if (m_pDS->num_rows() > 0)
		{
			idObject = m_pDS->fv("oID").get_asInt() ;
			m_pDS->close();
			return idObject;
		}
		m_pDS->close();
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s unable to getobjectid (%s)", __FUNCTION__, strSQL.c_str());
	}
	return -1;
}

bool CObjectDatabase::GetObjectPath(int idObject, CStdString& strFileNameAndPath)
{
	CStdString strSQL = "";
	try
	{
		if (NULL == m_pDB.get()) return false;
		if (NULL == m_pDS.get()) return false;



		CStdString strSQL=PrepareSQL("select dFileName, pPath from viewObjectDirentAll where oID=%i", idObject);

		m_pDS->query(strSQL.c_str());
		if (m_pDS->num_rows() > 0)
		{
			CStdString strFileName = m_pDS->fv("dFileName").get_asString();
			CStdString strPath = m_pDS->fv("pPath").get_asString();
			strFileNameAndPath = URIUtils::AddFileToFolder(strPath, strFileName);
			m_pDS->close();
			return true;
		}
		m_pDS->close();
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s unable to getobjectpath (%s)", __FUNCTION__, strSQL.c_str());
	}
	return false;
}

bool CObjectDatabase::GetObjectByAttribute(CStdString attr, const int idAttributeType, std::vector<int>& object_ids)
{
	try
	{
		if (NULL == m_pDB.get()) return false;
		if (NULL == m_pDS.get()) return false;


		CStdString strSQL=PrepareSQL("select idObject from attributes where idAttributeType=%i and valueString like '%%%s%%'", idAttributeType, attr.c_str());
		m_pDS2->query(strSQL.c_str());
		while (!m_pDS2->eof())
		{
			object_ids.push_back(m_pDS2->fv(0).get_asInt());
			m_pDS2->next();
		}
		m_pDS2->close();
		return true;
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s (%s) failed", __FUNCTION__, attr.c_str());
	}

	return false;
}

bool CObjectDatabase::GetObjectByAttribute(const int attr, const int idAttributeType, std::vector<int>& object_ids)
{
	try
	{
		if (NULL == m_pDB.get()) return false;
		if (NULL == m_pDS.get()) return false;


		CStdString strSQL=PrepareSQL("select idObject from attributes where idAttributeType=%i and valueNumber=%i", idAttributeType, attr);
		m_pDS2->query(strSQL.c_str());
		while (!m_pDS2->eof())
		{
			object_ids.push_back(m_pDS2->fv(0).get_asInt());
			m_pDS2->next();
		}
		m_pDS2->close();
		return true;
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s (%i) failed", __FUNCTION__, attr);
	}

	return false;
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

void CObjectDatabase::DeleteAttributesForObject(int idObject)
{
	CStdString strSQL;
	try
	{
		if (NULL == m_pDB.get()) return ;
		if (NULL == m_pDS.get()) return ;
		if (idObject < 0)
		{
			return;
		}


		strSQL=PrepareSQL("delete from attributes where idObject=%i", idObject);
		m_pDS->exec(strSQL.c_str());

	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s unable to deleteobjectattributes (%s)", __FUNCTION__, strSQL.c_str());
	}
}

bool CObjectDatabase::SetAttribute(const int idObject, int idAttributeType, CStdString attrValue, int idAttribute)
{
	CAttributeType type;
	GetAttributeType(idAttributeType, type);
	CAttribute attr;
	attr.setStringValue(attrValue);


	return SetAttribute(idObject, type, attr, idAttribute);
}

bool CObjectDatabase::SetAttribute(const int idObject, int idAttributeType, float attrValue, int idAttribute)
{
	CAttributeType type;
	GetAttributeType(idAttributeType, type);
	CAttribute attr;
	attr.setNumericValue(attrValue);


	return SetAttribute(idObject, type, attr, idAttribute);
}

bool CObjectDatabase::SetAttribute(const int idObject, CAttributeType attrType, CAttribute attr, int idAttribute)
{
	CStdString strSQL;
	try
	{

		if(!isValidAttributeType(idObject, attrType.idAttributeType)) return false;

		attr.setType(attrType);
		switch (attrType.type) {
		case STRING_ATTRIBUTE:
			if(idAttribute < 0)
			{
				strSQL=PrepareSQL("INSERT INTO attributes (idObject, idAttributeType, valueString) VALUES (%i, %i, '%s')",idObject, attrType.idAttributeType, attr.getStringValue().c_str());
			}
			else
			{
				strSQL=PrepareSQL("UPDATE attributes SET valueString='%s' WHERE idAttribute=%i", attr.getStringValue().c_str(), idAttribute);
			}
			break;
		case NUMBER_ATTRIBUTE:
			if(idAttribute < 0)
			{
				strSQL=PrepareSQL("INSERT INTO attributes (idObject, idAttributeType, valueNumber) VALUES (%i, %i, %i)",idObject, attrType.idAttributeType, attr.getRawIntValue());
			}
			else
			{
				strSQL=PrepareSQL("UPDATE attributes SET valueNumber=%i WHERE idAttribute=%i", attr.getRawIntValue(), idAttribute);
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

			attribute.setNumericValue(m_pDS2->fv("a.valueNumber").get_asInt());
			attribute.setStringValue(m_pDS2->fv("a.valueString").get_asString());
			attribute.setType(t);
			attribute.idAttribute = m_pDS2->fv("a.idAttribute").get_asInt();


		}

		m_pDS2->close();
		return true;

	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s unable to getattribute (%s)", __FUNCTION__, strSQL.c_str());
	}
	return false;
}

void CObjectDatabase::GetAllAttributesForObject(const int idObject, AttributeList& attributes)
{
	CStdString strSQL;
	try
	{
		if (NULL == m_pDB.get()) return;
		if (NULL == m_pDS.get()) return;
		strSQL=PrepareSQL("SELECT idAttribute FROM attributes WHERE idObject=%i", idObject);
		m_pDS->query(strSQL.c_str());
		while (!m_pDS->eof())
		{
			int idAttribute = m_pDS->fv(0).get_asInt();
			CAttribute attr;
			if(GetAttribute(idAttribute, attr))
			{
				attributes.push_back(attr);
			}
			m_pDS->next();
		}
		m_pDS->close();

	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s unable to getattributelist (%s)", __FUNCTION__, strSQL.c_str());
	}
}

bool CObjectDatabase::GetAttributeType(const int idAttributeType, CAttributeType& attributeType)
{
	CStdString strSQL;
	try
	{

		if (NULL == m_pDB.get()) return false;
		if (NULL == m_pDS.get()) return false;

		strSQL=PrepareSQL("SELECT * FROM attributeTypes WHERE idAttributeType=%i", idAttributeType);
		m_pDS2->query(strSQL.c_str());
		if(!m_pDS2->eof())
		{

			attributeType.idAttributeType = m_pDS2->fv("idAttributeType").get_asInt();
			attributeType.idObjectType = m_pDS2->fv("idObjectType").get_asInt();
			attributeType.inheritable = m_pDS2->fv("inheritable").get_asBool();
			attributeType.name = m_pDS2->fv("name").get_asString();
			attributeType.stub = m_pDS2->fv("stub").get_asString();
			attributeType.type = ATTRIBUTE_DATA_TYPE(m_pDS2->fv("dataType").get_asInt());
			attributeType.precision = m_pDS2->fv("dataPrecision").get_asInt();


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

int CObjectDatabase::AddRelationshipType(CStdString stub, int idObjectType1, int idObjectType2, int inheritableType1, int inheritableType2, int sequenced)
{
	try
	{
		if (NULL == m_pDB.get()) return -1;
		if (NULL == m_pDS.get()) return -1;
		int idRelationshipType = -1;
		CStdString strSQL = PrepareSQL("SELECT idRelationshipType FROM relationshipTypes WHERE stub LIKE '%s'",stub.c_str());
		m_pDS->query(strSQL.c_str());
		if (m_pDS->num_rows() == 0)
		{
			m_pDS->close();
			// doesnt exists, add it
			strSQL=PrepareSQL("insert into relationshipTypes (idRelationshipType, idObjectType1, idObjectType2, stub, inheritableType1, inheritableType2, sequenced) values( NULL, %i, %i, '%s', %i, %i, %i)", idObjectType1, idObjectType2, stub.c_str(), inheritableType1, inheritableType2, sequenced);
			m_pDS->exec(strSQL.c_str());
			idRelationshipType = (int)m_pDS->lastinsertid();
		}
		else
		{
			idRelationshipType = m_pDS->fv("idAttributeType").get_asInt();
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

int CObjectDatabase::LinkObjectToObject(int idRelationshipType, int idObject1, int idObject2, CStdString link, int index, bool remove)
{
	CStdString strSQL;
	try
	{
		if (NULL == m_pDB.get()) return -1;
		if (NULL == m_pDS.get()) return -1;

		int idRelationship = GetRelationshipId(idRelationshipType, idObject1, idObject2, link, index);

		if(idRelationship >= 0)
			return idRelationship;

		if(remove)
		{
			strSQL=PrepareSQL("DELETE FROM relationships WHERE idRelationshipType=%i AND idObject1=%i AND idObject2=%i",idRelationshipType,idObject1,idObject2);
			m_pDS->exec(strSQL.c_str());
			return 0;
		}


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

void CObjectDatabase::DeleteObjectLinks(int idObject, int idRelationshipType)
{
	CStdString strSQL;
	try
	{
		if (NULL == m_pDB.get()) return ;
		if (NULL == m_pDS.get()) return ;
		if (idObject < 0)
		{
			return;
		}


		strSQL=PrepareSQL("delete from relationships where (idObject1=%i or idObject2=%i)", idObject, idObject);
		if(idRelationshipType > 0)
			strSQL.AppendFormat(" AND idRelationshipType=%i", idRelationshipType);
		m_pDS->exec(strSQL.c_str());

	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s unable to deleteobjectlinks (%s)", __FUNCTION__, strSQL.c_str());
	}
}

bool CObjectDatabase::GetLinksForObject(int idObject, int idRelationshipType, std::vector<std::pair <int,int> >& objects, OBJECT_RELATIONSHIP_POSITION position, int index, bool sort)
{
	CStdString strSQL;
	try
	{
		if (NULL == m_pDB.get()) return false;
		if (NULL == m_pDS.get()) return false;


		switch (position) {
			case FIRST_OBJECT:
				strSQL = PrepareSQL("select seqIndex, o2ID from viewRelationshipsAll where o1ID=%i and rtID=%i", idObject, idRelationshipType);
				break;
			case SECOND_OBJECT:
				strSQL = PrepareSQL("select seqIndex, o1ID from viewRelationshipsAll where o2ID=%i and rtID=%i", idObject, idRelationshipType);
				break;
			default:
				return false;
		}

		if(index >= 0)
			strSQL.AppendFormat(" and seqIndex=%i", index);

		if(sort)
			strSQL.AppendFormat(" order by seqIndex");

		m_pDS2->query(strSQL.c_str());
		while (!m_pDS2->eof())
		{
			int sequenceIndex = m_pDS2->fv(0).get_asInt();
			int idOtherObject = m_pDS2->fv(1).get_asInt();
			objects.push_back(make_pair(sequenceIndex, idOtherObject));
			m_pDS2->next();
		}

		m_pDS2->close();
		return true;

	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s (%s) failed", __FUNCTION__, strSQL.c_str());
	}

	return false;
}

bool CObjectDatabase::HasRelations(const int idRelationshipType)
{
	try
	{
		if (NULL == m_pDB.get()) return false;
		if (NULL == m_pDS.get()) return false;

		m_pDS->query(PrepareSQL("SELECT * FROM relationships WHERE idRelationshipType=%i", idRelationshipType).c_str());

		bool bResult = (m_pDS->num_rows() > 0);
		m_pDS->close();
		return bResult;
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s failed", __FUNCTION__);
	}
	return false;
}

bool CObjectDatabase::HasRelationship(const int idObject, const int idRelationshipType)
{
	CStdString strSQL;
	try
	{
		if (NULL == m_pDB.get()) return false;
		if (NULL == m_pDS.get()) return false;

		strSQL=PrepareSQL("SELECT * FROM viewRelationshipsAll WHERE (o1ID=%i OR o2ID=%i)", idObject, idObject);

		if(idRelationshipType > 0)
			strSQL.AppendFormat(" AND rtID=%i", idRelationshipType);

		m_pDS2->query(strSQL.c_str());
		if (m_pDS2->eof())
		{
			m_pDS2->close();
			return false;
		}

		m_pDS2->close();
		return true;

	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s (%s) failed", __FUNCTION__, strSQL.c_str());
	}

	return false;
}

bool CObjectDatabase::GetAllRelationships(const int idObject, std::vector<CRelationship>& relations, int idRelationshipType)
{
	CStdString strSQL;
	try
	{
		if (NULL == m_pDB.get()) return false;
		if (NULL == m_pDS.get()) return false;

		strSQL=PrepareSQL("SELECT * FROM viewRelationshipsAll WHERE (o1ID=%i OR o2ID=%i)", idObject, idObject);

		if(idRelationshipType > 0)
			strSQL.AppendFormat(" AND rtID=%i", idRelationshipType);

		m_pDS2->query(strSQL.c_str());
		while (!m_pDS2->eof())
		{
			CRelationship relationship;

			relationship.m_o1ID = m_pDS2->fv("o1ID").get_asInt();
			relationship.m_o1Name = m_pDS2->fv("o1Name").get_asString();
			relationship.m_o1Stub = m_pDS2->fv("o1Stub").get_asString();
			relationship.m_o1TypeID = m_pDS2->fv("o1TypeID").get_asInt();
			relationship.m_o1TypeName = m_pDS2->fv("o1TypeName").get_asString();

			relationship.m_o2ID = m_pDS2->fv("o2ID").get_asInt();
			relationship.m_o2Name = m_pDS2->fv("o2Name").get_asString();
			relationship.m_o2Stub = m_pDS2->fv("o2Stub").get_asString();
			relationship.m_o2TypeID = m_pDS2->fv("o2TypeID").get_asInt();
			relationship.m_o2TypeName = m_pDS2->fv("o2TypeName").get_asString();

			relationship.m_type= m_pDS2->fv("rtName").get_asString();
			relationship.m_rtID = m_pDS2->fv("rtID").get_asInt();
			relationship.m_link = m_pDS2->fv("link").get_asString();
			relationship.m_index = m_pDS2->fv("seqIndex").get_asInt();

			relations.push_back(relationship);

			m_pDS2->next();
		}

		m_pDS2->close();


		return true;

	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s (%s) failed", __FUNCTION__, strSQL.c_str());
	}

	return false;
}

bool CObjectDatabase::GetRelationship(const int idRelationship, CRelationship& relationship)
{
	CStdString strSQL;
	try
	{

		if (NULL == m_pDB.get()) return false;
		if (NULL == m_pDS.get()) return false;

		strSQL=PrepareSQL("SELECT * FROM viewRelationshipsAll WHERE rID=%i", idRelationship);
		m_pDS2->query(strSQL.c_str());
		if(!m_pDS2->eof())
		{
			relationship.m_o1ID = m_pDS2->fv("o1ID").get_asInt();
			relationship.m_o1Name = m_pDS2->fv("o1Name").get_asString();
			relationship.m_o1Stub = m_pDS2->fv("o1Stub").get_asString();
			relationship.m_o1TypeID = m_pDS2->fv("o1TypeID").get_asInt();
			relationship.m_o1TypeName = m_pDS2->fv("o1TypeName").get_asString();

			relationship.m_o2ID = m_pDS2->fv("o2ID").get_asInt();
			relationship.m_o2Name = m_pDS2->fv("o2Name").get_asString();
			relationship.m_o2Stub = m_pDS2->fv("o2Stub").get_asString();
			relationship.m_o2TypeID = m_pDS2->fv("o2TypeID").get_asInt();
			relationship.m_o2TypeName = m_pDS2->fv("o2TypeName").get_asString();

			relationship.m_type= m_pDS2->fv("rtName").get_asString();
			relationship.m_link = m_pDS2->fv("link").get_asString();
			relationship.m_index = m_pDS2->fv("seqIndex").get_asInt();

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

bool CObjectDatabase::isValidArtworkType(int idObject, int idArtworkType)
{
	CStdString strSQL;
	try
	{
		if (NULL == m_pDB.get()) return false;
		if (NULL == m_pDS.get()) return false;

		int idObjectType = GetObjectType(idObject);
		vector<int> artworkTypes;
		if(GetAllArtworkTypeIDsForObjectType(idObjectType, artworkTypes))
		{
			return std::find(artworkTypes.begin(), artworkTypes.end(), idArtworkType) != artworkTypes.end();
		}

	}
	catch(...)
	{
		CLog::Log(LOGERROR, "%s unable to checkvalidity (%s)", __FUNCTION__, strSQL.c_str());
	}

	return false;
}

int CObjectDatabase::AddArtworkType(int idObjectType, CStdString stub, CStdString name, int inheritable)
{
	try
	{
		if (NULL == m_pDB.get()) return -1;
		if (NULL == m_pDS.get()) return -1;
		int idArtworkType = -1;
		CStdString strSQL = PrepareSQL("SELECT idArtworkType FROM artworkTypes WHERE stub LIKE '%s' and idObjectType=%i",stub.c_str(), idObjectType);
		m_pDS->query(strSQL.c_str());
		if (m_pDS->num_rows() == 0)
		{
			m_pDS->close();
			// doesnt exists, add it
			strSQL=PrepareSQL("insert into artworkTypes (idArtworkType, idObjectType, stub, name, inheritable) values( NULL, %i, '%s', '%s', %i)", idObjectType, stub.c_str(), name.c_str(), inheritable);
			m_pDS->exec(strSQL.c_str());
			idArtworkType = (int)m_pDS->lastinsertid();
		}
		else
		{
			idArtworkType = m_pDS->fv("idAttributeType").get_asInt();
			m_pDS->close();
		}

		return idArtworkType;
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s (%s) failed", __FUNCTION__, stub.c_str() );
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

			CStdString strSQL=PrepareSQL("select idArtworkType from artworkTypes where idObjectType IN (%s) AND inheritable = 1 OR idObjectType = %i", StringUtils::Join(objectIds,",").c_str(),idObjectType);
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

CStdString CObjectDatabase::GetArtworkTypeStub(int idArtworkType)
{

	CStdString strSQL=PrepareSQL("select stub from artworkTypes where idArtworkType=%i", idArtworkType);
	return GetSingleValue(strSQL, m_pDS2);
}

CStdString CObjectDatabase::GetArtForItem(int idObject, const int &idArtworkType)
{
	CStdString query = PrepareSQL("SELECT url FROM artwork WHERE idObject=%i AND idArtworkType=%i", idObject, idArtworkType);
	return GetSingleValue(query, m_pDS2);
}

bool CObjectDatabase::GetArtForItem(int idObject, map<string, string> &art)
{
	try
	{
		if (NULL == m_pDB.get()) return false;
		if (NULL == m_pDS2.get()) return false; // using dataset 2 as we're likely called in loops on dataset 1

		CStdString sql = PrepareSQL("SELECT at.stub, a.url FROM artwork a JOIN artworkTypes at ON a.idArtworkType = at.idArtworkType WHERE idObject=%i", idObject);
		m_pDS2->query(sql.c_str());
		while (!m_pDS2->eof())
		{
			art.insert(make_pair(m_pDS2->fv(0).get_asString(), m_pDS2->fv(1).get_asString()));
			m_pDS2->next();
		}
		m_pDS2->close();
		return !art.empty();
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s(%d) failed", __FUNCTION__, idObject);
	}
	return false;
}

void CObjectDatabase::SetArtForItem(int idObject, const map<int, string> &art)
{
	for (map<int, string>::const_iterator i = art.begin(); i != art.end(); ++i)
		SetArtForItem(idObject, i->first, i->second);
}

//This will need an update (or additional method) to set the artwork for a CTexture object
void CObjectDatabase::SetArtForItem(int idObject, const int idArtworkType, const CStdString &url)
{
	try
	{
		if (NULL == m_pDB.get()) return;
		if (NULL == m_pDS.get()) return;
		if (!isValidArtworkType(idObject, idArtworkType)) return;

		CStdString sql = PrepareSQL("SELECT idArtwork FROM artwork WHERE idObject=%i AND idArtworkType=%i", idObject, idArtworkType);
		m_pDS->query(sql.c_str());
		if (!m_pDS->eof())
		{ // update
			int artId = m_pDS->fv(0).get_asInt();
			m_pDS->close();
			sql = PrepareSQL("UPDATE artwork SET url='%s', lastUpdated='%s' where idArtwork=%d", url.c_str(), CDateTime::GetCurrentDateTime().GetAsDBDateTime().c_str(), artId);
			m_pDS->exec(sql.c_str());
		}
		else
		{ // insert
			m_pDS->close();
			sql = PrepareSQL("INSERT INTO artwork (idObject, idArtworkType, url, lastUpdated, width, height) VALUES (%i, %i, '%s', '%s', 0, 0)", idObject, idArtworkType, url.c_str(), CDateTime::GetCurrentDateTime().GetAsDBDateTime().c_str());
			m_pDS->exec(sql.c_str());
		}


	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s(%d, %d, '%s') failed", __FUNCTION__, idObject, idArtworkType, url.c_str());
	}
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
			idProfile = m_pDS->fv("idProfile").get_asInt();

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

void CObjectDatabase::GetBookMarksForFile(const CStdString& strFilenameAndPath, int idProfile, VECBOOKMARKS& bookmarks, CBookmark::EType type /*= CBookmark::STANDARD*/, bool bAppend)
{
	try
	{
		//    if (URIUtils::IsStack(strFilenameAndPath) && CFileItem(CStackDirectory::GetFirstStackedFile(strFilenameAndPath),false).IsDVDImage())
		//    {
		//      CStackDirectory dir;
		//      CFileItemList fileList;
		//      dir.GetDirectory(strFilenameAndPath, fileList);
		//      if (!bAppend)
		//        bookmarks.clear();
		//      for (int i = fileList.Size() - 1; i >= 0; i--) // put the bookmarks of the highest part first in the list
		//        GetBookMarksForFile(fileList[i]->GetPath(), bookmarks, type, true, (i+1));
		//    }
		//    else
		//    {
		int idFile = AddDirEnt(strFilenameAndPath);
		if (idFile < 0) return ;
		if (!bAppend)
			bookmarks.erase(bookmarks.begin(), bookmarks.end());
		if (NULL == m_pDB.get()) return ;
		if (NULL == m_pDS.get()) return ;

		CStdString strSQL=PrepareSQL("select * from bookmark where idDirent=%i and idProfile=%i and type=%i order by timeInSeconds", idFile, idProfile, (int)type);
		m_pDS->query( strSQL.c_str() );
		while (!m_pDS->eof())
		{
			CBookmark bookmark;
			bookmark.timeInSeconds = m_pDS->fv("timeInSeconds").get_asDouble();
			bookmark.totalTimeInSeconds = m_pDS->fv("totalTimeInSeconds").get_asDouble();
			bookmark.thumbNailImage = m_pDS->fv("thumbnailImage").get_asString();
			bookmark.playerState = m_pDS->fv("playerState").get_asString();
			bookmark.player = m_pDS->fv("player").get_asString();
			bookmark.type = type;
			bookmarks.push_back(bookmark);
			m_pDS->next();
		}
		//sort(bookmarks.begin(), bookmarks.end(), SortBookmarks);
		m_pDS->close();
		//  }
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s (%s) failed", __FUNCTION__, strFilenameAndPath.c_str());
	}
}

bool CObjectDatabase::GetResumeBookMark(const CStdString& strFilenameAndPath, int idProfile, CBookmark &bookmark)
{
	VECBOOKMARKS bookmarks;
	GetBookMarksForFile(strFilenameAndPath, idProfile, bookmarks, CBookmark::RESUME, false);
	if (bookmarks.size() > 0)
	{
		bookmark = bookmarks[0];
		return true;
	}
	return false;
}

bool CObjectDatabase::GetResumePoint(CObjectInfoTag& tag)
{
	return GetResumeBookMark(tag.m_fileNameAndPath, tag.m_profileId, tag.m_resumePoint);
}

void CObjectDatabase::DeleteResumeBookMark(const CStdString &strFilenameAndPath, int idProfile)
{
	if (!m_pDB.get() || !m_pDS.get())
		return;

	int fileID = AddDirEnt(strFilenameAndPath);
	if (fileID < -1)
		return;

	try
	{
		CStdString sql = PrepareSQL("delete from bookmark where idDirent=%i and idProfile=%i and type=%i", fileID, idProfile, CBookmark::RESUME);
		m_pDS->exec(sql.c_str());
	}
	catch(...)
	{
		CLog::Log(LOGERROR, "%s (%s) failed", __FUNCTION__, strFilenameAndPath.c_str());
	}
}

//********************************************************************************************************************************
void CObjectDatabase::AddBookMarkToFile(const CStdString& strFilenameAndPath, int idProfile, const CBookmark &bookmark, CBookmark::EType type /*= CBookmark::STANDARD*/)
{
	try
	{
		int idFile = AddDirEnt(strFilenameAndPath);
		if (idFile < 0)
			return;
		if (NULL == m_pDB.get()) return ;
		if (NULL == m_pDS.get()) return ;

		CStdString strSQL;
		int idBookmark=-1;
		if (type == CBookmark::RESUME) // get the same resume mark bookmark each time type
		{
			strSQL=PrepareSQL("select idBookmark from bookmark where idDirent=%i and idProfile=%i and type=%i", idFile, idProfile, (int)type);
		}
		else if (type == CBookmark::STANDARD) // get the same bookmark again, and update. not sure here as a dvd can have same time in multiple places, state will differ thou
		{
			/* get a bookmark within the same time as previous */
			double mintime = bookmark.timeInSeconds - 0.5f;
			double maxtime = bookmark.timeInSeconds + 0.5f;
			strSQL=PrepareSQL("select idBookmark from bookmark where idDirent=%i and idProfile=%i and type=%i and (timeInSeconds between %f and %f) and playerState='%s'", idFile, idProfile, (int)type, mintime, maxtime, bookmark.playerState.c_str());
		}

		// update or insert depending if it existed before
		if (idBookmark >= 0 )
			strSQL=PrepareSQL("update bookmark set timeInSeconds = %f, totalTimeInSeconds = %f, thumbNailImage = '%s', player = '%s', playerState = '%s' where idBookmark = %i", bookmark.timeInSeconds, bookmark.totalTimeInSeconds, bookmark.thumbNailImage.c_str(), bookmark.player.c_str(), bookmark.playerState.c_str(), idBookmark);
		else
			strSQL=PrepareSQL("insert into bookmark (idBookmark, idDirent, idProfile, timeInSeconds, totalTimeInSeconds, thumbNailImage, player, playerState, type) values(NULL,%i,%i,%f,%f,'%s','%s','%s', %i)", idFile, idProfile, bookmark.timeInSeconds, bookmark.totalTimeInSeconds, bookmark.thumbNailImage.c_str(), bookmark.player.c_str(), bookmark.playerState.c_str(), (int)type);

		m_pDS->exec(strSQL.c_str());
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s (%s) failed", __FUNCTION__, strFilenameAndPath.c_str());
	}
}

void CObjectDatabase::ClearBookMarkOfFile(const CStdString& strFilenameAndPath, int idProfile, CBookmark& bookmark, CBookmark::EType type /*= CBookmark::STANDARD*/)
{
	try
	{
		int idFile = AddDirEnt(strFilenameAndPath);
		if (idFile < 0) return ;
		if (NULL == m_pDB.get()) return ;
		if (NULL == m_pDS.get()) return ;

		/* a litle bit uggly, we clear first bookmark that is within one second of given */
		/* should be no problem since we never add bookmarks that are closer than that   */
		double mintime = bookmark.timeInSeconds - 0.5f;
		double maxtime = bookmark.timeInSeconds + 0.5f;
		CStdString strSQL = PrepareSQL("select idBookmark from bookmark where idDirent=%i and idProfile=%i and type=%i and playerState like '%s' and player like '%s' and (timeInSeconds between %f and %f)", idFile, idProfile, type, bookmark.playerState.c_str(), bookmark.player.c_str(), mintime, maxtime);

		m_pDS->query( strSQL.c_str() );
		if (m_pDS->num_rows() != 0)
		{
			int idBookmark = m_pDS->get_field_value("idBookmark").get_asInt();
			strSQL=PrepareSQL("delete from bookmark where idBookmark=%i",idBookmark);
			m_pDS->exec(strSQL.c_str());

		}

		m_pDS->close();
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s (%s) failed", __FUNCTION__, strFilenameAndPath.c_str());
	}
}

//********************************************************************************************************************************
void CObjectDatabase::ClearBookMarksOfFile(const CStdString& strFilenameAndPath, int idProfile, CBookmark::EType type /*= CBookmark::STANDARD*/)
{
	try
	{
		int idFile = AddDirEnt(strFilenameAndPath);
		if (idFile < 0) return ;
		if (NULL == m_pDB.get()) return ;
		if (NULL == m_pDS.get()) return ;

		CStdString strSQL=PrepareSQL("delete from bookmark where idDirent=%i and idProfile=%i and type=%i", idFile, idProfile, (int)type);
		m_pDS->exec(strSQL.c_str());
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s (%s) failed", __FUNCTION__, strFilenameAndPath.c_str());
	}
}

void CObjectDatabase::SetStreamDetailsForFileId(const CStdString detailsXML, int idFile)
{
	try
	{
		if (idFile < 0) return ;
		if (NULL == m_pDB.get()) return ;
		if (NULL == m_pDS.get()) return ;

		CStdString strSQL=PrepareSQL("update dirents set streams='%s' where idDirent=%i", detailsXML.c_str(), idFile);
		m_pDS->exec(strSQL.c_str());
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s (%s) failed", __FUNCTION__, detailsXML.c_str());
	}
}

void CObjectDatabase::SetStreamDetailsForFileId(const CStreamDetails& details, int idFile)
{
	TiXmlDocument doc;

	TiXmlElement * root = new TiXmlElement( "streams" );
	doc.LinkEndChild( root );

	for(int i=1; i<=details.GetVideoStreamCount(); i++)
	{
		TiXmlElement * stream = new TiXmlElement( "stream" );
		root->LinkEndChild( stream );

		XMLUtils::SetInt(stream, "type", (int)CStreamDetail::VIDEO);
		XMLUtils::SetString(stream, "videoCodec", details.GetVideoCodec(i));
		XMLUtils::SetFloat(stream, "videoAspect", details.GetVideoAspect(i));
		XMLUtils::SetInt(stream, "videoWidth", details.GetVideoWidth(i));
		XMLUtils::SetInt(stream, "videoHeight", details.GetVideoHeight(i));
		XMLUtils::SetInt(stream, "videoDuration", details.GetVideoDuration(i));
	}
	for (int i=1; i<=details.GetAudioStreamCount(); i++)
	{
		TiXmlElement * stream = new TiXmlElement( "stream" );
		root->LinkEndChild( stream );

		XMLUtils::SetInt(stream, "type", (int)CStreamDetail::AUDIO);
		XMLUtils::SetString(stream, "audioCodec", details.GetAudioCodec(i));
		XMLUtils::SetInt(stream, "audioChannels", details.GetAudioChannels(i));
		XMLUtils::SetString(stream, "audioLanguage", details.GetAudioLanguage(i));
	}
	for (int i=1; i<=details.GetSubtitleStreamCount(); i++)
	{
		TiXmlElement * stream = new TiXmlElement( "stream" );
		root->LinkEndChild( stream );

		XMLUtils::SetInt(stream, "type", (int)CStreamDetail::SUBTITLE);
		XMLUtils::SetString(stream, "subtitleLanguage", details.GetSubtitleLanguage(i));
	}

	TiXmlPrinter printer;
	printer.SetLineBreak("\n");
	printer.SetIndent("  ");
	doc.Accept(&printer);
	//CLog::Log(LOGNOTICE, "Contents of settings for dirent %i are...\n%s", idFile, printer.CStr());

	SetStreamDetailsForFileId(printer.CStr(), idFile);
}

void CObjectDatabase::SetStreamDetailsForFile(const CStreamDetails& details, const CStdString &strFileNameAndPath)
{
	int idFile = AddDirEnt(strFileNameAndPath);
	if(idFile < 0) return;

	SetStreamDetailsForFileId(details, idFile);
}

bool CObjectDatabase::GetStreamDetails(CStreamDetails& details, int idFile)
{
	CStdString strSQL;
	bool retValue = false;
	try
	{
		CStdString strSettings;
		if (idFile < 0) return false;
		if (NULL == m_pDB.get()) return false;
		if (NULL == m_pDS.get()) return false;

		strSQL = PrepareSQL("SELECT streams FROM dirents WHERE idDirent=%i", idFile);

		m_pDS->query(strSQL.c_str());
		if (!m_pDS->eof())
			strSettings = m_pDS->fv("streams").get_asString();
		m_pDS->close();

		if(!strSettings)
		{
			CLog::Log(LOGERROR, "Unable to load stream settings for dirent %i", idFile);
			return false;
		}

		retValue = ParseStreamDetails(strSettings, details);

	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s unable to getstreamdetails (%s)", __FUNCTION__, strSQL.c_str());
	}

	details.DetermineBestStreams();

	return retValue;
}

bool CObjectDatabase::GetStreamDetails(CObjectInfoTag& tag)
{
	bool retVal = false;
	try
	{
		if (NULL == m_pDB.get()) return false;
		if (NULL == m_pDS.get()) return false;



		CStdString strSQL=PrepareSQL("select dStreams from viewObjectDirentAll where oID=%i", tag.m_idObject);

		m_pDS->query(strSQL.c_str());
		if (m_pDS->num_rows() > 0)
		{
			CStdString streamsXML = m_pDS->fv("dStreams").get_asString();
			if(!streamsXML.IsEmpty())
			{
				retVal = ParseStreamDetails(streamsXML, tag.m_streams);
				tag.m_streams.DetermineBestStreams();

				if(tag.m_streams.GetVideoDuration() > 0)
					tag.m_duration = tag.m_streams.GetVideoDuration();
			}
		}
		m_pDS->close();
	}
	catch (...)
	{

	}

	return retVal;
}

bool CObjectDatabase::ParseStreamDetails(CStdString xml, CStreamDetails& details)
{
	CXBMCTinyXML detailsXML;

	detailsXML.Parse(xml);

	TiXmlNode *root = detailsXML.RootElement();

	bool retValue = false;
	//CLog::Log(LOGNOTICE, "Root node has value %s", root->ValueStr().c_str());
	if (root && strcmp(root->ValueStr().c_str(), "streams") == 0)
	{
		//CLog::Log(LOGNOTICE, "Loaded settings from dirent from %i", idFile);
		TiXmlPrinter printer;
		printer.SetLineBreak("\n");
		printer.SetIndent("  ");
		detailsXML.Accept(&printer);
		//CLog::Log(LOGNOTICE, "Contents of settings for dirent %i are...\n%s", idFile, printer.CStr());

		TiXmlNode *stream = root->FirstChild("stream");
		CStdString id;
		for(stream; stream; stream=stream->NextSibling("stream"))
		{
			int type;
			CStreamDetail::StreamType e;
			if(XMLUtils::GetInt(stream, "type", type, 0, 2))
				e = (CStreamDetail::StreamType)type;
			else
				continue;

			CLog::Log(LOGNOTICE, "Stream type is %i", type);

			switch (e) {
			case CStreamDetail::VIDEO:
			{
				CStreamDetailVideo *vp = new CStreamDetailVideo();
				XMLUtils::GetString(stream, "videoCodec", vp->m_strCodec);
				XMLUtils::GetFloat(stream, "videoAspect", vp->m_fAspect);
				XMLUtils::GetInt(stream, "videoWidth", vp->m_iWidth);
				XMLUtils::GetInt(stream, "videoHeight", vp->m_iHeight);
				XMLUtils::GetInt(stream, "videoDuration", vp->m_iDuration);
				details.AddStream(vp);
				retValue = true;
				break;
			}
			case CStreamDetail::AUDIO:
			{
				CStreamDetailAudio *ap = new CStreamDetailAudio();
				XMLUtils::GetString(stream, "audioCodec", ap->m_strCodec);
				XMLUtils::GetInt(stream, "audioChannels", ap->m_iChannels);
				XMLUtils::GetString(stream, "audioLanguage", ap->m_strLanguage);
				details.AddStream(ap);
				retValue = true;
				break;
			}
			case CStreamDetail::SUBTITLE:
			{
				CStreamDetailSubtitle *sp = new CStreamDetailSubtitle();
				XMLUtils::GetString(stream, "subtitleLanguage", sp->m_strLanguage);
				details.AddStream(sp);
				retValue = true;
				break;
			}
			}


		}
	}
	else
	{
		CLog::Log(LOGERROR, "Improperly formatted XML %s", xml.c_str());
		return false;
	}

	return retValue;
}


void CObjectDatabase::SetVideoSettingsForFile(const CVideoSettings& settings, const CStdString &strFileNameAndPath)
{
	int idFile = AddDirEnt(strFileNameAndPath);
	if(idFile < 0) return;

	SetVideoSettingsForFileId(settings, idFile);
}

void CObjectDatabase::SetVideoSettingsForFileId(const CVideoSettings& settings, int idFile)
{
	TiXmlDocument doc;

	TiXmlElement * root = new TiXmlElement( "settings" );
	doc.LinkEndChild( root );

	XMLUtils::SetInt(root, "viewmode", settings.m_ViewMode);
	XMLUtils::SetFloat(root, "zoomamount", settings.m_CustomZoomAmount);
	XMLUtils::SetFloat(root, "pixelratio", settings.m_CustomPixelRatio);
	XMLUtils::SetInt(root, "audiostream", settings.m_AudioStream);
	XMLUtils::SetInt(root, "subtitlestream", settings.m_SubtitleStream);
	XMLUtils::SetFloat(root, "subtitledelay", settings.m_SubtitleDelay);
	XMLUtils::SetBoolean(root, "subtitleson", settings.m_SubtitleOn);
	XMLUtils::SetFloat(root, "brightness", settings.m_Brightness);
	XMLUtils::SetFloat(root, "contrast", settings.m_Contrast);
	XMLUtils::SetFloat(root, "gamma", settings.m_Gamma);
	XMLUtils::SetFloat(root, "volumeamplification", settings.m_VolumeAmplification);
	XMLUtils::SetFloat(root, "audiodelay", settings.m_AudioDelay);
	XMLUtils::SetBoolean(root, "outputtoallspeakers", settings.m_OutputToAllSpeakers);
	XMLUtils::SetBoolean(root, "crop", settings.m_Crop);
	XMLUtils::SetInt(root, "cropbottom", settings.m_CropBottom);
	XMLUtils::SetInt(root, "croptop", settings.m_CropTop);
	XMLUtils::SetInt(root, "cropleft", settings.m_CropLeft);
	XMLUtils::SetInt(root, "cropright", settings.m_CropRight);
	XMLUtils::SetFloat(root, "sharpness", settings.m_Sharpness);
	XMLUtils::SetFloat(root, "noisereduction", settings.m_NoiseReduction);
	XMLUtils::SetBoolean(root, "nonlinstretch", settings.m_CustomNonLinStretch);
	XMLUtils::SetFloat(root, "verticalshift", settings.m_CustomVerticalShift);
	XMLUtils::SetInt(root, "deinterlacemode", (int)settings.m_DeinterlaceMode);
	XMLUtils::SetInt(root, "interlacemethod", (int)settings.m_InterlaceMethod);
	XMLUtils::SetInt(root, "scalingmethod", (int)settings.m_ScalingMethod);

	TiXmlPrinter printer;
	printer.SetLineBreak("\n");
	printer.SetIndent("  ");
	doc.Accept(&printer);

	SetVideoSettingsForFileId(printer.CStr(), idFile);
}

void CObjectDatabase::SetVideoSettingsForFileId(const CStdString settingsXML, int idFile)
{
	try
	{
		if (idFile < 0) return ;
		if (NULL == m_pDB.get()) return ;
		if (NULL == m_pDS.get()) return ;

		CStdString strSQL=PrepareSQL("update dirents set settings='%s' where idDirent=%i", settingsXML.c_str(), idFile);
		m_pDS->exec(strSQL.c_str());
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s (%s) failed", __FUNCTION__, settingsXML.c_str());
	}
}

bool CObjectDatabase::GetVideoSettings(CVideoSettings& settings, int idFile)
{
	CStdString strSQL;
	bool retValue = false;
	try
	{
		CStdString strSettings;
		if (idFile < 0) return false;
		if (NULL == m_pDB.get()) return false;
		if (NULL == m_pDS.get()) return false;

		strSQL = PrepareSQL("SELECT settings FROM dirents WHERE idDirent=%i", idFile);

		m_pDS->query(strSQL.c_str());
		if (!m_pDS->eof())
			strSettings = m_pDS->fv("settings").get_asString();
		m_pDS->close();

		if(!strSettings)
		{
			CLog::Log(LOGERROR, "Unable to load video settings for dirent %i", idFile);
			return false;
		}

		retValue = ParseVideoSettings(strSettings, settings);

	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s unable to getvideosettings (%s)", __FUNCTION__, strSQL.c_str());
	}


	return retValue;
}

bool CObjectDatabase::GetVideoSettings(CObjectInfoTag& tag)
{
	bool retVal = false;
	try
	{
		if (NULL == m_pDB.get()) return false;
		if (NULL == m_pDS.get()) return false;



		CStdString strSQL=PrepareSQL("select dSettings from viewObjectDirentAll where oID=%i", tag.m_idObject);

		m_pDS->query(strSQL.c_str());
		if (m_pDS->num_rows() > 0)
		{
			CStdString settingsXML = m_pDS->fv("dSettings").get_asString();
			if(!settingsXML.IsEmpty())
				retVal = ParseVideoSettings(settingsXML, tag.m_settings);
		}
		m_pDS->close();
	}
	catch (...)
	{

	}

	return retVal;
}

bool CObjectDatabase::ParseVideoSettings(CStdString xml, CVideoSettings& settings)
{
	CXBMCTinyXML settingsXML;

	settingsXML.Parse(xml);

	TiXmlNode *root = settingsXML.RootElement();


	//CLog::Log(LOGNOTICE, "Root node has value %s", root->ValueStr().c_str());
	if (root && strcmp(root->ValueStr().c_str(), "settings") == 0)
	{
		TiXmlPrinter printer;
		printer.SetLineBreak("\n");
		printer.SetIndent("  ");
		settingsXML.Accept(&printer);

		XMLUtils::GetInt(root, "viewmode", settings.m_ViewMode);
		XMLUtils::GetFloat(root, "zoomamount", settings.m_CustomZoomAmount);
		XMLUtils::GetFloat(root, "pixelratio", settings.m_CustomPixelRatio);
		XMLUtils::GetInt(root, "audiostream", settings.m_AudioStream);
		XMLUtils::GetInt(root, "subtitlestream", settings.m_SubtitleStream);
		XMLUtils::GetFloat(root, "subtitledelay", settings.m_SubtitleDelay);
		XMLUtils::GetBoolean(root, "subtitleson", settings.m_SubtitleOn);
		XMLUtils::GetFloat(root, "brightness", settings.m_Brightness);
		XMLUtils::GetFloat(root, "contrast", settings.m_Contrast);
		XMLUtils::GetFloat(root, "gamma", settings.m_Gamma);
		XMLUtils::GetFloat(root, "volumeamplification", settings.m_VolumeAmplification);
		XMLUtils::GetFloat(root, "audiodelay", settings.m_AudioDelay);
		XMLUtils::GetBoolean(root, "outputtoallspeakers", settings.m_OutputToAllSpeakers);
		XMLUtils::GetBoolean(root, "crop", settings.m_Crop);
		XMLUtils::GetInt(root, "cropbottom", settings.m_CropBottom);
		XMLUtils::GetInt(root, "croptop", settings.m_CropTop);
		XMLUtils::GetInt(root, "cropleft", settings.m_CropLeft);
		XMLUtils::GetInt(root, "cropright", settings.m_CropRight);
		XMLUtils::GetFloat(root, "sharpness", settings.m_Sharpness);
		XMLUtils::GetFloat(root, "noisereduction", settings.m_NoiseReduction);
		XMLUtils::GetBoolean(root, "nonlinstretch", settings.m_CustomNonLinStretch);
		XMLUtils::GetFloat(root, "verticalshift", settings.m_CustomVerticalShift);

		int deinterlacemode;
		XMLUtils::GetInt(root, "deinterlacemode", deinterlacemode);
		settings.m_DeinterlaceMode = (EDEINTERLACEMODE)deinterlacemode;

		int interlace;
		XMLUtils::GetInt(root, "interlacemethod", interlace);
		settings.m_InterlaceMethod = (EINTERLACEMETHOD)interlace;

		int scaling;
		XMLUtils::GetInt(root, "scalingmethod", scaling);
		settings.m_ScalingMethod = (ESCALINGMETHOD)scaling;

		settings.m_SubtitleCached = false;

		return true;
	}
	else
	{
		CLog::Log(LOGERROR, "Improperly formatted XML %s", xml.c_str());
		return false;
	}
}

void CObjectDatabase::EraseVideoSettings()
{
	try
	{
		CLog::Log(LOGINFO, "Deleting settings information for all movies");
		m_pDS->exec("update dirents set settings=NULL");
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s failed", __FUNCTION__);
	}
}

int CObjectDatabase::GetPlayCount(const int idObject, const int idProfile)
{
	if (idObject < 0 || idProfile < 0)
		return 0;  // not in db, so not watched

	try
	{
		// error!
		if (NULL == m_pDB.get()) return -1;
		if (NULL == m_pDS.get()) return -1;

		CStdString strSQL = PrepareSQL("select playCount from settings WHERE idObject=%i AND idProfile=%i", idObject, idProfile);
		int count = 0;
		if (m_pDS->query(strSQL.c_str()))
		{
			// there should only ever be one row returned
			if (m_pDS->num_rows() == 1)
				count = m_pDS->fv(0).get_asInt();
			m_pDS->close();
		}
		return count;
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s failed", __FUNCTION__);
	}
	return -1;
}


CDateTime CObjectDatabase::GetLastPlayed(const int idObject, const int idProfile)
{
	CDateTime lastPlayed;
	if (idObject < 0 || idProfile < 0)
		return 0;  // not in db, so not watched

	try
	{
		// error!
		if (NULL == m_pDB.get()) return -1;
		if (NULL == m_pDS.get()) return -1;

		CStdString strSQL = PrepareSQL("select lastPlayed from settings WHERE idObject=%i AND idProfile=%i", idObject, idProfile);

		if (m_pDS->query(strSQL.c_str()))
		{
			// there should only ever be one row returned
			if (m_pDS->num_rows() == 1)
				lastPlayed.SetFromDBDateTime(m_pDS->fv(0).get_asString());
			m_pDS->close();
		}
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s failed", __FUNCTION__);
	}

	return lastPlayed;
}

void CObjectDatabase::SetPlayCount(const int idObject, const int idProfile, int count, const CDateTime &date)
{
	if (idObject < 0 || idProfile < 0)
		return;

	// and mark as watched
	try
	{
		if (NULL == m_pDB.get()) return ;
		if (NULL == m_pDS.get()) return ;

		CStdString strSQL;
		if (count)
		{
			if (!date.IsValid())
				strSQL = PrepareSQL("update settings set playCount=%i,lastPlayed='%s' where idObject=%i AND idProfile=%i", count, CDateTime::GetCurrentDateTime().GetAsDBDateTime().c_str(), idObject, idProfile);
			else
				strSQL = PrepareSQL("update settings set playCount=%i,lastPlayed='%s' where idObject=%i AND idProfile=%i", count, date.GetAsDBDateTime().c_str(), idObject, idProfile);
		}
		else
		{
			if (!date.IsValid())
				strSQL = PrepareSQL("update settings set playCount=NULL,lastPlayed=NULL where idObject=%i and idProfile=%i", idObject, idProfile);
			else
				strSQL = PrepareSQL("update settings set playCount=NULL,lastPlayed='%s' where idObject=%i and idProfile=%i", date.GetAsDBDateTime().c_str(), idObject, idProfile);
		}

		m_pDS->exec(strSQL.c_str());

		// We only need to announce changes to video items in the library
		//    if (item.HasVideoInfoTag() && item.GetVideoInfoTag()->m_iDbId > 0)
		//    {
		//      // Only provide the "playcount" value if it has actually changed
		//      if (item.GetVideoInfoTag()->m_playCount != count)
		//      {
		//        CVariant data;
		//        data["playcount"] = count;
		//        ANNOUNCEMENT::CAnnouncementManager::Announce(ANNOUNCEMENT::VideoLibrary, "xbmc", "OnUpdate", CFileItemPtr(new CFileItem(item)), data);
		//      }
		//      else
		//        ANNOUNCEMENT::CAnnouncementManager::Announce(ANNOUNCEMENT::VideoLibrary, "xbmc", "OnUpdate", CFileItemPtr(new CFileItem(item)));
		//    }
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s failed", __FUNCTION__);
	}
}

void CObjectDatabase::IncrementPlayCount(const int idObject, const int idProfile)
{
	SetPlayCount(idObject, idProfile, GetPlayCount(idObject, idProfile) + 1);
}

void CObjectDatabase::UpdateLastPlayed(const int idObject, const int idProfile)
{
	SetPlayCount(idObject, idProfile, GetPlayCount(idObject, idProfile), CDateTime::GetCurrentDateTime());
}

bool CObjectDatabase::HasContent(const int idObjectType)
{
	return GetObjectTypeCount(idObjectType) > 0;
}

int CObjectDatabase::GetObjectTypeCount(const int idObjectType)
{
	int result = 0;
	try
	{
		if (NULL == m_pDB.get()) return false;
		if (NULL == m_pDS.get()) return false;

		CStdString sql = PrepareSQL("select count(1) from objects where idObjectType=%i", idObjectType);
		m_pDS->query( sql.c_str() );

		if (!m_pDS->eof())
			result = m_pDS->fv(0).get_asInt();

		m_pDS->close();
	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s failed", __FUNCTION__);
	}
	return result;
}

CStdString CObjectDatabase::LoadFile(CStdString &path)
{
	std::ifstream ifs(path);
	std::string content( (std::istreambuf_iterator<char>(ifs) ),
	                       (std::istreambuf_iterator<char>()    ) );

	return content;
}

void CObjectDatabase::ImportVideoFromXML(CStdString &path)
{
	try
	{
		if (NULL == m_pDB.get()) return;
		if (NULL == m_pDS.get()) return;

		CLog::Log(LOGINFO, "Beginning import");

		CXBMCTinyXML xmlDoc;
		CStdString xmlPath = URIUtils::AddFileToFolder(path, "videodb.xml");
		CStdString xml = LoadFile(xmlPath);
		if (!xmlDoc.Parse(xml))
			return;

		TiXmlElement *root = xmlDoc.RootElement();
		if (!root) return;

		int iVersion = 0;
		    XMLUtils::GetInt(root, "version", iVersion);

		    CLog::Log(LOGDEBUG, "%s: Starting import (export version = %i)", __FUNCTION__, iVersion);

		    TiXmlElement *movie = root->FirstChildElement();
		    int current = 0;
		    int total = 0;
		    // first count the number of items...
		    while (movie)
		    {
		      if (strnicmp(movie->Value(), "movie", 5)==0 ||
		          strnicmp(movie->Value(), "tvshow", 6)==0 ||
		          strnicmp(movie->Value(), "musicvideo",10)==0 )
		        total++;
		      movie = movie->NextSiblingElement();
		    }

		    CLog::Log(LOGINFO, "Adding %i videos...", total);

		    CStdString actorsDir(URIUtils::AddFileToFolder(path, "actors"));
		    CStdString moviesDir(URIUtils::AddFileToFolder(path, "movies"));
		    CStdString musicvideosDir(URIUtils::AddFileToFolder(path, "musicvideos"));
		    CStdString tvshowsDir(URIUtils::AddFileToFolder(path, "tvshows"));

		    TiXmlElement *path = root->FirstChildElement("paths");
		    path = path->FirstChildElement();
		    while (path)
		    {
		    	CStdString strPath;
		    	int idPath = -1;
		    	if (XMLUtils::GetString(path,"url",strPath))
		    		idPath = AddPath(strPath);

		    	CStdString content;
		    	if (XMLUtils::GetString(path,"content", content))
		    	{ // check the scraper exists, if so store the path
		    		CStdString id;
		    		XMLUtils::GetString(path,"scraperpath",id);
		    		int recurse;
		    		XMLUtils::GetInt(path,"scanrecursive",recurse);
		    		bool parent_name;
		    		XMLUtils::GetBoolean(path,"usefoldernames",parent_name);
		    		int idScraper = AddScraper(id, content, "", recurse, parent_name, false);

		    		LinkScraperToPath(idScraper, idPath);

		    	}
		    	path = path->NextSiblingElement();
		    }
		    movie = root->FirstChildElement();
		    while (movie)
		    {
		    	CLog::Log(LOGINFO, "Adding video #%i...", ++current);
		    	CVideoInfoTag info;
		    	if(strnicmp(movie->Value(), "movie", 5) == 0)
		    	{
		    		info.Load(movie);
		    		int idMovie = AddObject(OBJ_MOVIE, info.m_strTitle, info.m_strTitle);
		    		SetAttribute(idMovie, MOVIE_PLOT_STR, info.m_strPlot);
		    		SetAttribute(idMovie, CONTENT_ORIGINALTITLE_STR, info.m_strOriginalTitle);
		    		SetAttribute(idMovie, ONLINERATING_NUM, info.m_fRating);
		    		SetAttribute(idMovie, MOVIE_RANKING_NUM, info.m_iTop250);
		    		SetAttribute(idMovie, VOTES_STR, info.m_strVotes);
		    		SetAttribute(idMovie, MOVIE_PLOTOUTLINE_STR, info.m_strPlotOutline);
		    		SetAttribute(idMovie, TAGLINE_STR, info.m_strTagLine);
		    		SetAttribute(idMovie, FILENAME_STR, info.m_strFile);
		    		SetAttribute(idMovie, CONTENTRATING_STR, info.m_strMPAARating);
		    		SetAttribute(idMovie, ONLINEID_STR, info.m_strIMDBNumber);
		    		SetAttribute(idMovie, MOVIE_TRAILER_URL_STR, info.m_strTrailer);

		    		int idDirEnt = AddDirEnt(info.m_strFileNameAndPath);
		    		LinkObjectToDirent(idMovie, idDirEnt);

		    		if(info.m_strSet)
		    		{
		    			int idSet = AddObject(OBJ_MOVIESET, info.m_strSet, info.m_strSet);
		    			LinkObjectToObject(MOVIESET_HAS_MOVIE, idSet, idMovie);
		    		}

		    		CStdString strYear;
		    		strYear.Format("%i", info.m_iYear);
		    		int idYear = AddObject(OBJ_YEAR, strYear, strYear);
		    		LinkObjectToObject(CONTENT_HAS_YEAR, idMovie, idYear);

		    		for(vector<string>::const_iterator i = info.m_genre.begin(); i != info.m_genre.end(); ++i)
		    		{
		    			int idGenre = AddObject(OBJ_GENRE, *i, *i);
		    			LinkObjectToObject(VIDEO_HAS_GENRE, idMovie, idGenre);
		    		}

		    		for(vector<string>::const_iterator i = info.m_country.begin(); i != info.m_country.end(); ++i)
		    		{
		    			int idCountry = AddObject(OBJ_COUNTRY, *i, *i);
		    			LinkObjectToObject(MOVIE_HAS_COUNTRY, idMovie, idCountry);
		    		}

		    		for(vector<string>::const_iterator i = info.m_director.begin(); i != info.m_director.end(); ++i)
		    		{
		    			int idDirector = AddObject(OBJ_DIRECTOR, *i, *i);
		    			LinkObjectToObject(VIDEO_HAS_DIRECTOR, idMovie, idDirector);
		    		}

		    		for(vector<string>::const_iterator i = info.m_writingCredits.begin(); i != info.m_writingCredits.end(); ++i)
		    		{
		    			int idWriter = AddObject(OBJ_WRITER, *i, *i);
		    			LinkObjectToObject(MOVIE_HAS_WRITER, idMovie, idWriter);
		    		}

		    		for(vector<SActorInfo>::const_iterator i = info.m_cast.begin(); i != info.m_cast.end(); ++i)
		    		{
		    			int idActor = AddObject(OBJ_ACTOR, i->strName, i->strName);
		    			LinkObjectToObject(MOVIE_HAS_ACTOR, idMovie, idActor, i->strRole);
		    		}

		    		for(vector<string>::const_iterator i = info.m_tags.begin(); i != info.m_tags.end(); ++i)
		    		{
		    			int idTag = AddObject(OBJ_TAG, *i, *i);
		    			LinkObjectToObject(OBJECT_HAS_TAG, idMovie, idTag);
		    		}

		    		for(vector<string>::const_iterator i = info.m_studio.begin(); i != info.m_studio.end(); ++i)
		    		{
		    			int idStudio = AddObject(OBJ_STUDIO, *i, *i);
		    			LinkObjectToObject(VIDEO_HAS_STUDIO, idMovie, idStudio);
		    		}

		    		SetStreamDetailsForFileId(info.m_streamDetails, idDirEnt);
		    		UpdateDirentDateAdded(idDirEnt, info.m_dateAdded);

		    	}
		    	else if (strnicmp(movie->Value(), "tvshow", 6) == 0)
		    	{
		    		info.Load(movie);
		    		URIUtils::AddSlashAtEnd(info.m_strPath);

		    		int idTvShow = AddObject(OBJ_TVSHOW, info.m_strTitle, info.m_strTitle);
		    		SetAttribute(idTvShow, CONTENT_ORIGINALTITLE_STR, info.m_strOriginalTitle);
		    		SetAttribute(idTvShow, ONLINERATING_NUM, info.m_fRating);
		    		SetAttribute(idTvShow, VOTES_STR, info.m_strVotes);
		    		SetAttribute(idTvShow, TVSHOW_STATUS_STR, info.m_strStatus);
		    		SetAttribute(idTvShow, TAGLINE_STR, info.m_strTagLine);
		    		SetAttribute(idTvShow, FILENAME_STR, info.m_strFile);
		    		SetAttribute(idTvShow, CONTENTRATING_STR, info.m_strMPAARating);
		    		SetAttribute(idTvShow, ONLINEID_STR, info.m_strIMDBNumber);
		    		SetAttribute(idTvShow, TVSHOW_EPISODEGUIDE_STR, info.m_strEpisodeGuide);


		    		int idDirEnt = AddDirEnt(info.m_strFileNameAndPath);
		    		LinkObjectToDirent(idTvShow, idDirEnt);

		    		CStdString strYear;
		    		strYear.Format("%i", info.m_iYear);
		    		int idYear = AddObject(OBJ_YEAR, strYear, strYear);
		    		LinkObjectToObject(CONTENT_HAS_YEAR, idTvShow, idYear);

		    		for(vector<string>::const_iterator i = info.m_genre.begin(); i != info.m_genre.end(); ++i)
		    		{
		    			int idGenre = AddObject(OBJ_GENRE, *i, *i);
		    			LinkObjectToObject(VIDEO_HAS_GENRE, idTvShow, idGenre);
		    		}

		    		for(vector<string>::const_iterator i = info.m_director.begin(); i != info.m_director.end(); ++i)
		    		{
		    			int idDirector = AddObject(OBJ_DIRECTOR, *i, *i);
		    			LinkObjectToObject(VIDEO_HAS_DIRECTOR, idTvShow, idDirector);
		    		}


		    		for(vector<SActorInfo>::const_iterator i = info.m_cast.begin(); i != info.m_cast.end(); ++i)
		    		{
		    			int idActor = AddObject(OBJ_ACTOR, i->strName, i->strName);
		    			LinkObjectToObject(TVSHOW_HAS_ACTOR, idTvShow, idActor, i->strRole);
		    		}

		    		for(vector<string>::const_iterator i = info.m_tags.begin(); i != info.m_tags.end(); ++i)
		    		{
		    			int idTag = AddObject(OBJ_TAG, *i, *i);
		    			LinkObjectToObject(OBJECT_HAS_TAG, idTvShow, idTag);
		    		}

		    		for(vector<string>::const_iterator i = info.m_studio.begin(); i != info.m_studio.end(); ++i)
		    		{
		    			int idStudio = AddObject(OBJ_STUDIO, *i, *i);
		    			LinkObjectToObject(VIDEO_HAS_STUDIO, idTvShow, idStudio);
		    		}

		    		SetStreamDetailsForFileId(info.m_streamDetails, idDirEnt);
		    		UpdateDirentDateAdded(idDirEnt, info.m_dateAdded);

		    		// now load the episodes
		    		TiXmlElement *episode = movie->FirstChildElement("episodedetails");
		    		while (episode)
		    		{
		    			CVideoInfoTag info;
		    			info.Load(episode);

		    			int season = info.m_iSeason;
		    			CStdString seasonStub;
		    			seasonStub.AppendFormat("%d-%d", idTvShow, season);
		    			int idSeason = AddObject(OBJ_SEASON, seasonStub, seasonStub);
		    			if(LinkObjectToObject(TVSHOW_HAS_SEASON, idTvShow, idSeason,"", season) >= 0)
		    			{
		    				CStdString episodeStub;
		    				episodeStub.AppendFormat("%d-%dx%02d", idTvShow, season, info.m_iEpisode);
		    				int idEpisode = AddObject(OBJ_EPISODE, episodeStub, info.m_strTitle);
		    				LinkObjectToObject(SEASON_HAS_EPISODE, idSeason, idEpisode, "", info.m_iEpisode);
		    				SetAttribute(idEpisode, EPISODE_PLOT_STR, info.m_strPlot);
		    				SetAttribute(idEpisode, EPISODE_PRODUCTIONCODE_STR, info.m_strProductionCode);
		    				SetAttribute(idEpisode, EPISODE_SEASONSORT_NUM, info.m_iSpecialSortSeason);
		    				SetAttribute(idEpisode, RELEASEDATE_STR, info.m_firstAired.GetAsDBDate());

		    				int idDirEnt = AddDirEnt(info.m_strFileNameAndPath);
		    				LinkObjectToDirent(idEpisode, idDirEnt);

		    				for(vector<string>::const_iterator i = info.m_genre.begin(); i != info.m_genre.end(); ++i)
		    				{
		    					int idGenre = AddObject(OBJ_GENRE, *i, *i);
		    					LinkObjectToObject(VIDEO_HAS_GENRE, idEpisode, idGenre);
		    				}

		    				for(vector<string>::const_iterator i = info.m_director.begin(); i != info.m_director.end(); ++i)
		    				{
		    					int idDirector = AddObject(OBJ_DIRECTOR, *i, *i);
		    					LinkObjectToObject(VIDEO_HAS_DIRECTOR, idEpisode, idDirector);
		    				}

		    				for(vector<SActorInfo>::const_iterator i = info.m_cast.begin(); i != info.m_cast.end(); ++i)
		    				{
		    					int idActor = AddObject(OBJ_ACTOR, i->strName, i->strName);
		    					LinkObjectToObject(EPISODE_HAS_ACTOR, idEpisode, idActor, i->strRole);
		    				}

		    				for(vector<string>::const_iterator i = info.m_writingCredits.begin(); i != info.m_writingCredits.end(); ++i)
		    				{
		    					int idWriter = AddObject(OBJ_WRITER, *i, *i);
		    					LinkObjectToObject(EPISODE_HAS_WRITER, idEpisode, idWriter);
		    				}

		    				for(vector<string>::const_iterator i = info.m_tags.begin(); i != info.m_tags.end(); ++i)
		    				{
		    					int idTag = AddObject(OBJ_TAG, *i, *i);
		    					LinkObjectToObject(OBJECT_HAS_TAG, idEpisode, idTag);
		    				}

		    				for(vector<string>::const_iterator i = info.m_studio.begin(); i != info.m_studio.end(); ++i)
		    				{
		    					int idStudio = AddObject(OBJ_STUDIO, *i, *i);
		    					LinkObjectToObject(VIDEO_HAS_STUDIO, idEpisode, idStudio);
		    				}

		    				SetStreamDetailsForFileId(info.m_streamDetails, idDirEnt);
		    				UpdateDirentDateAdded(idDirEnt, info.m_dateAdded);


		    			}


		    			episode = episode->NextSiblingElement("episodedetails");
		    		}
		    	}

		    	movie = movie->NextSiblingElement();
		    }

		    CLog::Log(LOGINFO, "Finished import");



	}
	catch (...)
	{
		CLog::Log(LOGERROR, "%s failed", __FUNCTION__);
	}
}

