/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKStrokeElement.h"

#include "IWORKColorElement.h"
#include "IWORKDictionary.h"
#include "IWORKRefContext.h"
#include "IWORKToken.h"
#include "IWORKTokenizer.h"
#include "IWORKXMLParserState.h"

#include "libetonyek_utils.h"
#include "libetonyek_xml.h"

namespace libetonyek
{


namespace
{

class ElementElement : public IWORKXMLEmptyContextBase
{
public:
  ElementElement(IWORKXMLParserState &state, boost::optional<double> &value);

private:
  void attribute(int name, const char *value) override;

private:
  boost::optional<double> &m_value;
};

ElementElement::ElementElement(IWORKXMLParserState &state, boost::optional<double> &value)
  : IWORKXMLEmptyContextBase(state)
  , m_value(value)
{
}

void ElementElement::attribute(const int name, const char *const value)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::val))
    m_value = double_cast(value);
}

}

namespace
{

class PatternContainerElement : public IWORKXMLElementContextBase
{
public:
  PatternContainerElement(IWORKXMLParserState &state, std::deque<double> &value);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  std::deque<double> &m_value;
  boost::optional<double> m_element;
};

PatternContainerElement::PatternContainerElement(IWORKXMLParserState &state, std::deque<double> &value)
  : IWORKXMLElementContextBase(state)
  , m_value(value)
  , m_element()
{
}

IWORKXMLContextPtr_t PatternContainerElement::element(const int name)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::element))
  {
    if (m_element)
    {
      m_value.push_back(get(m_element));
      m_element.reset();
    }
    return makeContext<ElementElement>(getState(), m_element);
  }

  return IWORKXMLContextPtr_t();
}

void PatternContainerElement::endOfElement()
{
  if (m_element)
    m_value.push_back(get(m_element));
}

}

namespace
{

class PatternElement : public IWORKXMLElementContextBase
{
public:
  PatternElement(IWORKXMLParserState &state, boost::optional<IWORKPattern> &pattern);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;
private:
  boost::optional<IWORKPattern> &m_pattern;
  boost::optional<ID_t> m_ref;
};

PatternElement::PatternElement(IWORKXMLParserState &state, boost::optional<IWORKPattern> &pattern)
  : IWORKXMLElementContextBase(state)
  , m_pattern(pattern)
  , m_ref()
{
  m_pattern=IWORKPattern();
}

void PatternElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::type:
    switch (getState().getTokenizer().getId(value))
    {
    case IWORKToken::empty :
      m_pattern->m_type = IWORK_STROKE_TYPE_NONE;
      break;
    case IWORKToken::solid :
      m_pattern->m_type = IWORK_STROKE_TYPE_SOLID;
      break;
    case IWORKToken::pattern :
      m_pattern->m_type = IWORK_STROKE_TYPE_DASHED;
      break;
    default :
      ETONYEK_DEBUG_MSG(("PatternElement::attribute[IWORKStrokeElement.cpp]: unknown pattern type %s\n", value));
      break;
    }
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::phase:
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::ID:
    return IWORKXMLElementContextBase::attribute(name, value);
  default:
    ETONYEK_DEBUG_MSG(("PatternElement::attribute[IWORKStrokeElement.cpp]: unknown attribute\n"));
  }
}

IWORKXMLContextPtr_t PatternElement::element(const int name)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::pattern))
    return makeContext<PatternContainerElement>(getState(), m_pattern->m_values);
  return IWORKXMLContextPtr_t();
}

void PatternElement::endOfElement()
{
  if (getId())
  {
    getState().getDictionary().m_patterns.insert(IWORKPatternMap_t::value_type(get(getId()),get(m_pattern)));
  }
}
}

IWORKStrokeElement::IWORKStrokeElement(IWORKXMLParserState &state, boost::optional<IWORKStroke> &value)
  : IWORKXMLElementContextBase(state)
  , m_value(value)
  , m_width()
  , m_color()
  , m_cap()
  , m_join()
  , m_pattern()
  , m_patternRef()
{
}

void IWORKStrokeElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::cap :
    switch (getState().getTokenizer().getId(value))
    {
    case IWORKToken::butt :
      m_cap = IWORK_LINE_CAP_BUTT;
      break;
    case IWORKToken::round :
      m_cap = IWORK_LINE_CAP_ROUND;
      break;
    default:
      ETONYEK_DEBUG_MSG(("IWORKStrokeElement::attribute[IWORKPropertyMapElement.cpp]: find unknown cap\n"));
    }
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::join :
    switch (getState().getTokenizer().getId(value))
    {
    case IWORKToken::miter :
      m_join = IWORK_LINE_JOIN_MITER;
      break;
    case IWORKToken::round :
      m_join = IWORK_LINE_JOIN_ROUND;
      break;
    default:
      ETONYEK_DEBUG_MSG(("IWORKStrokeElement::attribute[IWORKPropertyMapElement.cpp]: find unknown join\n"));
    }
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::width :
    m_width = double_cast(value);
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::ID :
    IWORKXMLElementContextBase::attribute(name, value);
    break;
  default:
    //ETONYEK_DEBUG_MSG(("IWORKStrokeElement::attribute[IWORKPropertyMapElement.cpp]: find unknown attribute\n"));
    break;
  }
}

IWORKXMLContextPtr_t IWORKStrokeElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::color :
    return makeContext<IWORKColorElement>(getState(), m_color);
  case IWORKToken::NS_URI_SF | IWORKToken::pattern :
    return makeContext<PatternElement>(getState(), m_pattern);
  case IWORKToken::NS_URI_SF | IWORKToken::pattern_ref :
    return makeContext<IWORKRefContext>(getState(), m_patternRef);
  default:
    ETONYEK_DEBUG_MSG(("IWORKStrokeElement::element[IWORKPropertyMapElement.cpp]: find unknown element\n"));
  }

  return IWORKXMLContextPtr_t();
}

void IWORKStrokeElement::endOfElement()
{
  if (m_patternRef)
  {
    IWORKPatternMap_t::const_iterator it = getState().getDictionary().m_patterns.find(get(m_patternRef));
    if (getState().getDictionary().m_patterns.end() != it)
      m_pattern=it->second;
    else if (!get(m_patternRef).empty())
    {
      ETONYEK_DEBUG_MSG(("IWORKStrokeElement::endOfElement: unknown pattern %s\n", get(m_patternRef).c_str()));
    }
  }
  if (m_width)
  {
    IWORKStroke value;
    value.m_width = get(m_width);
    if (m_cap) value.m_cap = get(m_cap);
    if (m_color) value.m_color = get(m_color);
    if (m_join) value.m_join = get(m_join);
    if (m_pattern) value.m_pattern=get(m_pattern);
    m_value=value;
    if (getId())
      getState().getDictionary().m_strokes[get(getId())]=value;
  }
}


}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
