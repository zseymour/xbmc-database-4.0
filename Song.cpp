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

#include "Song.h"
#include "utils/Variant.h"
#include "utils/StringUtils.h"

using namespace std;


CSong::CSong()
{
  Clear();
}


void CSong::Clear()
{
  strFileName.Empty();
  strTitle.Empty();
  artist.clear();
  strAlbum.Empty();
  albumArtist.clear();
  genre.clear();
  strMusicBrainzTrackID.Empty();
  strComment.Empty();
  rating = '0';
  iTrack = 0;
  iDuration = 0;
  iYear = 0;
  iStartOffset = 0;
  iEndOffset = 0;
  idSong = -1;
  iTimesPlayed = 0;
  lastPlayed.Reset();
  iKaraokeNumber = 0;
  strKaraokeLyrEncoding.Empty();
  iKaraokeDelay = 0;
  idAlbum = -1;
  bCompilation = false;
}

bool CSong::HasArt() const
{
  if (!strThumb.empty()) return true;
  //if (!embeddedArt.empty()) return true;
  return false;
}

bool CSong::ArtMatches(const CSong &right) const
{
  return (right.strThumb == strThumb);
}
