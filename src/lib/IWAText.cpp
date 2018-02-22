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
  , m_sections()
  , m_paras()
  , m_spans()

  , m_comments()
  , m_fields()
  , m_ignoreCharacters()
  , m_langs()
  , m_links()
  , m_lists()
  , m_listLevels()
  , m_notes()
{
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

void IWAText::setComments(const std::map<unsigned, IWORKOutputElements> &comments)
{
  m_comments = comments;
}

void IWAText::setFields(const std::map<unsigned, IWORKFieldType> &fields)
{
  m_fields = fields;
}

void IWAText::setIgnoreCharacters(const std::set<unsigned> &ignoreCharacters)
{
  m_ignoreCharacters = ignoreCharacters;
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

void IWAText::setNotes(const std::map<unsigned, IWORKOutputElements> &notes)
{
  m_notes = notes;
}

void IWAText::parse(IWORKText &collector)
{
  auto sectionIt = m_sections.begin();
  map<unsigned, IWORKStylePtr_t>::const_iterator paraIt = m_paras.begin();
  map<unsigned, IWORKStylePtr_t>::const_iterator spanIt = m_spans.begin();
  auto commentIt = m_comments.begin();
  auto fieldIt = m_fields.begin();
  auto ignoreCharacterIt = m_ignoreCharacters.begin();
  map<unsigned, string>::const_iterator langIt = m_langs.begin();
  map<unsigned, string>::const_iterator linkIt = m_links.begin();
  map<unsigned, IWORKStylePtr_t>::const_iterator listIt = m_lists.begin();
  map<unsigned, unsigned>::const_iterator listLevelIt = m_listLevels.begin();
  auto noteIt = m_notes.begin();
  bool wasSpace = false;
  IWORKStylePtr_t currentListStyle;
  bool isLink = false;
  string curText;

  std::size_t pos = 0;
  librevenge::RVNGString::Iter iter(m_text);
  iter.rewind();
  for (; iter.next(); ++pos)
  {
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
    // handle field
    if (fieldIt != m_fields.end() && fieldIt->first == pos)
    {
      flushText(curText, collector);
      collector.insertField(fieldIt->second);
      ++fieldIt;
      continue;
    }
    // handle note
    if (noteIt != m_notes.end() && noteIt->first == pos)
    {
      flushText(curText, collector);
      IWORKOutputElements elements;
      librevenge::RVNGPropertyList props;
      elements.addOpenFootnote(props);
      elements.append(noteIt->second);
      elements.addCloseFootnote();
      collector.insertInlineContent(elements);
      ++noteIt;
      continue;
    }
    if (commentIt != m_comments.end() && commentIt->first == pos)
    {
      flushText(curText, collector);
      IWORKOutputElements elements;
      librevenge::RVNGPropertyList props;
      elements.addOpenComment(props);
      elements.append(commentIt->second);
      elements.addCloseComment();
      collector.insertInlineContent(elements);
      ++commentIt;
    }
    // internal character: begin of a note, ...
    if ((ignoreCharacterIt != m_ignoreCharacters.end()) && (*ignoreCharacterIt == pos))
    {
      flushText(curText, collector);
      ++ignoreCharacterIt;
      continue;
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
      flushText(curText, collector);
      collector.flushParagraph();
      collector.insertPageBreak();
      break;
    }
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
