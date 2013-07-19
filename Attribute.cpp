/*
 * Attribute.cpp
 *
 *  Created on: Jul 19, 2013
 *      Author: zachary
 */

#include "Attribute.h"
#include <tgmath.h>

CAttribute::CAttribute() {
	// TODO Auto-generated constructor stub

}

CAttribute::CAttribute(CAttributeType attributeType) {
	type = attributeType;
}

CAttribute::~CAttribute() {
	// TODO Auto-generated destructor stub
}


CStdString CAttribute::getStringValue()
{
	CStdString returnValue = "";
	switch (type.type) {
		case STRING_ATTRIBUTE:
			returnValue = strValue;
			break;
		case NUMBER_ATTRIBUTE:
			if(type.precision == 0)
				returnValue.Format("%i", intValue);
			else
				returnValue.Format("%.2f", getNumericValue());
			break;
		default:
			break;
	}

	return returnValue;
}

float CAttribute::getNumericValue()
{
	float returnValue = 0;
	switch(type.type) {
	case NUMBER_ATTRIBUTE:
		returnValue = intValue * pow(10, type.precision);
		break;
	default:
		break;
	}

	return returnValue;

}

int CAttribute::getRawIntValue()
{
	return intValue;
}

CStdString CAttribute::getName()
{
	return type.name;
}

ATTRIBUTE_DATA_TYPE CAttribute::getType()
{
	return type.type;
}

void CAttribute::setStringValue(CStdString newValue)
{
	strValue = newValue;
}

void CAttribute::setType(CAttributeType newType)
{
	type = newType;
}

void CAttribute::setNumericValue(int newValue)
{
	intValue = newValue;
}

void CAttribute::setNumericValue(float newValue)
{
	if(newValue >= 0)
		intValue = (int) (newValue * pow(10, -type.precision) + 0.5);
	else
		intValue = (int) (newValue * pow(10, -type.precision) - 0.5);
}

