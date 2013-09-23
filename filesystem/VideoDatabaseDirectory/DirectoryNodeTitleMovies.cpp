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

#include "DirectoryNodeTitleMovies.h"
#include "QueryParams.h"

using namespace XFILE::VIDEODATABASEDIRECTORY;

CDirectoryNodeTitleMovies::CDirectoryNodeTitleMovies(const CStdString& strName, CDirectoryNode* pParent)
  : CDirectoryNode(NODE_TYPE_TITLE_MOVIES, strName, pParent)
{

}

//bool CDirectoryNodeTitleMovies::GetContent(CFileItemList& items) const
//{
//  CVideoDatabase videodatabase;
//  if (!videodatabase.Open())
//    return false;
//
//  CQueryParams params;
//  CollectQueryParams(params);
//
//  bool bSuccess=videodatabase.GetMoviesNav(BuildPath(), items, params.GetGenreId(), params.GetYear(), params.GetActorId(), params.GetDirectorId(), params.GetStudioId(), params.GetCountryId(), params.GetSetId(), params.GetTagId());
//
//  videodatabase.Close();
//
//  return bSuccess;
//}