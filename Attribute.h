/*
 * Attribute.h
 *
 *  Created on: Jul 19, 2013
 *      Author: zachary
 */

#ifndef ATTRIBUTE_H_
#define ATTRIBUTE_H_

#include "utils/StdString.h"


typedef enum {
	STRING_ATTRIBUTE = 0,
	NUMBER_ATTRIBUTE = 1,
	BLOB_ATTRIBUTE = 2
} ATTRIBUTE_DATA_TYPE;

class CAttributeType
	{
	public:
		int idAttributeType;
		int idObjectType;
		CStdString stub;
		CStdString name;
		ATTRIBUTE_DATA_TYPE type;
		int precision;
		bool inheritable;
	};

class CAttribute {
public:
	CAttribute();
	CAttribute(CAttributeType type);
	virtual ~CAttribute();


	CStdString getStringValue();
	float getNumericValue();
	int getRawIntValue();
	CStdString getName();
	void setStringValue(CStdString newValue);
	void setNumericValue(float newValue);
	void setNumericValue(int newValue);
	void setType(CAttributeType newType);
	ATTRIBUTE_DATA_TYPE getDataType();
	const int getTypeID() const
	{
		return type.idAttributeType;
	};

	const bool isType(int idType) const
	{
		return type.idAttributeType == idType;
	};

	int idAttribute;
private:
	CAttributeType type;
	CStdString strValue;
	int intValue;
};

typedef std::vector<CAttribute> AttributeList;

#endif /* ATTRIBUTE_H_ */
