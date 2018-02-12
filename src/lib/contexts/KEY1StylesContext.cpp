/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <boost/spirit/include/qi.hpp>

#include "KEY1StylesContext.h"

#include "IWORKProperties.h"
#include "IWORKTokenizer.h"

#include "KEY1Dictionary.h"
#include "KEY1FillElement.h"
#include "KEY1StringConverter.h"
#include "KEY1Token.h"
#include "KEY1ParserState.h"

#include "libetonyek_xml.h"

namespace libetonyek
{
namespace
{
class MarkerStyleElement : public KEY1XMLElementContextBase
{
public:
  MarkerStyleElement(KEY1ParserState &state, boost::optional<IWORKMarker> &m_marker);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  boost::optional<IWORKMarker> &m_marker;
};

MarkerStyleElement::MarkerStyleElement(KEY1ParserState &state, boost::optional<IWORKMarker> &marker)
  : KEY1XMLElementContextBase(state)
  , m_marker(marker)
{
  m_marker=IWORKMarker();
}

void MarkerStyleElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case KEY1Token::is_filled :
    get(m_marker).m_filled = bool_cast(value);
    break;
  case KEY1Token::id :
    setId(value);
    break;
  case KEY1Token::match_point :
    get(m_marker).m_endPoint=KEY1StringConverter<IWORKPosition>::convert(value);
    break;
  case KEY1Token::path :
    get(m_marker).m_path=value;
    break;
  default :
    ETONYEK_DEBUG_MSG(("MarkerStyleElement::attribute[KEY1StylesContext.cpp]: unexpected attribute\n"));
    break;
  }
}

IWORKXMLContextPtr_t MarkerStyleElement::element(const int /*name*/)
{
  ETONYEK_DEBUG_MSG(("MarkerStyleElement::element[KEY1StylesContext.cpp]: unknown element\n"));

  return IWORKXMLContextPtr_t();
}

void MarkerStyleElement::endOfElement()
{
  // todo safe marker
}
}

namespace
{
class PatternStyleElement : public KEY1XMLElementContextBase
{
public:
  PatternStyleElement(KEY1ParserState &state, boost::optional<IWORKPattern> &m_pattern);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  boost::optional<IWORKPattern> &m_pattern;
};

PatternStyleElement::PatternStyleElement(KEY1ParserState &state, boost::optional<IWORKPattern> &pattern)
  : KEY1XMLElementContextBase(state)
  , m_pattern(pattern)
{
  m_pattern=IWORKPattern();
}

void PatternStyleElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case KEY1Token::pattern:
    switch (getState().getTokenizer().getId(value))
    {
    case KEY1Token::none :
      m_pattern->m_type = IWORK_STROKE_TYPE_NONE;
      break;
    case KEY1Token::solid :
      m_pattern->m_type = IWORK_STROKE_TYPE_SOLID;
      break;
    default :
    {
      namespace spirit = boost::spirit;
      using spirit::ascii::space;
      using spirit::ascii::char_;
      using spirit::qi::double_;
      std::string val(value);
      std::string::const_iterator it = val.begin(), end = val.end();
      if (spirit::qi::phrase_parse(it, end, double_ >> *(double_), space, m_pattern->m_values) && it==end)
        m_pattern->m_type = IWORK_STROKE_TYPE_DASHED;
      else
      {
        ETONYEK_DEBUG_MSG(("PatternStyleElement::attribute[KEY1StylesContext.cpp]: can not parse pattern %s\n", value));
        m_pattern->m_values.clear();
      }
      break;
    }
    }
    break;
  case KEY1Token::id :
    setId(value);
    break;
  default :
    ETONYEK_DEBUG_MSG(("PatternStyleElement::attribute[KEY1StylesContext.cpp]: unexpected attribute\n"));
    break;
  }
}

IWORKXMLContextPtr_t PatternStyleElement::element(const int /*name*/)
{
  ETONYEK_DEBUG_MSG(("PatternStyleElement::element[KEY1StylesContext.cpp]: unknown element\n"));

  return IWORKXMLContextPtr_t();
}

void PatternStyleElement::endOfElement()
{
  if (getId())
    getState().getDictionary().m_patterns[get(getId())]=get(m_pattern);
}
}

namespace
{
class ShadowStyleElement : public KEY1XMLElementContextBase
{
public:
  ShadowStyleElement(KEY1ParserState &state, boost::optional<IWORKShadow> &m_shadow);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  boost::optional<IWORKShadow> &m_shadow;
};

ShadowStyleElement::ShadowStyleElement(KEY1ParserState &state, boost::optional<IWORKShadow> &shadow)
  : KEY1XMLElementContextBase(state)
  , m_shadow(shadow)
{
  m_shadow=IWORKShadow();
}

void ShadowStyleElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case KEY1Token::angle :
    get(m_shadow).m_angle=double_cast(value);
    break;
  case KEY1Token::color :
  {
    boost::optional<IWORKColor> color=KEY1StringConverter<IWORKColor>::convert(value);
    if (color) get(m_shadow).m_color=get(color);
    break;
  }
  case KEY1Token::id :
    setId(value);
    break;
  case KEY1Token::offset :
    get(m_shadow).m_offset=double_cast(value);
    break;
  case KEY1Token::opacity :
    get(m_shadow).m_opacity=double_cast(value);
    break;
  case KEY1Token::radius :
    get(m_shadow).m_radius=double_cast(value);
    break;
  default :
    ETONYEK_DEBUG_MSG(("ShadowStyleElement::attribute[KEY1StylesContext.cpp]: unexpected attribute\n"));
    break;
  }
}

IWORKXMLContextPtr_t ShadowStyleElement::element(const int /*name*/)
{
  ETONYEK_DEBUG_MSG(("ShadowStyleElement::element[KEY1StylesContext.cpp]: unknown element\n"));

  return IWORKXMLContextPtr_t();
}

void ShadowStyleElement::endOfElement()
{
  if (getId())
    getState().getDictionary().m_shadows[get(getId())]=get(m_shadow);
}
}

KEY1StylesContext::KEY1StylesContext(KEY1ParserState &state, IWORKStylePtr_t &style, IWORKStylePtr_t parentStyle)
  : KEY1XMLElementContextBase(state)
  , m_style(style)
  , m_propMap()
  , m_parentStyle(parentStyle)
  , m_fill()
  , m_lineHead()
  , m_lineTail()
  , m_pattern()
  , m_shadow()
{
}

IWORKXMLContextPtr_t KEY1StylesContext::element(const int name)
{
  switch (name)
  {
  case KEY1Token::dash_style | KEY1Token::NS_URI_KEY :
    return makeContext<PatternStyleElement>(getState(), m_pattern);
  case KEY1Token::fill_style | KEY1Token::NS_URI_KEY :
    return makeContext<KEY1FillElement>(getState(), m_fill);
  case KEY1Token::line_head_style | KEY1Token::NS_URI_KEY :
    return makeContext<MarkerStyleElement>(getState(), m_lineHead);
  case KEY1Token::line_tail_style | KEY1Token::NS_URI_KEY :
    return makeContext<MarkerStyleElement>(getState(), m_lineTail);
  case KEY1Token::shadow_style | KEY1Token::NS_URI_KEY :
    return makeContext<ShadowStyleElement>(getState(), m_shadow);
  default :
    ETONYEK_DEBUG_MSG(("KEY1StylesContext::element[KEY1StylesContext.cpp]: unknown element\n"));
    break;
  }
  return IWORKXMLContextPtr_t();
}

void KEY1StylesContext::endOfElement()
{
  if (m_fill)
    m_propMap.put<property::Fill>(get(m_fill));
  if (m_lineHead)
    m_propMap.put<property::HeadLineEnd>(get(m_lineHead));
  if (m_lineTail)
    m_propMap.put<property::TailLineEnd>(get(m_lineTail));
  if (m_pattern)
  {
    IWORKStroke stroke;
    if (m_parentStyle && m_parentStyle->has<property::Stroke>())
      stroke=m_parentStyle->get<property::Stroke>();
    stroke.m_pattern=get(m_pattern);
    m_propMap.put<property::Stroke>(stroke);
  }
  if (m_shadow)
    m_propMap.put<property::Shadow>(get(m_shadow));
  m_style.reset(new IWORKStyle(m_propMap, boost::none, m_parentStyle));
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
