/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKPropertyMapElement.h"

#include <boost/lexical_cast.hpp>

#include "libetonyek_xml.h"
#include "IWORKCollector.h"
#include "IWORKColorElement.h"
#include "IWORKDictionary.h"
#include "IWORKGeometryElement.h"
#include "IWORKProperties.h"
#include "IWORKRefContext.h"
#include "IWORKStringElement.h"
#include "IWORKStyleContext.h"
#include "IWORKStyleRefContext.h"
#include "IWORKTabsElement.h"
#include "IWORKToken.h"
#include "IWORKTokenizer.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

using boost::any;
using boost::lexical_cast;
using boost::none;
using boost::optional;

using std::deque;
using std::string;

namespace
{

class PropertyContextBase : public IWORKXMLElementContextBase
{
protected:
  PropertyContextBase(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

protected:
  IWORKPropertyMap &m_propMap;
  bool m_default;
};

PropertyContextBase::PropertyContextBase(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : IWORKXMLElementContextBase(state)
  , m_propMap(propMap)
  , m_default(true)
{
}

}

namespace
{

template<typename T>
struct NumberConverter
{
  static T convert(const char *value);
};

template<>
struct NumberConverter<bool>
{
  static optional<bool> convert(const char *const value)
  {
    return try_bool_cast(value);
  }
};

template<>
struct NumberConverter<int>
{
  static optional<int> convert(const char *const value)
  {
    return try_int_cast(value);
  }
};

template<>
struct NumberConverter<double>
{
  static optional<double> convert(const char *const value)
  {
    return try_double_cast(value);
  }
};

template<typename T>
class NumberElement : public IWORKXMLEmptyContextBase
{
public:
  NumberElement(IWORKXMLParserState &state, optional<T> &value);

private:
  virtual void attribute(int name, const char *value);

private:
  optional<T> &m_value;
};

template<typename T>
NumberElement<T>::NumberElement(IWORKXMLParserState &state, optional<T> &value)
  : IWORKXMLEmptyContextBase(state)
  , m_value(value)
{
}

template<typename T>
void NumberElement<T>::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SFA | IWORKToken::number :
    m_value = NumberConverter<T>::convert(value);
    break;
  }
}

}

namespace
{

template<class ContextT, class PropertyT>
class DirectPropertyContextBase : public PropertyContextBase
{
public:
  DirectPropertyContextBase(IWORKXMLParserState &state, IWORKPropertyMap &propMap, int propId);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  const int m_propId;
  typename IWORKPropertyInfo<PropertyT>::ValueType m_value;
};

template<class ContextT, class PropertyT>
DirectPropertyContextBase<ContextT, PropertyT>::DirectPropertyContextBase(IWORKXMLParserState &state, IWORKPropertyMap &propMap, const int propId)
  : PropertyContextBase(state, propMap)
  , m_propId(propId)
  , m_value()
{
}

template<class ContextT, class PropertyT>
IWORKXMLContextPtr_t DirectPropertyContextBase<ContextT, PropertyT>::element(const int name)
{
  m_default = false;

  if (m_propId == name)
    return makeContext<ContextT>(getState(), m_value);

  return IWORKXMLContextPtr_t();
}

template<class ContextT, class PropertyT>
void DirectPropertyContextBase<ContextT, PropertyT>::endOfElement()
{
  if (bool(m_value))
    m_propMap.put<PropertyT>(m_value);
  else if (m_default)
    m_propMap.clear<PropertyT>();
}

}

namespace
{

template<class ContextT, class PropertyT>
class ValuePropertyContextBase : public PropertyContextBase
{
public:
  ValuePropertyContextBase(IWORKXMLParserState &state, IWORKPropertyMap &propMap, int propId);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  const int m_propId;
  optional<typename IWORKPropertyInfo<PropertyT>::ValueType> m_value;
};

template<class ContextT, class PropertyT>
ValuePropertyContextBase<ContextT, PropertyT>::ValuePropertyContextBase(IWORKXMLParserState &state, IWORKPropertyMap &propMap, const int propId)
  : PropertyContextBase(state, propMap)
  , m_propId(propId)
  , m_value()
{
}

template<class ContextT, class PropertyT>
IWORKXMLContextPtr_t ValuePropertyContextBase<ContextT, PropertyT>::element(const int name)
{
  m_default = false;

  if (m_propId == name)
    return makeContext<ContextT>(getState(), m_value);

  return IWORKXMLContextPtr_t();
}

template<class ContextT, class PropertyT>
void ValuePropertyContextBase<ContextT, PropertyT>::endOfElement()
{
  if (bool(m_value))
    m_propMap.put<PropertyT>(get(m_value));
  else if (m_default)
    m_propMap.clear<PropertyT>();
}

}

namespace
{

template<typename ValueT, class PropertyT>
class NumericPropertyBase : public ValuePropertyContextBase<NumberElement<ValueT>, PropertyT>
{
  typedef ValuePropertyContextBase<NumberElement<ValueT>, PropertyT> Parent_t;

public:
  NumericPropertyBase(IWORKXMLParserState &state, IWORKPropertyMap &propMap);
};

template<typename ValueT, class PropertyT>
NumericPropertyBase<ValueT, PropertyT>::NumericPropertyBase(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : Parent_t(state, propMap, IWORKToken::NS_URI_SF | IWORKToken::number)
{
}

}

namespace
{

template<>
struct NumberConverter<IWORKAlignment>
{
  static optional<IWORKAlignment> convert(const char *const value)
  {
    const optional<int> alignment(try_int_cast(value));
    if (alignment)
    {
      switch (get(alignment))
      {
      case 0 :
        return IWORK_ALIGNMENT_LEFT;
      case 1 :
        return IWORK_ALIGNMENT_RIGHT;
      case 2 :
        return IWORK_ALIGNMENT_CENTER;
      case 3 :
        return IWORK_ALIGNMENT_JUSTIFY;
      case 4 : // TODO: what is this?
        break;
      }
    }

    return none;
  }
};

}

namespace
{

class FontColorElement : public ValuePropertyContextBase<IWORKColorElement, property::FontColor>
{
public:
  FontColorElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap);
};

FontColorElement::FontColorElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : ValuePropertyContextBase(state, propMap, IWORKToken::NS_URI_SF | IWORKToken::color)
{
}

}

namespace
{

class GeometryElement : public DirectPropertyContextBase<IWORKGeometryElement, property::Geometry>
{
public:
  GeometryElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap);
};

GeometryElement::GeometryElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : DirectPropertyContextBase(state, propMap, IWORKToken::NS_URI_SF | IWORKToken::geometry)
{
}

}

namespace
{

template<>
struct NumberConverter<IWORKCapitalization>
{
  static optional<IWORKCapitalization> convert(const char *const value)
  {
    const optional<int> capitalization(try_int_cast(value));
    if (capitalization)
    {
      switch (get(capitalization))
      {
      case 0 :
        return IWORK_CAPITALIZATION_NONE;
      case 1 :
        return IWORK_CAPITALIZATION_ALL_CAPS;
      case 2 :
        return IWORK_CAPITALIZATION_SMALL_CAPS;
      case 3 :
        return IWORK_CAPITALIZATION_TITLE;
      }
    }

    return none;
  }
};

}

namespace
{

class FontNameElement : public ValuePropertyContextBase<IWORKStringElement, property::FontName>
{
public:
  FontNameElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap);
};

FontNameElement::FontNameElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : ValuePropertyContextBase(state, propMap, IWORKToken::NS_URI_SF | IWORKToken::string)
{
}

}

namespace
{

template<>
struct NumberConverter<IWORKBaseline>
{
  static optional<IWORKBaseline> convert(const char *const value)
  {
    const optional<int> superscript(try_int_cast(value));
    if (superscript)
    {
      switch (get(superscript))
      {
      case 1 :
        return IWORK_BASELINE_SUPER;
      case 2 :
        return IWORK_BASELINE_SUB;
      }
    }

    return none;
  }
};

}

namespace
{

class TabsProperty : public PropertyContextBase
{
public:
  TabsProperty(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  IWORKTabStops_t m_tabs;
  optional<ID_t> m_ref;
};

TabsProperty::TabsProperty(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
  , m_tabs()
  , m_ref()
{
}

IWORKXMLContextPtr_t TabsProperty::element(const int name)
{
  m_default = false;

  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::tabs :
    return makeContext<IWORKTabsElement>(getState(), m_tabs);
  case IWORKToken::NS_URI_SF | IWORKToken::tabs_ref :
    return makeContext<IWORKRefContext>(getState(), m_ref);
  }

  return IWORKXMLContextPtr_t();
}

void TabsProperty::endOfElement()
{
  if (!m_tabs.empty())
  {
    m_propMap.put<property::Tabs>(m_tabs);
  }
  else if (m_ref)
  {
    IWORKTabStopsMap_t::const_iterator it = getState().getDictionary().m_tabs.find(get(m_ref));
    if (getState().getDictionary().m_tabs.end() != it)
      m_propMap.put<property::Tabs>(it->second);
  }
  else if (m_default)
  {
    m_propMap.clear<property::Tabs>();
  }
}

}

namespace
{

class TextBackgroundElement : public ValuePropertyContextBase<IWORKColorElement, property::TextBackground>
{
public:
  TextBackgroundElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap);
};

TextBackgroundElement::TextBackgroundElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : ValuePropertyContextBase(state, propMap, IWORKToken::NS_URI_SF | IWORKToken::color)
{
}

}

namespace
{

class LinespacingElement : public IWORKXMLEmptyContextBase
{
public:
  LinespacingElement(IWORKXMLParserState &state, optional<IWORKLineSpacing> &value);

private:
  virtual void attribute(int name, const char *value);
  virtual void endOfElement();

private:
  optional<IWORKLineSpacing> &m_value;
  optional<double> m_amount;
  optional<bool> m_relative;
};

LinespacingElement::LinespacingElement(IWORKXMLParserState &state, optional<IWORKLineSpacing> &value)
  : IWORKXMLEmptyContextBase(state)
  , m_value(value)
  , m_amount()
  , m_relative()
{
}

void LinespacingElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::amt :
    m_amount = double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::mode :
    m_relative = IWORKToken::relative == getToken(value);
    break;
  }
}

void LinespacingElement::endOfElement()
{
  if (m_amount)
    m_value = IWORKLineSpacing(get(m_amount), get_optional_value_or(m_relative, false));
}

}

namespace
{

class LineSpacingElement : public ValuePropertyContextBase<LinespacingElement, property::LineSpacing>
{
public:
  LineSpacingElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap);
};

LineSpacingElement::LineSpacingElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : ValuePropertyContextBase(state, propMap, IWORKToken::NS_URI_SF | IWORKToken::linespacing)
{
}

}

namespace
{

class ParagraphFillElement : public ValuePropertyContextBase<IWORKColorElement, property::ParagraphFill>
{
public:
  ParagraphFillElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap);
};

ParagraphFillElement::ParagraphFillElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : ValuePropertyContextBase(state, propMap, IWORKToken::NS_URI_SF | IWORKToken::color)
{
}

}

namespace
{

class ElementElement : public IWORKXMLEmptyContextBase
{
public:
  ElementElement(IWORKXMLParserState &state, optional<double> &value);

private:
  virtual void attribute(int name, const char *value);

private:
  optional<double> &m_value;
};

ElementElement::ElementElement(IWORKXMLParserState &state, optional<double> &value)
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
  PatternContainerElement(IWORKXMLParserState &state, deque<double> &value);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  deque<double> &m_value;
  optional<double> m_element;
};

PatternContainerElement::PatternContainerElement(IWORKXMLParserState &state, deque<double> &value)
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
  PatternElement(IWORKXMLParserState &state, deque<double> &value);

private:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  deque<double> &m_value;
};

PatternElement::PatternElement(IWORKXMLParserState &state, deque<double> &value)
  : IWORKXMLElementContextBase(state)
  , m_value(value)
{
}

IWORKXMLContextPtr_t PatternElement::element(const int name)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::pattern))
    return makeContext<PatternContainerElement>(getState(), m_value);
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class StrokeElement : public IWORKXMLElementContextBase
{
public:
  StrokeElement(IWORKXMLParserState &state, optional<IWORKStroke> &value);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<IWORKStroke> &m_value;
  optional<double> m_width;
  optional<IWORKColor> m_color;
  deque<double> m_pattern;
};

StrokeElement::StrokeElement(IWORKXMLParserState &state, optional<IWORKStroke> &value)
  : IWORKXMLElementContextBase(state)
  , m_value(value)
  , m_width()
  , m_color()
  , m_pattern()
{
}

void StrokeElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::width :
    m_width = double_cast(value);
    break;
  }
}

IWORKXMLContextPtr_t StrokeElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::color :
    return makeContext<IWORKColorElement>(getState(), m_color);
  case IWORKToken::NS_URI_SF | IWORKToken::pattern :
    return makeContext<PatternElement>(getState(), m_pattern);
  }

  return IWORKXMLContextPtr_t();
}

void StrokeElement::endOfElement()
{
  if (m_width)
  {
    m_value = IWORKStroke();
    IWORKStroke &value = get(m_value);
    value.m_width = get(m_width);
    if (m_color)
      value.m_color = get(m_color);
    value.m_pattern = m_pattern;
  }
}

}

namespace
{

class ParagraphStrokeElement : public ValuePropertyContextBase<StrokeElement, property::ParagraphStroke>
{
public:
  ParagraphStrokeElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap);
};

ParagraphStrokeElement::ParagraphStrokeElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : ValuePropertyContextBase(state, propMap, IWORKToken::NS_URI_SF | IWORKToken::stroke)
{
}

}

namespace
{

template<>
struct NumberConverter<IWORKBorderType>
{
  static optional<IWORKBorderType> convert(const char *const value)
  {
    switch (int_cast(value))
    {
    case 1 :
      return IWORK_BORDER_TYPE_TOP;
    case 2 :
      return IWORK_BORDER_TYPE_BOTTOM;
    case 3 :
      return IWORK_BORDER_TYPE_TOP_AND_BOTTOM;
    case 4 :
      return IWORK_BORDER_TYPE_ALL;
    }
    return optional<IWORKBorderType>();
  }
};

}

namespace
{

class LanguageElement : public PropertyContextBase
{
public:
  LanguageElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<string> m_lang;
};

LanguageElement::LanguageElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
{
}

IWORKXMLContextPtr_t LanguageElement::element(const int name)
{
  m_default = false;
  if ((IWORKToken::NS_URI_SF | IWORKToken::string) == name)
    return makeContext<IWORKStringElement>(getState(), m_lang);
  return IWORKXMLContextPtr_t();
}

void LanguageElement::endOfElement()
{
  if (m_lang)
  {
    if (IWORKToken::__multilingual != getToken(get(m_lang).c_str()))
      m_propMap.put<property::Language>(get(m_lang));
  }
  else if (m_default)
    m_propMap.clear<property::Language>();
}

}

namespace
{

typedef NumericPropertyBase<bool, property::Bold> BoldElement;
typedef NumericPropertyBase<bool, property::Italic> ItalicElement;
typedef NumericPropertyBase<bool, property::KeepLinesTogether> KeepLinesTogetherElement;
typedef NumericPropertyBase<bool, property::KeepWithNext> KeepWithNextElement;
typedef NumericPropertyBase<bool, property::Outline> OutlineElement;
typedef NumericPropertyBase<bool, property::PageBreakBefore> PageBreakBeforeElement;
typedef NumericPropertyBase<bool, property::Strikethru> StrikethruElement;
typedef NumericPropertyBase<bool, property::Underline> UnderlineElement;
typedef NumericPropertyBase<bool, property::WidowControl> WidowControlElement;
typedef NumericPropertyBase<double, property::BaselineShift> BaselineShiftElement;
typedef NumericPropertyBase<double, property::FirstLineIndent> FirstLineIndentElement;
typedef NumericPropertyBase<double, property::FontSize> FontSizeElement;
typedef NumericPropertyBase<double, property::LeftIndent> LeftIndentElement;
typedef NumericPropertyBase<double, property::RightIndent> RightIndentElement;
typedef NumericPropertyBase<double, property::SpaceAfter> SpaceAfterElement;
typedef NumericPropertyBase<double, property::SpaceBefore> SpaceBeforeElement;
typedef NumericPropertyBase<double, property::Tracking> TrackingElement;
typedef NumericPropertyBase<IWORKAlignment, property::Alignment> AlignmentElement;
typedef NumericPropertyBase<IWORKBaseline, property::Baseline> SuperscriptElement;
typedef NumericPropertyBase<IWORKBorderType, property::ParagraphBorderType> ParagraphBorderTypeElement;
typedef NumericPropertyBase<IWORKCapitalization, property::Capitalization> CapitalizationElement;

}

IWORKPropertyMapElement::IWORKPropertyMapElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : IWORKXMLElementContextBase(state)
  , m_propMap(propMap)
{
}

IWORKXMLContextPtr_t IWORKPropertyMapElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::alignment :
    return makeContext<AlignmentElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::baselineShift :
    return makeContext<BaselineShiftElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::bold :
    return makeContext<BoldElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::capitalization :
    return makeContext<CapitalizationElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::firstLineIndent :
    return makeContext<FirstLineIndentElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::fontColor :
    return makeContext<FontColorElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::fontName :
    return makeContext<FontNameElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::fontSize :
    return makeContext<FontSizeElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<GeometryElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::italic :
    return makeContext<ItalicElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::keepLinesTogether :
    return makeContext<KeepLinesTogetherElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::keepWithNext :
    return makeContext<KeepWithNextElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::language :
    return makeContext<LanguageElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::leftIndent :
    return makeContext<LeftIndentElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::lineSpacing :
    return makeContext<LineSpacingElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::outline :
    return makeContext<OutlineElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::pageBreakBefore :
    return makeContext<PageBreakBeforeElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphBorderType :
    return makeContext<ParagraphBorderTypeElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphFill :
    return makeContext<ParagraphFillElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphStroke :
    return makeContext<ParagraphStrokeElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::rightIndent :
    return makeContext<RightIndentElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::spaceAfter :
    return makeContext<SpaceAfterElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::spaceBefore :
    return makeContext<SpaceBeforeElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::strikethru :
    return makeContext<StrikethruElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::superscript :
    return makeContext<SuperscriptElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::tabs :
    return makeContext<TabsProperty>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::textBackground :
    return makeContext<TextBackgroundElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::tracking :
    return makeContext<TrackingElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::underline :
    return makeContext<UnderlineElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::widowControl :
    return makeContext<WidowControlElement>(getState(), m_propMap);
  }

  return IWORKXMLContextPtr_t();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
