/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://www.xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "DirectoryNodeYearAlbum.h"
#include "QueryParams.h"

using namespace XFILE::MUSICDATABASEDIRECTORY;

CDirectoryNodeYearAlbum::CDirectoryNodeYearAlbum(const CStdString& strName, CDirectoryNode* pParent)
  : CDirectoryNode(NODE_TYPE_YEAR_ALBUM, strName, pParent)
{

}

NODE_TYPE CDirectoryNodeYearAlbum::GetChildType() const
{
  if (GetName()=="-1")
    return NODE_TYPE_YEAR_SONG;

  return NODE_TYPE_SONG;
}

CStdString CDirectoryNodeYearAlbum::GetLocalizedName() const
{
  if (GetID() == -1)
    return "allalbums"; // All Albums
//  CMusicDatabase db;
//  if (db.Open())
//    return db.GetAlbumById(GetID());
//  return "";
}

//bool CDirectoryNodeYearAlbum::GetContent(CFileItemList& items) const
//{
//  CMusicDatabase musicdatabase;
//  if (!musicdatabase.Open())
//    return false;
//
//  CQueryParams params;
//  CollectQueryParams(params);
//
//  bool bSuccess=musicdatabase.GetAlbumsByYear(BuildPath(), items, params.GetYear());
//
//  musicdatabase.Close();
//
//  return bSuccess;
//}
