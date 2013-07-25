/*
 * test.cpp
 *
 *  Created on: Jun 17, 2013
 *      Author: zachary
 */

#define TARGET_POSIX 1

#include <iostream>
#include "DatabaseManager.h"
#include "ObjectDatabase.h"
#include "dbwrappers/Database.h"
#include <stdio.h>
#include <sqlite3.h>
#include "utils/StdString.h"
#include "utils/StringUtils.h"
#include "TestObjects.h"

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
		TestObjects::InsertTestMovie(db);

	}
	else
	{
		cout << "Database failed to open." << endl;
	}



}



