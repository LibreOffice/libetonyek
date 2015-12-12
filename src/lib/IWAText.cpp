/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWAText.h"

#include "IWORKText.h"

namespace libetonyek
{

using std::make_pair;
using std::map;
using std::pair;
using std::string;

namespace
{

void mergeTextSpans(const map<unsigned, IWORKStylePtr_t> &paras,
                    const map<unsigned, IWORKStylePtr_t> &spans,
                    const map<unsigned, string> &langs,
                    map<unsigned, pair<IWORKStylePtr_t, IWORKStylePtr_t> > &merged)
{
  merged[0] = make_pair(IWORKStylePtr_t(), IWORKStylePtr_t());
  for (map<unsigned, IWORKStylePtr_t>::const_iterator it = paras.begin(); it != paras.end(); ++it)
    merged[it->first].first = it->second;
  for (map<unsigned, IWORKStylePtr_t>::const_iterator it = spans.begin(); it != spans.end(); ++it)
    merged[it->first].second = it->second;
  for (map<unsigned, string>::const_iterator it = langs.begin(); it != langs.end(); ++it)
  {
    map<unsigned, pair<IWORKStylePtr_t, IWORKStylePtr_t> >::iterator mergedIt = merged.lower_bound(it->first);
    if (mergedIt == merged.end())
      mergedIt = merged.insert(merged.end(), make_pair(it->first, make_pair(IWORKStylePtr_t(), IWORKStylePtr_t())));
    // TODO: create a new char. style with the language and make the
    // current char. style parent of it
    IWORKStylePtr_t langStyle;
    mergedIt->second.second = langStyle;
  }
}

void flushText(string &text, IWORKText &collector)
{
  if (!text.empty())
  {
    collector.insertText(text);
    text.clear();
  }
}

void writeText(const string &text, const unsigned start, const unsigned end, const bool endPara, IWORKText &collector)
{
  assert(end <= text.size());

  string buf;
  for (unsigned i = start; i < end; ++i)
  {
    switch (text[i])
    {
    case '\t' :
      flushText(buf, collector);
      collector.insertTab();
      break;
    case '\r' :
      flushText(buf, collector);
      collector.insertLineBreak();
      break;
    case '\n' :
      flushText(buf, collector);
      if (endPara && i != end - 1) // ignore the newline that ends the paragraph
        collector.insertLineBreak();
      break;
    default :
      buf.push_back(text[i]);
      break;
    }
  }

  flushText(buf, collector);
}

}

IWAText::IWAText(const std::string text)
  : m_text(text)
  , m_paras()
  , m_spans()
  , m_langs()
{
}

void IWAText::setParagraphs(const std::map<unsigned, boost::shared_ptr<IWORKStyle> > &paras)
{
  m_paras = paras;
}

void IWAText::setSpans(const std::map<unsigned, boost::shared_ptr<IWORKStyle> > &spans)
{
  m_spans = spans;
}

void IWAText::setLanguages(const std::map<unsigned, std::string> &langs)
{
  m_langs = langs;
}

void IWAText::parse(IWORKText &collector)
{
  map<unsigned, pair<IWORKStylePtr_t, IWORKStylePtr_t> > textSpans;
  mergeTextSpans(m_paras, m_spans, m_langs, textSpans);

  for (map<unsigned, pair<IWORKStylePtr_t, IWORKStylePtr_t> >::const_iterator it = textSpans.begin(); it != textSpans.end();)
  {
    if (bool(it->second.first))
      collector.setParagraphStyle(it->second.first);
    collector.setSpanStyle(it->second.second);
    const unsigned start = it->first;
    ++it;
    if (it == textSpans.end())
    {
      writeText(m_text, start, m_text.size(), true, collector);
      collector.flushSpan();
      collector.flushParagraph();
    }
    else
    {
      writeText(m_text, start, it->first, bool(it->second.first), collector);
      collector.flushSpan();
      if (bool(it->second.first))
        collector.flushParagraph();
    }
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
