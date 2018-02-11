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

#include "KEY1DivElement.h"

#include "IWORKProperties.h"
#include "IWORKText.h"
#include "IWORKTokenizer.h"
#include "KEY1ParserState.h"
#include "KEY1StringConverter.h"
#include "KEY1Token.h"
#include "KEY1XMLContextBase.h"

#include "libetonyek_xml.h"

namespace libetonyek
{
KEY1DivStyle::KEY1DivStyle(KEY1ParserState &state, IWORKStylePtr_t parentStyle)
  : m_state(state)
  , m_style()
  , m_parentStyle(parentStyle)
  , m_propMap()
{
}

bool KEY1DivStyle::readAttribute(const int name, const char *const value)
{
  switch (name)
  {
  case KEY1Token::paragraph_alignment:
  {
    switch (m_state.getTokenizer().getId(value))
    {
    case KEY1Token::center :
      m_propMap.put<property::Alignment>(IWORK_ALIGNMENT_CENTER);
      break;
    case KEY1Token::justified :
      m_propMap.put<property::Alignment>(IWORK_ALIGNMENT_JUSTIFY);
      break;
    case KEY1Token::left :
      m_propMap.put<property::Alignment>(IWORK_ALIGNMENT_LEFT);
      break;
    case KEY1Token::right :
      m_propMap.put<property::Alignment>(IWORK_ALIGNMENT_RIGHT);
      break;
    default:
      ETONYEK_DEBUG_MSG(("KEY1DivStyle::readAttribute[KEY1DivElement.cpp]: unknown alignment %s\n", value));
      break;
    }
    break;
  }
  case KEY1Token::paragraph_first_line_indent :
  {
    boost::optional<double> size=try_double_cast(value);
    if (size)
      m_propMap.put<property::FirstLineIndent>(get(size));
    else
    {
      ETONYEK_DEBUG_MSG(("KEY1DivStyle::readAttribute[KEY1DivElement.cpp]: unknown first line indent %s\n", value));
    }
    break;
  }
  case KEY1Token::paragraph_head_indent :
  {
    boost::optional<double> size=try_double_cast(value);
    if (size)
      m_propMap.put<property::LeftIndent>(get(size));
    else
    {
      ETONYEK_DEBUG_MSG(("KEY1DivStyle::readAttribute[KEY1DivElement.cpp]: unknown left line indent %s\n", value));
    }
    break;
  }
  case KEY1Token::paragraph_tail_indent :
  {
    boost::optional<double> size=try_double_cast(value);
    if (size)
      m_propMap.put<property::RightIndent>(get(size));
    else
    {
      ETONYEK_DEBUG_MSG(("KEY1DivStyle::readAttribute[KEY1DivElement.cpp]: unknown left line indent %s\n", value));
    }
    break;
  }
  case KEY1Token::tab_stops :
  {
    namespace spirit = boost::spirit;
    namespace qi =  boost::spirit::qi;
    using spirit::ascii::space;
    using spirit::qi::double_;
    using boost::phoenix::construct;

    qi::rule<std::string::const_iterator, IWORKTabStop(), spirit::ascii::space_type> tabStopRule =
      (qi::lit('C') >> qi::double_)[qi::_val=construct<IWORKTabStop>(IWORK_TABULATION_CENTER,qi::_1)] |
      (qi::lit('D') >> qi::double_)[qi::_val=construct<IWORKTabStop>(IWORK_TABULATION_DECIMAL,qi::_1)] |
      (qi::lit('L') >> qi::double_)[qi::_val=construct<IWORKTabStop>(IWORK_TABULATION_LEFT,qi::_1)] |
      (qi::lit('R') >> qi::double_)[qi::_val=construct<IWORKTabStop>(IWORK_TABULATION_RIGHT,qi::_1)];
    std::string val(value);
    std::string::const_iterator it = val.begin(), end = val.end();
    IWORKTabStops_t tabs;
    if (spirit::qi::phrase_parse(it, end, tabStopRule >> *(tabStopRule), space, tabs) && it==end)
      m_propMap.put<property::Tabs>(tabs);
    else
    {
      ETONYEK_DEBUG_MSG(("KEY1DivStyle::readAttribute[KEY1DivElement.cpp]: unknown tabs %s\n", value));
    }
    break;
  }
  default:
    return false;
  }
  return true;
}

IWORKStylePtr_t KEY1DivStyle::getStyle()
{
  if (!m_style)
    m_style.reset(new IWORKStyle(m_propMap, boost::none, m_parentStyle));
  return m_style;
}


KEY1DivElement::KEY1DivElement(KEY1ParserState &state, IWORKStylePtr_t spanStyle, IWORKStylePtr_t parentStyle, bool &delayedLineBreak)
  : KEY1XMLElementContextBase(state)
  , m_spanStyle(state, spanStyle)
  , m_style(state, parentStyle)
  , m_opened(false)
  , m_delayedLineBreak(delayedLineBreak)
{
}

void KEY1DivElement::attribute(const int name, const char *const value)
{
  if (m_style.readAttribute(name, value) || m_spanStyle.readAttribute(name, value))
    return;
  switch (name)
  {
  case KEY1Token::id :
    setId(value);
    break;
  default:
    ETONYEK_DEBUG_MSG(("KEY1DivElement::attribute: unknown attribute with value=%s\n", value));
  }
}

IWORKXMLContextPtr_t KEY1DivElement::element(const int name)
{
  switch (name)
  {
  case KEY1Token::span | KEY1Token::NS_URI_KEY :
    return makeContext<KEY1SpanElement>(getState(), m_spanStyle.getStyle(), m_delayedLineBreak);
  default:
    ETONYEK_DEBUG_MSG(("KEY1DivElement::element: unknown element\n"));
    break;
  }
  return IWORKXMLContextPtr_t();
}

void KEY1DivElement::CDATA(const char *value)
{
  ensureOpened();
  KEY1SpanElement::sendCDATA(value, getState().m_currentText, m_spanStyle.getStyle(), m_delayedLineBreak);
}

void KEY1DivElement::text(const char *const value)
{
  ensureOpened();
  if (bool(getState().m_currentText))
  {
    getState().m_currentText->setSpanStyle(m_spanStyle.getStyle());
    getState().m_currentText->insertText(value);
  }
}

void KEY1DivElement::endOfElement()
{
  m_delayedLineBreak=false;
  ensureOpened();
  if (bool(getState().m_currentText))
    getState().m_currentText->flushParagraph();
}

void KEY1DivElement::ensureOpened()
{
  if (m_delayedLineBreak)
  {
    if (bool(getState().m_currentText))
      getState().m_currentText->flushParagraph();
    m_delayedLineBreak=false;
  }
  if (!m_opened)
  {
    if (bool(getState().m_currentText))
      getState().m_currentText->setParagraphStyle(m_style.getStyle());
    m_opened = true;
  }
}
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
