/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

#include "KEY1SpanElement.h"

#include "IWORKProperties.h"
#include "IWORKText.h"
#include "KEY1ParserState.h"
#include "KEY1StringConverter.h"
#include "KEY1Token.h"
#include "KEY1XMLContextBase.h"

#include "libetonyek_xml.h"

namespace libetonyek
{
KEY1SpanStyle::KEY1SpanStyle(KEY1ParserState &/*state*/, IWORKStylePtr_t parentStyle)
  : m_style()
  , m_parentStyle(parentStyle)
  , m_propMap()
{
}

bool KEY1SpanStyle::readAttribute(const int name, const char *const value)
{
  switch (name)
  {
  case KEY1Token::font_color :
  {
    boost::optional<IWORKColor> color=KEY1StringConverter<IWORKColor>::convert(value);
    if (color)
      m_propMap.put<property::FontColor>(get(color));
    else
    {
      ETONYEK_DEBUG_MSG(("KEY1SpanStyle::readAttribute[KEY1SpanElement.cpp]: unknown font color %s\n", value));
    }
    return true;
  }
  case KEY1Token::font_ligatures :   // with value=all
  {
    static bool first=true;
    if (first)
    {
      ETONYEK_DEBUG_MSG(("KEY1SpanStyle::readAttribute[KEY1SpanElement.cpp]: oops find some font ligatures\n"));
      first=false;
    }
    return true;
  }
  case KEY1Token::font_name :
    m_propMap.put<property::FontName>(value);
    return true;
  case KEY1Token::font_size :
  {
    boost::optional<double> size=try_double_cast(value);
    if (size)
      m_propMap.put<property::FontSize>(get(size));
    else
    {
      ETONYEK_DEBUG_MSG(("KEY1SpanStyle::readAttribute[KEY1SpanElement.cpp]: unknown font size %s\n", value));
    }
    return true;
  }
  case KEY1Token::font_kerning :
  {
    boost::optional<double> kerning=try_double_cast(value);
    if (kerning)
      m_propMap.put<property::Tracking>(get(kerning));
    else
    {
      ETONYEK_DEBUG_MSG(("KEY1SpanStyle::readAttribute[KEY1SpanElement.cpp]: unknown font kerning %s\n", value));
    }
    return true;
  }
  case KEY1Token::font_superscript :
  {
    boost::optional<int> val=try_int_cast(value);
    if (val)
      m_propMap.put<property::BaselineShift>(get(val));
    else
    {
      ETONYEK_DEBUG_MSG(("KEY1SpanStyle::readAttribute[KEY1SpanElement.cpp]: unknown superscript %s\n", value));
    }
    return true;
  }
  case KEY1Token::font_underline :
  {
    boost::optional<bool> val=try_bool_cast(value);
    if (val)
      m_propMap.put<property::Underline>(get(val));
    else
    {
      ETONYEK_DEBUG_MSG(("KEY1SpanStyle::readAttribute[KEY1SpanElement.cpp]: unknown underline %s\n", value));
    }
    return true;
  }
  default:
    return false;
  }
  return true;
}

IWORKStylePtr_t KEY1SpanStyle::getStyle()
{
  if (!m_style)
    m_style.reset(new IWORKStyle(m_propMap, boost::none, m_parentStyle));
  return m_style;
}

KEY1SpanElement::KEY1SpanElement(KEY1ParserState &state, IWORKStylePtr_t parentStyle, bool &delayedLineBreak)
  : KEY1XMLElementContextBase(state)
  , m_style(state, parentStyle)
  , m_opened(false)
  , m_delayedLineBreak(delayedLineBreak)
{
}

void KEY1SpanElement::attribute(const int name, const char *const value)
{
  if (m_style.readAttribute(name, value))
    return;
  switch (name)
  {
  case KEY1Token::id :
    setId(value);
    break;
  default:
    ETONYEK_DEBUG_MSG(("KEY1SpanElement::attribute: unknown attribute with value=%s\n", value));
  }
}

IWORKXMLContextPtr_t KEY1SpanElement::element(const int /*name*/)
{
  ETONYEK_DEBUG_MSG(("KEY1SpanElement::element: unknown element\n"));
  return IWORKXMLContextPtr_t();
}

void KEY1SpanElement::CDATA(const char *value)
{
  ensureClosed();
  sendCDATA(value, getState().m_currentText, m_style.getStyle(), m_delayedLineBreak);
}

void KEY1SpanElement::text(const char *const value)
{
  if (m_delayedLineBreak)
  {
    ensureClosed();
    if (bool(getState().m_currentText))
      getState().m_currentText->flushParagraph();
    m_delayedLineBreak=false;
  }
  ensureOpened();
  if (bool(getState().m_currentText))
    getState().m_currentText->insertText(value);
}

void KEY1SpanElement::endOfElement()
{
  ensureClosed();
}

void KEY1SpanElement::ensureOpened()
{
  if (!m_opened)
  {
    if (bool(getState().m_currentText))
      getState().m_currentText->setSpanStyle(m_style.getStyle());
    m_opened = true;
  }
}

void KEY1SpanElement::ensureClosed()
{
  if (m_opened)
  {
    if (bool(getState().m_currentText))
    {
      getState().m_currentText->flushSpan();
      getState().m_currentText->setSpanStyle(IWORKStylePtr_t());
    }
    m_opened = false;
  }
}

void KEY1SpanElement::sendCDATA(const char *value, std::shared_ptr<IWORKText> currentText, IWORKStylePtr_t spanStyle, bool &delayedLineBreak)
{
  if (!currentText)
    return;

  if (delayedLineBreak)
  {
    currentText->flushParagraph();
    delayedLineBreak=false;
  }
  std::deque<std::string> listLines;
  std::string val(value);
  namespace spirit = boost::spirit;
  namespace qi =  boost::spirit::qi;
  using spirit::qi::char_;
  using spirit::eol;
  std::string::const_iterator it=val.begin(), end=val.end();
  if (qi::parse(it, end, - qi::as_string[*(char_ - eol)] % eol, listLines) && it==end)
  {
    size_t lastLine=listLines.size();
    if (lastLine>0 && listLines[lastLine-1].empty()) --lastLine;
    for (size_t i=0; i<lastLine; ++i)
    {
      currentText->setSpanStyle(spanStyle);
      currentText->insertText(listLines[i].c_str());
      currentText->flushSpan();
      if (i+1!=lastLine)
        currentText->flushParagraph();
      else
        delayedLineBreak=true;
    }
    currentText->setSpanStyle(IWORKStylePtr_t());
  }
  else
  {
    currentText->setSpanStyle(spanStyle);
    currentText->insertText(value);
    currentText->flushSpan();
    currentText->setSpanStyle(IWORKStylePtr_t());
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
