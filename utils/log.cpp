/*
 * log.cpp
 *
 *  Created on: Jun 19, 2013
 *      Author: zachary
 */

#include "log.h"
#include <iostream>
#include <time.h>
#include <stdarg.h>

using namespace std;

static char levelNames[][8] =
{"DEBUG", "INFO", "NOTICE", "WARNING", "ERROR", "SEVERE", "FATAL", "NONE"};

CLog::CLog() {
	// TODO Auto-generated constructor stub

}

CLog::~CLog() {
	// TODO Auto-generated destructor stub
}

void CLog::Log(int loglevel, const char *format, ... )
{
	static const char* prefixFormat = "[%7s] %s ";
	CStdString strPrefix, strData;
	time_t rawtime;
	struct tm * timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strPrefix.Format(prefixFormat, levelNames[loglevel], asctime(timeinfo));

	strData.reserve(16384);
	va_list va;
	va_start(va, format);
	strData.FormatV(format,va);
	va_end(va);

	unsigned int length = 0;
	while ( length != strData.length() )
	{
		length = strData.length();
		strData.TrimRight(" ");
		strData.TrimRight('\n');
		strData.TrimRight("\r");
	}

	if (!length)
		return;

	cout << strPrefix << strData << endl;

}

