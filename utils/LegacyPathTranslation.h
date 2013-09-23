#pragma once
/*
 *      Copyright (C) 2013 Team XBMC
 *      http://www.xbmc.org
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <string>

typedef struct Translator Translator;

/*!
 \brief Translates old internal paths into new ones

 Translates old videodb:// and musicdb:// paths which used numbers
 to indicate a specific category to new paths using more descriptive
 strings to indicate categories.
 */
class CLegacyPathTranslation
{
public:
  /*!
   \brief Translates old videodb:// paths to new ones

   \param legacyPath Path in the old videodb:// format using numbers
   \return Path in the new videodb:// format using descriptive strings
   */
  static std::string TranslateVideoDbPath(const std::string &legacyPath);

  /*!
   \brief Translates old musicdb:// paths to new ones

   \param legacyPath Path in the old musicdb:// format using numbers
   \return Path in the new musicdb:// format using descriptive strings
   */
  static std::string TranslateMusicDbPath(const std::string &legacyPath);

private:
  static std::string TranslatePath(const std::string &legacyPath, Translator *translationMap, size_t translationMapSize);
};
