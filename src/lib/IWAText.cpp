/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWAText.h"

#include <boost/make_shared.hpp>

#include "IWORKProperties.h"
#include "IWORKText.h"

namespace libetonyek
{

using boost::make_shared;
using boost::none;
using boost::shared_ptr;

using std::make_pair;
using std::map;
using std::pair;
using std::string;

IWAText::IWAText(const std::string text)
  : m_text(text)
  , m_paras()
  , m_spans()
  , m_langs()
  , m_links()
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

void IWAText::parse(IWORKText &collector)
{
  map<unsigned, IWORKStylePtr_t>::const_iterator paraIt = m_paras.begin();
  map<unsigned, IWORKStylePtr_t>::const_iterator spanIt = m_spans.begin();
  map<unsigned, string>::const_iterator langIt = m_langs.begin();
  map<unsigned, string>::const_iterator linkIt = m_links.begin();
  size_t textStart = 0;
  bool wasSpace = false;
  IWORKStylePtr_t currentSpanStyle;
  bool isLink = false;

  for (size_t i = 0; i != m_text.size(); ++i)
  {
    // handle span style change
    IWORKStylePtr_t spanStyle;
    bool span = false;
    if ((spanIt != m_spans.end()) && (spanIt->first == i))
    {
      spanStyle = currentSpanStyle = spanIt->second;
      span = true;
      ++spanIt;
    }
    if ((langIt != m_langs.end()) && (langIt->first == i))
    {
      IWORKPropertyMap props;
      if (!langIt->second.empty())
        props.put<property::Language>(langIt->second);
      if (bool(currentSpanStyle))
        props.setParent(&currentSpanStyle->getPropertyMap());
      spanStyle = make_shared<IWORKStyle>(props, none, none);
      span = true;
      ++langIt;
    }
    if (span)
    {
      if (textStart < i)
        collector.insertText(m_text.substr(textStart, i - textStart));
      textStart = i;
      if (i != 0)
        collector.flushSpan();
      collector.setSpanStyle(spanStyle);
    }

    // handle start/end of a link
    if ((linkIt != m_links.end()) && (linkIt->first == i))
    {
      if (textStart < i)
        collector.insertText(m_text.substr(textStart, i - textStart));
      textStart = i;
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
    if ((paraIt != m_paras.end()) && (paraIt->first == i))
    {
      collector.setParagraphStyle(paraIt->second);
      ++paraIt;
    }

    // handle text
    switch (m_text[i])
    {
    case '\t' :
      wasSpace = false;
      if (textStart < i)
        collector.insertText(m_text.substr(textStart, i - textStart));
      textStart = i + 1;
      collector.insertTab();
      break;
    case '\r' :
      wasSpace = false;
      if (textStart < i)
        collector.insertText(m_text.substr(textStart, i - textStart));
      textStart = i + 1;
      collector.insertLineBreak();
      break;
    case '\n' :
      wasSpace = false;
      if (textStart < i)
        collector.insertText(m_text.substr(textStart, i - textStart));
      textStart = i + 1;
      collector.flushParagraph();
      break;
    case ' ' :
      if (wasSpace)
      {
        if (textStart < i)
          collector.insertText(m_text.substr(textStart, i - textStart));
        textStart = i + 1;
        collector.insertSpace();
      }
      wasSpace = true;
      break;
    default :
      wasSpace = false;
      break;
    }
  }

  if (textStart < m_text.size())
  {
    collector.insertText(m_text.substr(textStart, m_text.size() - textStart));
    collector.flushParagraph();
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
