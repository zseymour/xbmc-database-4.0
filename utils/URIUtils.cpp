/*
 * URIUtils.cpp
 *
 *  Created on: Jun 24, 2013
 *      Author: zachary
 */

#include "URIUtils.h"
#include "StringUtils.h"
#include "../URL.h"
#include <cstring>

using namespace std;


CStdString URIUtils::GetParentFolderURI(const CStdString& uri, bool preserveFileNameInPath)
{
  if (preserveFileNameInPath)
    return AddFileToFolder(GetParentPath(uri), GetFileName(uri));
  else
    return GetParentPath(uri);
}

/* returns a filename given an url */
/* handles both / and \, and options in urls*/
const CStdString URIUtils::GetFileName(const CStdString& strFileNameAndPath)
{
  if(IsURL(strFileNameAndPath))
  {
    CURL url(strFileNameAndPath);
    return GetFileName(url.GetFileName());
  }

  /* find any slashes */
  const int slash1 = strFileNameAndPath.find_last_of('/');
  const int slash2 = strFileNameAndPath.find_last_of('\\');

  /* select the last one */
  int slash;
  if(slash2>slash1)
    slash = slash2;
  else
    slash = slash1;

  return strFileNameAndPath.substr(slash+1);
}

CStdString URIUtils::GetParentPath(const CStdString& strPath)
{
  CStdString strReturn;
  GetParentPath(strPath, strReturn);
  return strReturn;
}

bool URIUtils::GetParentPath(const CStdString& strPath, CStdString& strParent)
{
  strParent = "";

  CURL url(strPath);
  CStdString strFile = url.GetFileName();
  if ( URIUtils::ProtocolHasParentInHostname(url.GetProtocol()) && strFile.IsEmpty())
  {
    strFile = url.GetHostName();
    return GetParentPath(strFile, strParent);
  }
//  else if (url.GetProtocol() == "stack")
//  {
//    CStackDirectory dir;
//    CFileItemList items;
//    dir.GetDirectory(strPath,items);
//    GetDirectory(items[0]->GetPath(),items[0]->m_strDVDLabel);
//    if (items[0]->m_strDVDLabel.Mid(0,6).Equals("rar://") || items[0]->m_strDVDLabel.Mid(0,6).Equals("zip://"))
//      GetParentPath(items[0]->m_strDVDLabel, strParent);
//    else
//      strParent = items[0]->m_strDVDLabel;
//    for( int i=1;i<items.Size();++i)
//    {
//      GetDirectory(items[i]->GetPath(),items[i]->m_strDVDLabel);
//      if (items[0]->m_strDVDLabel.Mid(0,6).Equals("rar://") || items[0]->m_strDVDLabel.Mid(0,6).Equals("zip://"))
//        items[i]->SetPath(GetParentPath(items[i]->m_strDVDLabel));
//      else
//        items[i]->SetPath(items[i]->m_strDVDLabel);
//
//      GetCommonPath(strParent,items[i]->GetPath());
//    }
//    return true;
//  }
//  else if (url.GetProtocol() == "multipath")
//  {
//    // get the parent path of the first item
//    return GetParentPath(CMultiPathDirectory::GetFirstPath(strPath), strParent);
//  }
//  else if (url.GetProtocol() == "plugin")
//  {
//    if (!url.GetOptions().IsEmpty())
//    {
//      url.SetOptions("");
//      strParent = url.Get();
//      return true;
//    }
//    if (!url.GetFileName().IsEmpty())
//    {
//      url.SetFileName("");
//      strParent = url.Get();
//      return true;
//    }
//    if (!url.GetHostName().IsEmpty())
//    {
//      url.SetHostName("");
//      strParent = url.Get();
//      return true;
//    }
//    return true;  // already at root
//  }
//  else if (url.GetProtocol() == "special")
//  {
//    if (HasSlashAtEnd(strFile) )
//      strFile = strFile.Left(strFile.size() - 1);
//    if(strFile.ReverseFind('/') < 0)
//      return false;
//  }
  else if (strFile.size() == 0)
  {
    if (url.GetHostName().size() > 0)
    {
      // we have an share with only server or workgroup name
      // set hostname to "" and return true to get back to root
      url.SetHostName("");
      strParent = url.Get();
      return true;
    }
    return false;
  }

  if (HasSlashAtEnd(strFile) )
  {
    strFile = strFile.Left(strFile.size() - 1);
  }

  int iPos = strFile.ReverseFind('/');
#ifndef _LINUX
  if (iPos < 0)
  {
    iPos = strFile.ReverseFind('\\');
  }
#endif
  if (iPos < 0)
  {
    url.SetFileName("");
    strParent = url.Get();
    return true;
  }

  strFile = strFile.Left(iPos);

  AddSlashAtEnd(strFile);

  url.SetFileName(strFile);
  strParent = url.Get();
  return true;
}

bool URIUtils::ProtocolHasParentInHostname(const CStdString& prot)
{
  return prot.Equals("zip")
      || prot.Equals("rar")
      || prot.Equals("bluray")
      || prot.Equals("udf");
}

bool URIUtils::ProtocolHasEncodedHostname(const CStdString& prot)
{
  return ProtocolHasParentInHostname(prot)
      || prot.Equals("musicsearch")
      || prot.Equals("image");
}

bool URIUtils::ProtocolHasEncodedFilename(const CStdString& prot)
{
  CStdString prot2 = CURL::TranslateProtocol(prot);

  // For now assume only (quasi) http internet streams use URL encoding
  return prot2 == "http"  ||
         prot2 == "https";
}

//
//bool URIUtils::IsInPath(const CStdString &uri, const CStdString &baseURI)
//{
//  CStdString uriPath = CSpecialProtocol::TranslatePath(uri);
//  CStdString basePath = CSpecialProtocol::TranslatePath(baseURI);
//  return (strncmp(uriPath.c_str(), basePath.c_str(), basePath.GetLength()) == 0);
//}

/* returns filename extension including period of filename */
CStdString URIUtils::GetExtension(const CStdString& strFileName)
{
  if (IsURL(strFileName))
  {
    CURL url(strFileName);
    return GetExtension(url.GetFileName());
  }

  size_t period = strFileName.find_last_of("./\\");
  if (period == string::npos || strFileName[period] != '.')
    return CStdString();

  return strFileName.substr(period);
}

CStdString URIUtils::AddFileToFolder(const CStdString& strFolder,
                                const CStdString& strFile)
{
  if (IsURL(strFolder))
  {
    CURL url(strFolder);
    if (url.GetFileName() != strFolder)
    {
      url.SetFileName(AddFileToFolder(url.GetFileName(), strFile));
      return url.Get();
    }
  }

  CStdString strResult = strFolder;
  if (!strResult.IsEmpty())
    AddSlashAtEnd(strResult);

  // Remove any slash at the start of the file
  if (strFile.size() && (strFile[0] == '/' || strFile[0] == '\\'))
    strResult += strFile.Mid(1);
  else
    strResult += strFile;

  // correct any slash directions
  if (!IsDOSPath(strFolder))
    strResult.Replace('\\', '/');
  else
    strResult.Replace('/', '\\');

  return strResult;
}

void URIUtils::AddSlashAtEnd(CStdString& strFolder)
{
  if (IsURL(strFolder))
  {
    CURL url(strFolder);
    CStdString file = url.GetFileName();
    if(!file.IsEmpty() && file != strFolder)
    {
      AddSlashAtEnd(file);
      url.SetFileName(file);
      strFolder = url.Get();
    }
    return;
  }

  if (!HasSlashAtEnd(strFolder))
  {
    if (IsDOSPath(strFolder))
      strFolder += '\\';
    else
      strFolder += '/';
  }
}

bool URIUtils::HasSlashAtEnd(const CStdString& strFile, bool checkURL /* = false */)
{
  if (strFile.size() == 0) return false;
  if (checkURL && IsURL(strFile))
  {
    CURL url(strFile);
    CStdString file = url.GetFileName();
    return file.IsEmpty() || HasSlashAtEnd(file, false);
  }
  char kar = strFile.c_str()[strFile.size() - 1];

  if (kar == '/' || kar == '\\')
    return true;

  return false;
}

void URIUtils::RemoveSlashAtEnd(CStdString& strFolder)
{
  if (IsURL(strFolder))
  {
    CURL url(strFolder);
    CStdString file = url.GetFileName();
    if (!file.IsEmpty() && file != strFolder)
    {
      RemoveSlashAtEnd(file);
      url.SetFileName(file);
      strFolder = url.Get();
      return;
    }
    if(url.GetHostName().IsEmpty())
      return;
  }

  while (HasSlashAtEnd(strFolder))
    strFolder.Delete(strFolder.size() - 1);
}

bool URIUtils::CompareWithoutSlashAtEnd(const CStdString& strPath1, const CStdString& strPath2)
{
  CStdString strc1 = strPath1, strc2 = strPath2;
  RemoveSlashAtEnd(strc1);
  RemoveSlashAtEnd(strc2);
  return strc1.Equals(strc2);
}

void URIUtils::Split(const CStdString& strFileNameAndPath,
                     CStdString& strPath, CStdString& strFileName)
{
  //Splits a full filename in path and file.
  //ex. smb://computer/share/directory/filename.ext -> strPath:smb://computer/share/directory/ and strFileName:filename.ext
  //Trailing slash will be preserved
  strFileName = "";
  strPath = "";
  int i = strFileNameAndPath.size() - 1;
  while (i > 0)
  {
    char ch = strFileNameAndPath[i];
    // Only break on ':' if it's a drive separator for DOS (ie d:foo)
    if (ch == '/' || ch == '\\' || (ch == ':' && i == 1)) break;
    else i--;
  }
  if (i == 0)
    i--;

  strPath = strFileNameAndPath.Left(i + 1);
  strFileName = strFileNameAndPath.Right(strFileNameAndPath.size() - i - 1);
}

CStdStringArray URIUtils::SplitPath(const CStdString& strPath)
{
  CURL url(strPath);

  // silly CStdString can't take a char in the constructor
  CStdString sep(1, url.GetDirectorySeparator());

  // split the filename portion of the URL up into separate dirs
  CStdStringArray dirs;
  StringUtils::SplitString(url.GetFileName(), sep, dirs);

  // we start with the root path
  CStdString dir = url.GetWithoutFilename();

  if (!dir.IsEmpty())
    dirs.insert(dirs.begin(), dir);

  // we don't need empty token on the end
  if (dirs.size() > 1 && dirs.back().IsEmpty())
    dirs.erase(dirs.end() - 1);

  return dirs;
}

bool URIUtils::IsOnDVD(const CStdString& strFile)
{
#ifdef _WIN32
  if (strFile.Mid(1,1) == ":")
    return (GetDriveType(strFile.Left(2)) == DRIVE_CDROM);
#endif

  if (strFile.Left(4).CompareNoCase("dvd:") == 0)
    return true;

  if (strFile.Left(4).CompareNoCase("udf:") == 0)
    return true;

  if (strFile.Left(8).CompareNoCase("iso9660:") == 0)
    return true;

  if (strFile.Left(5).CompareNoCase("cdda:") == 0)
    return true;

  return false;
}

bool URIUtils::IsMultiPath(const CStdString& strPath)
{
  return strPath.Left(10).Equals("multipath:");
}

bool URIUtils::IsDVD(const CStdString& strFile)
{
  CStdString strFileLow = strFile;
  strFileLow.MakeLower();
  if (strFileLow.Find("video_ts.ifo") != -1 && IsOnDVD(strFile))
    return true;

#if defined(_WIN32)
  if (strFile.Left(6).Equals("dvd://"))
    return true;

  if(strFile.Mid(1) != ":\\"
  && strFile.Mid(1) != ":")
    return false;

  if(GetDriveType(strFile.c_str()) == DRIVE_CDROM)
    return true;
#else
  if (strFileLow == "iso9660://" || strFileLow == "udf://" || strFileLow == "dvd://1" )
    return true;
#endif

  return false;
}

bool URIUtils::IsStack(const CStdString& strFile)
{
  return strFile.Left(6).Equals("stack:");
}

bool URIUtils::IsRAR(const CStdString& strFile)
{
  CStdString strExtension = GetExtension(strFile);

  if (strExtension.Equals(".001") && strFile.Mid(strFile.length()-7,7).CompareNoCase(".ts.001"))
    return true;

  if (strExtension.CompareNoCase(".cbr") == 0)
    return true;

  if (strExtension.CompareNoCase(".rar") == 0)
    return true;

  return false;
}

bool URIUtils::IsInArchive(const CStdString &strFile)
{
  return IsInZIP(strFile) || IsInRAR(strFile) || IsInAPK(strFile);
}

bool URIUtils::IsInAPK(const CStdString& strFile)
{
  CURL url(strFile);

  return url.GetProtocol() == "apk" && url.GetFileName() != "";
}

bool URIUtils::IsInZIP(const CStdString& strFile)
{
  CURL url(strFile);

  return url.GetProtocol() == "zip" && url.GetFileName() != "";
}

bool URIUtils::IsInRAR(const CStdString& strFile)
{
  CURL url(strFile);

  return url.GetProtocol() == "rar" && url.GetFileName() != "";
}

bool URIUtils::IsAPK(const CStdString& strFile)
{
  return GetExtension(strFile).CompareNoCase(".apk") == 0;
}

bool URIUtils::IsZIP(const CStdString& strFile) // also checks for comic books!
{
  CStdString strExtension = GetExtension(strFile);

  if (strExtension.CompareNoCase(".zip") == 0)
    return true;

  if (strExtension.CompareNoCase(".cbz") == 0)
    return true;

  return false;
}

bool URIUtils::IsArchive(const CStdString& strFile)
{
  CStdString extension = GetExtension(strFile);

  return (extension.CompareNoCase(".zip") == 0 ||
          extension.CompareNoCase(".rar") == 0 ||
          extension.CompareNoCase(".apk") == 0 ||
          extension.CompareNoCase(".cbz") == 0 ||
          extension.CompareNoCase(".cbr") == 0);
}

bool URIUtils::IsPlugin(const CStdString& strFile)
{
  CURL url(strFile);
  return url.GetProtocol().Equals("plugin");
}

bool URIUtils::IsScript(const CStdString& strFile)
{
  CURL url(strFile);
  return url.GetProtocol().Equals("script");
}

bool URIUtils::IsAddonsPath(const CStdString& strFile)
{
  CURL url(strFile);
  return url.GetProtocol().Equals("addons");
}

bool URIUtils::IsSourcesPath(const CStdString& strPath)
{
  CURL url(strPath);
  return url.GetProtocol().Equals("sources");
}

bool URIUtils::IsCDDA(const CStdString& strFile)
{
  return strFile.Left(5).Equals("cdda:");
}

bool URIUtils::IsISO9660(const CStdString& strFile)
{
  return strFile.Left(8).Equals("iso9660:");
}

bool URIUtils::IsURL(const CStdString& strFile)
{
  return strFile.Find("://") >= 0;
}

bool URIUtils::IsDAAP(const CStdString& strFile)
{
  return strFile.Left(5).Equals("daap:");
}

bool URIUtils::IsUPnP(const CStdString& strFile)
{
  return strFile.Left(5).Equals("upnp:");
}

bool URIUtils::IsTuxBox(const CStdString& strFile)
{
  return strFile.Left(7).Equals("tuxbox:");
}

bool URIUtils::IsMythTV(const CStdString& strFile)
{
  return strFile.Left(5).Equals("myth:");
}

bool URIUtils::IsHDHomeRun(const CStdString& strFile)
{
  return strFile.Left(10).Equals("hdhomerun:");
}

bool URIUtils::IsSlingbox(const CStdString& strFile)
{
  return strFile.Left(6).Equals("sling:");
}

bool URIUtils::IsVTP(const CStdString& strFile)
{
  return strFile.Left(4).Equals("vtp:");
}

bool URIUtils::IsHTSP(const CStdString& strFile)
{
  return strFile.Left(5).Equals("htsp:");
}



bool URIUtils::IsPVRRecording(const CStdString& strFile)
{
  CStdString strFileWithoutSlash(strFile);
  RemoveSlashAtEnd(strFileWithoutSlash);

  return strFileWithoutSlash.Right(4).Equals(".pvr") &&
         strFile.Left(16).Equals("pvr://recordings");
}

bool URIUtils::IsMusicDb(const CStdString& strFile)
{
  return strFile.Left(8).Equals("musicdb:");
}

bool URIUtils::IsVideoDb(const CStdString& strFile)
{
  return strFile.Left(8).Equals("videodb:");
}

bool URIUtils::IsBluray(const CStdString& strFile)
{
  return strFile.Left(7).Equals("bluray:");
}

bool URIUtils::IsAndroidApp(const CStdString &path)
{
  return path.Left(11).Equals("androidapp:");
}

bool URIUtils::IsDOSPath(const CStdString &path)
{
  if (path.size() > 1 && path[1] == ':' && isalpha(path[0]))
    return true;

  // windows network drives
  if (path.size() > 1 && path[0] == '\\' && path[1] == '\\')
    return true;

  return false;
}
