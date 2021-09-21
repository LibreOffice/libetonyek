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

IWAText::IWAText(const std::string &text, IWORKLanguageManager &langManager)
  : m_text(text.c_str())
  , m_langManager(langManager)
  , m_pageMasters()
  , m_sections()
  , m_paras()
  , m_spans()

  , m_langs()
  , m_links()
  , m_lists()
  , m_listLevels()
  , m_dropCaps()
  , m_rtls()

  , m_attachments()
{
}

void IWAText::setPageMasters(const std::map<unsigned, IWORKStylePtr_t> &pageMasters)
{
  m_pageMasters = pageMasters;
}

void IWAText::setSections(const std::map<unsigned, IWORKStylePtr_t> &sections)
{
  m_sections = sections;
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

void IWAText::setDropCaps(const std::map<unsigned, IWORKStylePtr_t> &dropCaps)
{
  m_dropCaps = dropCaps;
}

void IWAText::setRTLs(const std::map<unsigned, bool> &rtls)
{
  m_rtls = rtls;
}

void IWAText::setAttachments(const std::multimap<unsigned, std::function<void(unsigned, bool &)> > &attachments)
{
  m_attachments = attachments;
}

void IWAText::parse(IWORKText &collector, const std::function<void(unsigned, IWORKStylePtr_t)> &openPageSpan)
{
  auto pageMasterIt = m_pageMasters.begin();
  auto sectionIt = m_sections.begin();
  map<unsigned, IWORKStylePtr_t>::const_iterator paraIt = m_paras.begin();
  map<unsigned, IWORKStylePtr_t>::const_iterator dropCapIt = m_dropCaps.begin();
  map<unsigned, bool>::const_iterator rtlIt = m_rtls.begin();
  map<unsigned, IWORKStylePtr_t>::const_iterator spanIt = m_spans.begin();
  map<unsigned, string>::const_iterator langIt = m_langs.begin();
  map<unsigned, string>::const_iterator linkIt = m_links.begin();
  map<unsigned, IWORKStylePtr_t>::const_iterator listIt = m_lists.begin();
  map<unsigned, unsigned>::const_iterator listLevelIt = m_listLevels.begin();
  auto attachmentIt = m_attachments.begin();
  bool wasSpace = false;
  IWORKStylePtr_t currentListStyle;
  bool isLink = false;
  string curText;

  // to handle drop cap's style, we need to set the drop cap style at
  // the beginning of a pararagraph, and then reset the current style
  // at the end of the drop cap's letters
  IWORKStylePtr_t lastSpanStyle;
  boost::optional<std::size_t> endDropCapPos;
  bool rtl=false;

  std::size_t pos = 0;
  librevenge::RVNGString::Iter iter(m_text);
  iter.rewind();
  for (; iter.next(); ++pos)
  {
    // first the page master change
    if ((pageMasterIt != m_pageMasters.end()) && (pageMasterIt->first == pos))
    {
      if (openPageSpan)
      {
        flushText(curText, collector);
        openPageSpan(unsigned(pos), pageMasterIt->second);
      }
      ++pageMasterIt;
    }
    // first handle section change
    if ((sectionIt != m_sections.end()) && (sectionIt->first == pos))
    {
      collector.flushLayout();
      collector.setLayoutStyle(sectionIt->second);
      ++sectionIt;
    }

    // handle span style change
    IWORKStylePtr_t spanStyle;
    IWORKStylePtr_t langStyle;
    IWORKStylePtr_t dropCapStyle;
    bool spanChanged = false;
    bool langChanged = false;
    bool isEndDropCap = bool(endDropCapPos) && get(endDropCapPos)==pos;
    if ((spanIt != m_spans.end()) && (spanIt->first == pos))
    {
      lastSpanStyle = spanStyle = spanIt->second;
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
    if (spanChanged || langChanged || isEndDropCap)
    {
      flushText(curText, collector);
      if (pos != 0)
        collector.flushSpan();
      if (spanChanged || isEndDropCap)
      {
        collector.setSpanStyle(spanChanged ? spanStyle : lastSpanStyle);
        endDropCapPos.reset();
      }
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

    // handle list style change
    if ((dropCapIt != m_dropCaps.end()) && (dropCapIt->first == pos))
    {
      dropCapStyle=dropCapIt->second;
      ++dropCapIt;
    }
    if ((rtlIt != m_rtls.end()) && (rtlIt->first==pos))
    {
      rtl=rtlIt->second;
      ++rtlIt;
    }
    // handle paragraph style change
    if ((paraIt != m_paras.end()) && (paraIt->first == pos))
    {
      if (dropCapStyle && dropCapStyle->has<property::DropCap>())
      {
        auto const &dropCap=dropCapStyle->get<property::DropCap>();
        if (!dropCap.empty() && dropCap.m_style)
        {
          collector.setSpanStyle(dropCap.m_style);
          endDropCapPos=pos+dropCap.m_numCharacters;
        }
        IWORKPropertyMap props;
        props.put<property::DropCap>(dropCap);
        if (rtl)
          props.put<property::WritingMode>("rl-tb");
        collector.setParagraphStyle(std::make_shared<IWORKStyle>(props, boost::none, paraIt->second));
      }
      else if (rtl)
      {
        IWORKPropertyMap props;
        props.put<property::WritingMode>("rl-tb");
        collector.setParagraphStyle(std::make_shared<IWORKStyle>(props, boost::none, paraIt->second));
      }
      else
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

    bool ignoreCharacter=false;
    while (attachmentIt != m_attachments.end() && attachmentIt->first == pos)
    {
      flushText(curText, collector);
      attachmentIt->second(unsigned(pos), ignoreCharacter);
      ++attachmentIt;
    }
    if (ignoreCharacter)
      continue;
    // handle text
    const char *const u8Char = iter();

    if (unsigned(u8Char[0])<=0x1f && bool(endDropCapPos))
    {
      // a special character, better to force a span style reset
      flushText(curText, collector);
      collector.flushSpan();
      collector.setSpanStyle(lastSpanStyle);
      endDropCapPos.reset();
    }

    switch (u8Char[0])
    {
    case char(4): // new section(ok)
      // be sure to send the last section, even if it has no content
      if (openPageSpan && pos+1==size_t(m_text.len()) && (pageMasterIt != m_pageMasters.end()) && (pageMasterIt->first == pos+1))
      {
        flushText(curText, collector);
        openPageSpan(unsigned(pos), pageMasterIt->second);
      }
      break;
    case char(14): // footnote: normally already ignored
      break;
    case char(5):
    {
      flushText(curText, collector);
      collector.flushParagraph();
      collector.insertPageBreak();
      break;
    }
    case char(12): // column break
      if (m_sections.empty()) break;
      flushText(curText, collector);
      collector.flushParagraph();
      collector.insertColumnBreak();
      break;
    case '\t' :
      flushText(curText, collector);
      collector.insertTab();
      break;
    case '\r' :
      flushText(curText, collector);
      collector.insertLineBreak();
      break;
    case '\n' :
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
        curText.push_back(' ');
      break;
    default:
      if (unsigned(u8Char[0])<=0x1f)
      {
        ETONYEK_DEBUG_MSG(("IWAText::parse: find bad character %d\n", (int) unsigned(u8Char[0])));
        break;
      }
      curText.append(u8Char);
      break;
    }
    wasSpace=u8Char[0]==' ';
  }
  flushText(curText, collector);
  collector.flushParagraph();
  collector.setListLevel(0);
  collector.flushList();
  collector.flushLayout();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
