/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWAText.h"

#include <memory>

#include "IWORKLanguageManager.h"
#include "IWORKProperties.h"
#include "IWORKText.h"

namespace libetonyek
{

using boost::none;

using std::make_shared;
using std::map;
using std::string;

namespace
{

void flushText(string &text, IWORKText &collector)
{
  if (!text.empty())
  {
    collector.insertText(text);
    text.clear();
  }
}

}

IWAText::IWAText(const std::string text, IWORKLanguageManager &langManager)
  : m_text(text.c_str())
  , m_langManager(langManager)
  , m_paras()
  , m_spans()
  , m_langs()
  , m_links()
  , m_lists()
  , m_listLevels()
{
}

void IWAText::setParagraphs(const std::map<unsigned, IWORKStylePtr_t> &paras)
{
  m_paras = paras;
}

void IWAText::setSpans(const std::map<unsigned, IWORKStylePtr_t> &spans)
{
  m_spans = spans;
}

void IWAText::setLanguages(const std::map<unsigned, std::string> &langs)
{
  m_langs = langs;
}

void IWAText::setLinks(const std::map<unsigned, std::string> &links)
{
  m_links = links;
}

void IWAText::setListLevels(const std::map<unsigned, unsigned> &levels)
{
  m_listLevels = levels;
}

void IWAText::setLists(const std::map<unsigned, IWORKStylePtr_t> &lists)
{
  m_lists = lists;
}

void IWAText::parse(IWORKText &collector)
{
  map<unsigned, IWORKStylePtr_t>::const_iterator paraIt = m_paras.begin();
  map<unsigned, IWORKStylePtr_t>::const_iterator spanIt = m_spans.begin();
  map<unsigned, string>::const_iterator langIt = m_langs.begin();
  map<unsigned, string>::const_iterator linkIt = m_links.begin();
  map<unsigned, IWORKStylePtr_t>::const_iterator listIt = m_lists.begin();
  map<unsigned, unsigned>::const_iterator listLevelIt = m_listLevels.begin();
  bool wasSpace = false;
  IWORKStylePtr_t currentListStyle;
  bool isLink = false;
  string curText;

  std::size_t pos = 0;
  librevenge::RVNGString::Iter iter(m_text);
  iter.rewind();
  for (; iter.next(); ++pos)
  {
    // handle span style change
    IWORKStylePtr_t spanStyle;
    IWORKStylePtr_t langStyle;
    bool spanChanged = false;
    bool langChanged = false;
    if ((spanIt != m_spans.end()) && (spanIt->first == pos))
    {
      spanStyle = spanIt->second;
      spanChanged = true;
      ++spanIt;
    }
    if ((langIt != m_langs.end()) && (langIt->first == pos))
    {
      IWORKPropertyMap props;
      if (!langIt->second.empty())
      {
        const string &tag = m_langManager.addTag(langIt->second);
        if (tag.empty())
          props.clear<property::Language>();
        else
          props.put<property::Language>(tag);
      }
      else
      {
        props.clear<property::Language>();
      }
      langStyle = make_shared<IWORKStyle>(props, none, none);
      langChanged = true;
      ++langIt;
    }
    if (spanChanged || langChanged)
    {
      flushText(curText, collector);
      if (pos != 0)
        collector.flushSpan();
      if (spanChanged)
        collector.setSpanStyle(spanStyle);
      if (langChanged)
        collector.setLanguage(langStyle);
    }

    // handle start/end of a link
    if ((linkIt != m_links.end()) && (linkIt->first == pos))
    {
      flushText(curText, collector);
      if (isLink)
      {
        collector.closeLink();
        isLink = false;
      }
      if (!linkIt->second.empty())
      {
        collector.openLink(linkIt->second);
        isLink = true;
      }
      ++linkIt;
    }

    // handle paragraph style change
    if ((paraIt != m_paras.end()) && (paraIt->first == pos))
    {
      collector.setParagraphStyle(paraIt->second);
      ++paraIt;
    }

    // handle list style change
    if ((listIt != m_lists.end()) && (listIt->first == pos))
    {
      currentListStyle = listIt->second;
      collector.setListStyle(currentListStyle);
      ++listIt;
    }

    // handle list level change
    if ((listLevelIt != m_listLevels.end()) && (listLevelIt->first == pos))
    {
      collector.setListLevel(listLevelIt->second + 1);
      ++listLevelIt;
    }

    // handle text
    const char *const u8Char = iter();
    switch (u8Char[0])
    {
    case char(5):
    {
      wasSpace = false;
      flushText(curText, collector);
      collector.flushParagraph();
      collector.insertPageBreak();
      break;
    }
    case char(14):
    {
      static bool first=true;
      if (first)
      {
        ETONYEK_DEBUG_MSG(("IWAText::parse:: find some footnotes, unimplemented\n"));
        first=false;
      }
      break;
    }
    case '\t' :
      wasSpace = false;
      flushText(curText, collector);
      collector.insertTab();
      break;
    case '\r' :
      wasSpace = false;
      flushText(curText, collector);
      collector.insertLineBreak();
      break;
    case '\n' :
      wasSpace = false;
      flushText(curText, collector);
      collector.flushParagraph();
      break;
    case ' ' :
      if (wasSpace)
      {
        flushText(curText, collector);
        collector.insertSpace();
      }
      else
      {
        wasSpace = true;
        curText.push_back(' ');
      }
      break;
    // find also 14
    default:
      if (unsigned(u8Char[0])<=0x1f)
      {
        ETONYEK_DEBUG_MSG(("IWAText::parse: find bad character %d\n", (int) unsigned(u8Char[0])));
        break;
      }
      wasSpace = false;
      curText.append(u8Char);
      break;
    }
  }

  flushText(curText, collector);
  collector.flushParagraph();
  collector.setListLevel(0);
  collector.flushList();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
