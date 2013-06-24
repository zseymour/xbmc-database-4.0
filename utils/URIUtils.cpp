/*
 * URIUtils.cpp
 *
 *  Created on: Jun 24, 2013
 *      Author: zachary
 */

#include "URIUtils.h"
#include "../URL.h"

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

bool URIUtils::IsDOSPath(const CStdString &path)
{
  if (path.size() > 1 && path[1] == ':' && isalpha(path[0]))
    return true;

  // windows network drives
  if (path.size() > 1 && path[0] == '\\' && path[1] == '\\')
    return true;

  return false;
}

bool URIUtils::IsURL(const CStdString& strFile)
{
  return strFile.Find("://") >= 0;
}

bool URIUtils::IsStack(const CStdString& strFile)
{
  return strFile.Left(6).Equals("stack:");
}
