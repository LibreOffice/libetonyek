/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libe-book project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifdef HAVE_CONFIG
#include "config.h"
#endif

#include "IWORKLanguageManager.h"

#include <cstdlib>
#include <stdexcept>

#include <liblangtag/langtag.h>

#include "libetonyek_utils.h"

namespace libetonyek
{

using librevenge::RVNGPropertyList;

using std::shared_ptr;
using std::string;
using std::unordered_map;
using std::unordered_set;

namespace
{

const shared_ptr<lt_tag_t> parseTag(const std::string &lang)
{
  const shared_ptr<lt_tag_t> tag(lt_tag_new(), lt_tag_unref);
  lt_error_t *error = 0;
  lt_tag_parse(tag.get(), lang.c_str(), &error);
  if (error && lt_error_is_set(error, LT_ERR_ANY))
  {
    lt_error_unref(error);
    return shared_ptr<lt_tag_t>();
  }
  return tag;
}

const std::string makeFullTag(const shared_ptr<lt_tag_t> &tag)
{
  lt_error_t *error = 0;
  const shared_ptr<char> full(lt_tag_transform(tag.get(), &error), std::free);
  if (error && lt_error_is_set(error, LT_ERR_ANY))
  {
    lt_error_unref(error);
    return lt_tag_get_string(tag.get());
  }
  return full.get();
}

}

struct IWORKLanguageManager::LangDB
{
  LangDB();

  unordered_map<string, string> m_db;
};

IWORKLanguageManager::LangDB::LangDB()
  : m_db()
{
#ifdef ETONYEK_LANGTAG_DB_DATADIR
  lt_db_set_datadir(ETONYEK_LANGTAG_DB_DATADIR);
#endif

  shared_ptr<lt_lang_db_t> langDB(lt_db_get_lang(), lt_lang_db_unref);
  shared_ptr<lt_iter_t> it(LT_ITER_INIT(langDB.get()), lt_iter_finish);
  lt_pointer_t key(0);
  lt_pointer_t value(0);
  while (lt_iter_next(it.get(), &key, &value))
  {
    const char *const tag = reinterpret_cast<const char *>(key);
    lt_lang_t *const lang = reinterpret_cast<lt_lang_t *>(value);
    m_db[lt_lang_get_name(lang)] = tag;
  }
}

IWORKLanguageManager::IWORKLanguageManager()
  : m_tagMap()
  , m_invalidTags()
  , m_langMap()
  , m_invalidLangs()
  , m_localeMap()
  , m_invalidLocales()
  , m_propsMap()
  , m_langDB()
{
}

const std::string IWORKLanguageManager::addTag(const std::string &tag)
{
  // Check if the tag is already known
  const unordered_map<string, string>::const_iterator it = m_tagMap.find(tag);
  if (it != m_tagMap.end())
    return it->second;
  // Check if the tag was previously rejected as invalid
  const unordered_set<string>::const_iterator invIt = m_invalidTags.find(tag);
  if (invIt != m_invalidTags.end())
    return "";

  const shared_ptr<lt_tag_t> &langTag = parseTag(tag);
  if (!langTag)
  {
    m_invalidTags.insert(tag);
    return "";
  }

  const string fullTag(makeFullTag(langTag));
  m_tagMap[tag] = fullTag;
  addProperties(fullTag);

  return fullTag;
}

const std::string IWORKLanguageManager::addLanguage(const std::string &lang)
{
  // Check if the lang is already known
  const unordered_map<string, string>::const_iterator it = m_langMap.find(lang);
  if (it != m_langMap.end())
    return it->second;
  // Check if the lang was previously rejected as invalid
  const unordered_set<string>::const_iterator invIt = m_invalidLangs.find(lang);
  if (invIt != m_invalidLangs.end())
    return "";

  const unordered_map<string, string>::const_iterator langIt = getLangDB().m_db.find(lang);
  if (langIt == getLangDB().m_db.end())
  {
    m_invalidLangs.insert(lang);
    return "";
  }

  const shared_ptr<lt_tag_t> &langTag = parseTag(langIt->second);
  if (!langTag)
    throw std::logic_error("cannot parse tag that came from liblangtag language DB");

  const string fullTag(makeFullTag(langTag));
  m_langMap[lang] = fullTag;
  addProperties(fullTag);

  return fullTag;
}

const std::string IWORKLanguageManager::addLocale(const std::string &locale)
{
  // Check if the locale is already known
  const unordered_map<string, string>::const_iterator it = m_localeMap.find(locale);
  if (it != m_localeMap.end())
    return it->second;
  // Check if the locale was previously rejected as invalid
  const unordered_set<string>::const_iterator invIt = m_invalidLocales.find(locale);
  if (invIt != m_invalidLocales.end())
    return "";

  lt_error_t *error = 0;
  const shared_ptr<lt_tag_t> tag(lt_tag_convert_from_locale_string(locale.c_str(), &error), lt_tag_unref);
  if ((error && lt_error_is_set(error, LT_ERR_ANY)) || !tag)
  {
    lt_error_unref(error);
    m_invalidLocales.insert(locale);
    return "";
  }

  const string fullTag(makeFullTag(tag));
  m_tagMap[locale] = fullTag;
  addProperties(fullTag);

  return fullTag;
}

const std::string IWORKLanguageManager::getLanguage(const std::string &tag) const
{
  const shared_ptr<lt_tag_t> &langTag = parseTag(tag);
  if (!langTag)
    throw std::logic_error("cannot parse tag that has been successfully parsed before");
  return lt_lang_get_name(lt_tag_get_language(langTag.get()));
}

const IWORKLanguageManager::LangDB &IWORKLanguageManager::getLangDB() const
{
  if (!m_langDB)
    m_langDB.reset(new LangDB());
  return *m_langDB;
}

void IWORKLanguageManager::addProperties(const std::string &tag)
{
  const shared_ptr<lt_tag_t> &langTag = parseTag(tag);
  if (!langTag)
    throw std::logic_error("cannot parse tag that has been successfully parsed before");

  RVNGPropertyList props;
  const lt_lang_t *const lang = lt_tag_get_language(langTag.get());
  if (lang)
    props.insert("fo:language", lt_lang_get_tag(lang));
  const lt_region_t *const region = lt_tag_get_region(langTag.get());
  if (region)
    props.insert("fo:country", lt_region_get_tag(region));
  const lt_script_t *const script = lt_tag_get_script(langTag.get());
  if (script)
    props.insert("fo:script", lt_script_get_tag(script));

  m_propsMap[tag] = props;
}

void IWORKLanguageManager::writeProperties(const std::string &tag, librevenge::RVNGPropertyList &props) const
{
  const unordered_map<string, RVNGPropertyList>::const_iterator it = m_propsMap.find(tag);
  if (it == m_propsMap.end())
  {
    ETONYEK_DEBUG_MSG(("IWORKLanguageManager::writeProperties: unknown tag %s\n", tag.c_str()));
    return;
  }
  for (RVNGPropertyList::Iter iter(it->second); !iter.last(); iter.next())
    props.insert(iter.key(), iter()->getStr());
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
