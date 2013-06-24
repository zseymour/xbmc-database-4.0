/*
 * Util.h
 *
 *  Created on: Jun 24, 2013
 *      Author: zachary
 */

#ifndef UTIL_H_
#define UTIL_H_

#include "utils/StdString.h"

class CUtil {
public:
	CUtil(void);
	virtual ~CUtil(void);
	static CStdString ValidatePath(const CStdString &path, bool bFixDoubleSlashes /* = false */);
};

#endif /* UTIL_H_ */
