/*
 * Util.cpp
 *
 *  Created on: Jun 24, 2013
 *      Author: zachary
 */

#include "Util.h"
#include "utils/URIUtils.h"

CStdString CUtil::ValidatePath(const CStdString &path, bool bFixDoubleSlashes /* = false */)
{
  CStdString result = path;

  // Don't do any stuff on URLs containing %-characters or protocols that embed
  // filenames. NOTE: Don't use IsInZip or IsInRar here since it will infinitely
  // recurse and crash XBMC
  if (URIUtils::IsURL(path) &&
     (path.Find('%') >= 0 ||
      path.Left(4).Equals("apk:") ||
      path.Left(4).Equals("zip:") ||
      path.Left(4).Equals("rar:") ||
      path.Left(6).Equals("stack:") ||
      path.Left(7).Equals("bluray:") ||
      path.Left(10).Equals("multipath:") ))
    return result;

  // check the path for incorrect slashes
#ifdef _WIN32
  if (URIUtils::IsDOSPath(path))
  {
    result.Replace('/', '\\');
    /* The double slash correction should only be used when *absolutely*
       necessary! This applies to certain DLLs or use from Python DLLs/scripts
       that incorrectly generate double (back) slashes.
    */
    if (bFixDoubleSlashes)
    {
      // Fixup for double back slashes (but ignore the \\ of unc-paths)
      for (int x = 1; x < result.GetLength() - 1; x++)
      {
        if (result[x] == '\\' && result[x+1] == '\\')
          result.Delete(x);
      }
    }
  }
  else if (path.Find("://") >= 0 || path.Find(":\\\\") >= 0)
#endif
  {
    result.Replace('\\', '/');
    /* The double slash correction should only be used when *absolutely*
       necessary! This applies to certain DLLs or use from Python DLLs/scripts
       that incorrectly generate double (back) slashes.
    */
    if (bFixDoubleSlashes)
    {
      // Fixup for double forward slashes(/) but don't touch the :// of URLs
      for (int x = 2; x < result.GetLength() - 1; x++)
      {
        if ( result[x] == '/' && result[x + 1] == '/' && !(result[x - 1] == ':' || (result[x - 1] == '/' && result[x - 2] == ':')) )
          result.Delete(x);
      }
    }
  }
  return result;
}

