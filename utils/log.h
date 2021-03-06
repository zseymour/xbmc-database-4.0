/*
 * log.h
 *
 *  Created on: Jun 19, 2013
 *      Author: zachary
 */

#ifndef LOG_H_
#define LOG_H_

#include "StdString.h"

#define LOG_LEVEL_NONE         -1 // nothing at all is logged
#define LOG_LEVEL_NORMAL        0 // shows notice, error, severe and fatal
#define LOG_LEVEL_DEBUG         1 // shows all
#define LOG_LEVEL_DEBUG_FREEMEM 2 // shows all + shows freemem on screen
#define LOG_LEVEL_MAX           LOG_LEVEL_DEBUG_FREEMEM

// ones we use in the code
#define LOGDEBUG   0
#define LOGINFO    1
#define LOGNOTICE  2
#define LOGWARNING 3
#define LOGERROR   4
#define LOGSEVERE  5
#define LOGFATAL   6
#define LOGNONE    7

class CLog {
public:
	CLog();
	virtual ~CLog();
	static void Log(int loglevel, const char *format, ... );
};

#endif /* LOG_H_ */
