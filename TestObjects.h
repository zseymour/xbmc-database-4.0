/*
 * TestObjects.h
 *
 *  Created on: Jul 2, 2013
 *      Author: zachary
 */

#ifndef TESTOBJECTS_H_
#define TESTOBJECTS_H_

#include "ObjectDatabase.h"

class TestObjects {
public:

	TestObjects();
	virtual ~TestObjects();

	static void InsertTestMovie(CObjectDatabase& db);
	static void InsertTestTvShow(CObjectDatabase& db);
	static void InsertTestAlbum(CObjectDatabase& db);
};

#endif /* TESTOBJECTS_H_ */
