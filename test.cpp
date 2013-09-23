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
#include "ObjectInfoTag.h"
#include <stdio.h>
#include <sqlite3.h>
#include "utils/StdString.h"
#include "utils/StringUtils.h"
#include <algorithm>

using namespace std;



char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

int main(int argc, char * argv[])
{

	if(argc==1 || cmdOptionExists(argv, argv+argc, "-h"))
	{
		cout << "Usage: " << argv[0] << " --db /path/to/MyObject4.db  [--xml /path/to/(video|music)db.xml] [--query (video|music)db://query/url]" << endl;
		return 0;
	}
	CStdString host;
	char * dbBase = getCmdOption(argv, argv+argc, "--db");

	if(dbBase)
	{
		host = CStdString(dbBase);
	}
	else
	{
		cout << "Error: --db option required.  See help (-h) for more." << endl;
		return 1;
	}
	//CStdString file = "database4.db";
	DatabaseSettings settings;
	settings.host = host;
	settings.name = "MyObject";
	settings.type = "sqlite3";


	CDatabaseManager::Get().Initialize(false,&settings);



	CObjectDatabase db(settings);
	if(db.Open())
	{
		char * pathToXml = getCmdOption(argv, argv+argc, "--xml");
		if(pathToXml)
		{
			CStdString path(pathToXml);

			db.ImportVideoFromXML(path);
		}

		char * queryString = getCmdOption(argv, argv+argc, "--query");

		if(queryString)
		{
			CStdString url(queryString);
			CStdStringArray results;
			db.GetObjectsByUrl(url, results);
			cout << "Database Query: " << url << endl;
			cout << "---------------------------------------------------------" << endl;
			for(CStdStringArray::iterator it = results.begin(); it != results.end(); ++it)
			{
				cout << *it << endl;
			}
		}


	}
	else
	{
		cout << "Database failed to open." << endl;
	}


	return 0;
}



