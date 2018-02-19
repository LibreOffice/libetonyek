/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWATEXT_H_INCLUDED
#define IWATEXT_H_INCLUDED

#include <map>
#include <string>

#include <librevenge/librevenge.h>

#include "IWORKEnum.h"
#include "IWORKStyle_fwd.h"

namespace libetonyek
{

class IWORKLanguageManager;
class IWORKOutputElements;
class IWORKText;

class IWAText
{
public:
  IWAText(const std::string text, IWORKLanguageManager &langManager);

  void setParagraphs(const std::map<unsigned, IWORKStylePtr_t> &paras);
  void setSpans(const std::map<unsigned, IWORKStylePtr_t> &spans);

  void setFields(const std::map<unsigned, IWORKFieldType> &fields);
  void setLanguages(const std::map<unsigned, std::string> &langs);
  void setLinks(const std::map<unsigned, std::string> &links);
  void setListLevels(const std::map<unsigned, unsigned> &levels);
  void setLists(const std::map<unsigned, IWORKStylePtr_t> &lists);
  void setNotes(const std::map<unsigned, IWORKOutputElements> &notes);

  void parse(IWORKText &collector);

private:
  const librevenge::RVNGString m_text;
  IWORKLanguageManager &m_langManager;

  std::map<unsigned, IWORKStylePtr_t> m_paras;
  std::map<unsigned, IWORKStylePtr_t> m_spans;
  std::map<unsigned, IWORKFieldType> m_fields;
  std::map<unsigned, std::string> m_langs;
  std::map<unsigned, std::string> m_links;
  std::map<unsigned, IWORKStylePtr_t> m_lists;
  std::map<unsigned, unsigned> m_listLevels;
  std::map<unsigned, IWORKOutputElements> m_notes;
};

}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
