/*
 * mysystem.h
 *
 *  Created on: Jul 10, 2013
 *      Author: zachary
 */

#ifndef MYSYSTEM_H_
#define MYSYSTEM_H_

#include <cstdlib>
#include <cstdint>
#include <time.h>
#include <cstring>
#include <cwchar>


#define stricmp strcasecmp
#define strnicmp strncasecmp

#define FALSE   0
#define TRUE    1

#define VOID    void
#define __int8    char
#define __int16   short
#define __int32   int
#define __int64   long long
#define __uint64  unsigned long long
typedef unsigned int  DWORD;
typedef unsigned short  WORD;
typedef unsigned char   BYTE;
typedef char        CHAR;
typedef unsigned char UCHAR;
typedef wchar_t     WCHAR;
typedef int         BOOL;
typedef BYTE        BOOLEAN;
typedef short       SHORT;
typedef unsigned short  USHORT;
typedef int         INT;
typedef unsigned int  UINT;
// typedef int INT32;              // unused; must match Xmd.h but why bother
typedef unsigned int  UINT32;
typedef long long     INT64;
typedef unsigned long long    UINT64;
typedef long        LONG;
typedef long long     LONGLONG;
typedef float         FLOAT;
typedef size_t        SIZE_T;
typedef void*         PVOID;
typedef void*         LPVOID;
//typedef PVOID         HANDLE;
#define INVALID_HANDLE_VALUE     ((HANDLE)~0U)
typedef void*       HWND;
#if defined(TARGET_DARWIN_OSX)
typedef SInt32      HRESULT;
#else
typedef LONG        HRESULT;
#endif
typedef BYTE*       LPBYTE;
typedef DWORD*        LPDWORD;
typedef CHAR*     LPTSTR;
typedef WCHAR         *PWSTR,      *LPWSTR,    *NWPSTR;
typedef CHAR            *PSTR,       *LPSTR,     *NPSTR;
typedef LONG        *PLONG, *LPLONG;
typedef unsigned __int64 ULONGLONG;
typedef long        LONG_PTR;
typedef unsigned long   ULONG_PTR;
typedef ULONG_PTR     DWORD_PTR;
typedef __int64     __time64_t;

typedef union _LARGE_INTEGER
{
	struct {
		DWORD LowPart;
		int32_t HighPart;
	} u;
	LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;

typedef union _ULARGE_INTEGER {
	struct {
		DWORD LowPart;
		DWORD HighPart;
	} u;
	ULONGLONG QuadPart;
} ULARGE_INTEGER, *PULARGE_INTEGER;

typedef struct _SYSTEMTIME
{
  WORD wYear;
  WORD wMonth;
  WORD wDayOfWeek;
  WORD wDay;
  WORD wHour;
  WORD wMinute;
  WORD wSecond;
  WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

typedef struct _FILETIME
{
  DWORD dwLowDateTime;
  DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;

typedef struct _TIME_ZONE_INFORMATION {
  LONG Bias;
  WCHAR StandardName[32];
  SYSTEMTIME StandardDate;
  LONG StandardBias;
  WCHAR DaylightName[32];
  SYSTEMTIME DaylightDate;
  LONG DaylightBias;
} TIME_ZONE_INFORMATION, *PTIME_ZONE_INFORMATION, *LPTIME_ZONE_INFORMATION;

#define TIME_ZONE_ID_INVALID    ((DWORD)0xFFFFFFFF)
#define TIME_ZONE_ID_UNKNOWN    0
#define TIME_ZONE_ID_STANDARD   1
#define TIME_ZONE_ID_DAYLIGHT   2


#if defined(__x86_64__) || defined(__powerpc__) || defined(__ppc__) || defined (__arm__) // should this be powerpc64 only?
#define __stdcall
#else /* !__x86_64__ */
#define __stdcall   __attribute__((__stdcall__))
#endif /* __x86_64__ */
#define __cdecl
#define WINBASEAPI
#define NTAPI       __stdcall
#define CALLBACK    __stdcall
#define WINAPI      __stdcall
#define WINAPIV     __cdecl
#define CopyMemory(dst,src,size) memmove(dst, src, size)
#define ZeroMemory(dst,size) memset(dst, 0, size)


DWORD GetTimeZoneInformation( LPTIME_ZONE_INFORMATION lpTimeZoneInformation );
LONGLONG Int32x32To64(LONG Multiplier, LONG Multiplicand);

#endif /* MYSYSTEM_H_ */
