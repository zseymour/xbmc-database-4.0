/*
 * test.cpp
 *
 *  Created on: Jun 17, 2013
 *      Author: zachary
 */
#include <iostream>
#include "DatabaseManager.h"
#include "ObjectDatabase.h"
#include "dbwrappers/Database.h"
#include <stdio.h>
#include <sqlite3.h>
#include "utils/StdString.h"
#include "utils/StringUtils.h"
using namespace std;

int main()
{
	CStdString host = "/home/zachary/workspace/xbmc-database-4.0/Debug";
	//CStdString file = "database4.db";
	DatabaseSettings settings;
	settings.host = host;
	settings.name = "database";
	settings.type = "sqlite3";

	CDatabaseManager::Get().Initialize(false,&settings);

	CObjectDatabase db;
	if(db.Open())
	{
		CStdString path = "/home/zach/movies/";
		int pathId = db.AddPath(path);
		int scraperId = db.AddScraper("scraper.movie","movies");
		db.LinkScraperToPath(scraperId, pathId);

		CStdString filename = "/home/zach/movies/Brave.mkv";
		int idDirEnt = db.AddDirEnt(filename);
		int idObjectType = db.GetObjectTypeId("movie");
		int idObject = db.AddObject(idObjectType, "brave", "Brave");
		db.LinkObjectToDirent(idObject, idDirEnt);

		CObjectDatabase::CAttribute fileAttr;
		fileAttr.strValue = filename;
		CObjectDatabase::CAttribute onlineRatingAttr;
		onlineRatingAttr.intValue = 98;
		CObjectDatabase::CAttribute contentRatingAttr;
		contentRatingAttr.strValue = "PG-13";
		CObjectDatabase::CAttribute votesAttr;
		votesAttr.intValue = 31134;

		map<CStdString, CObjectDatabase::CAttribute> attributes;
		attributes["filename"] = fileAttr;
		attributes["onlinerating"] = onlineRatingAttr;
		attributes["contentrating"] = contentRatingAttr;
		attributes["votes"] = votesAttr;

		db.AddAttributesForObject(idObject, attributes);

	}

//	sqlite3 *db;
//
//	int rc;
//	rc = sqlite3_open(host + "/" + file, &db);
//	if( rc ){
//		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
//		sqlite3_close(db);
//		return(1);
//	}

}



