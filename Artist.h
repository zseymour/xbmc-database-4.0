#pragma once

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

#include <map>
#include <vector>
#include "utils/StdString.h"
#include "utils/XMLUtils.h"

class TiXmlNode;
class CAlbum;

class CArtist
{
public:
  long idArtist;
  bool operator<(const CArtist& a) const
  {
    if (strArtist < a.strArtist) return true;
    if (strArtist > a.strArtist) return false;
    if (strMusicBrainzArtistID < a.strMusicBrainzArtistID) return true;
    if (strMusicBrainzArtistID > a.strMusicBrainzArtistID) return false;
    return false;
  }

  void Reset()
  {
    strArtist.Empty();
    genre.clear();
    strBiography.Empty();
    styles.clear();
    moods.clear();
    instruments.clear();
    strBorn.Empty();
    strFormed.Empty();
    strDied.Empty();
    strDisbanded.Empty();
    yearsActive.clear();
    discography.clear();
    idArtist = -1;
    strPath.Empty();
  }

  /*! \brief Load artist information from an XML file.
   See CVideoInfoTag::Load for a description of the types of elements we load.
   \param element    the root XML element to parse.
   \param append     whether information should be added to the existing tag, or whether it should be reset first.
   \param prioritise if appending, whether additive tags should be prioritised (i.e. replace or prepend) over existing values. Defaults to false.
   \sa CVideoInfoTag::Load
   */
  bool Load(const TiXmlElement *element, bool append = false, bool prioritise = false);
  bool Save(TiXmlNode *node, const CStdString &tag, const CStdString& strPath);

  CStdString strArtist;
  CStdString strMusicBrainzArtistID;
  std::vector<std::string> genre;
  CStdString strBiography;
  std::vector<std::string> styles;
  std::vector<std::string> moods;
  std::vector<std::string> instruments;
  CStdString strBorn;
  CStdString strFormed;
  CStdString strDied;
  CStdString strDisbanded;
  std::vector<std::string> yearsActive;
  CStdString strPath;
  std::vector<std::pair<CStdString,CStdString> > discography;
};

class CArtistCredit
{
  friend class CAlbum;

public:
  CArtistCredit() { }
  CArtistCredit(std::string strArtist, std::string strJoinPhrase) : m_strArtist(strArtist), m_strJoinPhrase(strJoinPhrase), m_boolFeatured(false) { }
  CArtistCredit(std::string strArtist, std::string strMusicBrainzArtistID, std::string strJoinPhrase)
  : m_strArtist(strArtist), m_strMusicBrainzArtistID(strMusicBrainzArtistID), m_strJoinPhrase(strJoinPhrase), m_boolFeatured(false)  {  }
  bool operator<(const CArtistCredit& a) const
  {
    if (m_strArtist < a.m_strArtist) return true;
    if (m_strArtist > a.m_strArtist) return false;
    if (m_strMusicBrainzArtistID < a.m_strMusicBrainzArtistID) return true;
    if (m_strMusicBrainzArtistID > a.m_strMusicBrainzArtistID) return false;
    return false;
  }

  std::string GetArtist() const                { return m_strArtist; }
  std::string GetMusicBrainzArtistID() const   { return m_strMusicBrainzArtistID; }
  std::string GetJoinPhrase() const            { return m_strJoinPhrase; }
  void SetArtist(const std::string &strArtist) { m_strArtist = strArtist; }
  void SetMusicBrainzArtistID(const std::string &strMusicBrainzArtistID) { m_strMusicBrainzArtistID = strMusicBrainzArtistID; }
  void SetJoinPhrase(const std::string &strJoinPhrase) { m_strJoinPhrase = strJoinPhrase; }

private:
  long idArtist;
  std::string m_strArtist;
  std::string m_strMusicBrainzArtistID;
  std::string m_strJoinPhrase;
  bool m_boolFeatured;
};

typedef std::vector<CArtist> VECARTISTS;
typedef std::vector<CArtistCredit> VECARTISTCREDITS;

