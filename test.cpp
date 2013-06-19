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
		vector<int> ids;
		if(db.GetAllAttributesForObjectType(16,ids))
		{
//			for(vector<int>::iterator it = ids.begin(); it != ids.end(); ++it)
//			{
//				cout << *it << endl;
//			}

			cout << StringUtils::Join(ids,",");
		}
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



