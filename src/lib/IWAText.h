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

#include <functional>
#include <map>
#include <set>
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
  IWAText(const std::string &text, IWORKLanguageManager &langManager);

  void setPageMasters(const std::map<unsigned, IWORKStylePtr_t> &pageMasters);
  void setSections(const std::map<unsigned, IWORKStylePtr_t> &sections);
  void setParagraphs(const std::map<unsigned, IWORKStylePtr_t> &paras);
  void setSpans(const std::map<unsigned, IWORKStylePtr_t> &spans);

  void setLanguages(const std::map<unsigned, std::string> &langs);
  void setLinks(const std::map<unsigned, std::string> &links);
  void setListLevels(const std::map<unsigned, unsigned> &levels);
  void setLists(const std::map<unsigned, IWORKStylePtr_t> &lists);
  void setDropCaps(const std::map<unsigned, IWORKStylePtr_t> &dropCaps);
  void setRTLs(const std::map<unsigned, bool> &rtls);

  void setAttachments(const std::multimap<unsigned, std::function<void(unsigned, bool &)> > &attachments);

  void parse(IWORKText &collector, const std::function<void(unsigned, IWORKStylePtr_t)> &openPageSpan=nullptr);

private:
  const librevenge::RVNGString m_text;
  IWORKLanguageManager &m_langManager;

  std::map<unsigned, IWORKStylePtr_t> m_pageMasters;
  std::map<unsigned, IWORKStylePtr_t> m_sections;
  std::map<unsigned, IWORKStylePtr_t> m_paras;
  std::map<unsigned, IWORKStylePtr_t> m_spans;

  std::map<unsigned, std::string> m_langs;
  std::map<unsigned, std::string> m_links;
  std::map<unsigned, IWORKStylePtr_t> m_lists;
  std::map<unsigned, unsigned> m_listLevels;
  std::map<unsigned, IWORKStylePtr_t> m_dropCaps;
  std::map<unsigned, bool> m_rtls;

  std::multimap<unsigned, std::function<void(unsigned, bool &)> > m_attachments;
};

}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
