/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKLANGUAGEMANAGER_H_INCLUDED
#define IWORKLANGUAGEMANAGER_H_INCLUDED

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <librevenge/librevenge.h>

namespace libetonyek
{

class IWORKLanguageManager
{
  struct LangDB;

public:
  IWORKLanguageManager();

  const std::string addTag(const std::string &tag);
  const std::string addLanguage(const std::string &lang);
  const std::string addLocale(const std::string &locale);

  const std::string getLanguage(const std::string &tag) const;

  void writeProperties(const std::string &tag, librevenge::RVNGPropertyList &props) const;

private:
  const LangDB &getLangDB() const;

  void addProperties(const std::string &tag);

private:
  std::unordered_map<std::string, std::string> m_tagMap;
  std::unordered_set<std::string> m_invalidTags;
  std::unordered_map<std::string, std::string> m_langMap;
  std::unordered_set<std::string> m_invalidLangs;
  std::unordered_map<std::string, std::string> m_localeMap;
  std::unordered_set<std::string> m_invalidLocales;
  std::unordered_map<std::string, librevenge::RVNGPropertyList> m_propsMap;
  mutable std::shared_ptr<LangDB> m_langDB;
};

}

#endif // IWORKLANGUAGEMANAGER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
