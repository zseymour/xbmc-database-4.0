/*
 * mysystem.cpp
 *
 *  Created on: Jul 10, 2013
 *      Author: zachary
 */

#include "mysystem.h"

DWORD GetTimeZoneInformation( LPTIME_ZONE_INFORMATION lpTimeZoneInformation )
{
  if (lpTimeZoneInformation == NULL)
    return TIME_ZONE_ID_INVALID;

  memset(lpTimeZoneInformation, 0, sizeof(TIME_ZONE_INFORMATION));

  struct tm t;
  time_t tt = time(NULL);
  if(localtime_r(&tt, &t))
    lpTimeZoneInformation->Bias = -t.tm_gmtoff / 60;

  swprintf(lpTimeZoneInformation->StandardName, 31, L"%s", tzname[0]);
  swprintf(lpTimeZoneInformation->DaylightName, 31, L"%s", tzname[1]);

  return TIME_ZONE_ID_UNKNOWN;
}

LONGLONG Int32x32To64(LONG Multiplier, LONG Multiplicand)
{
  LONGLONG result = Multiplier;
  result *= Multiplicand;
  return result;
}






