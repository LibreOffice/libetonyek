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
#include "IWORKStyleContext.h"
#include "IWORKStyleRefContext.h"
#include "IWORKToken.h"
#include "IWORKTokenizer.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

using boost::any;
using boost::lexical_cast;
using boost::none;
using boost::optional;

using std::string;

namespace
{

class PropertyContextBase : public IWORKXMLElementContextBase
{
protected:
  PropertyContextBase(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

protected:
  IWORKPropertyMap &m_propMap;
};

PropertyContextBase::PropertyContextBase(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : IWORKXMLElementContextBase(state)
  , m_propMap(propMap)
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
  if (m_propId == name)
    return makeContext<ContextT>(getState(), m_value);

  return IWORKXMLContextPtr_t();
}

template<class ContextT, class PropertyT>
void DirectPropertyContextBase<ContextT, PropertyT>::endOfElement()
{
  if (bool(m_value))
    m_propMap.put<PropertyT>(m_value);
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
  if (m_propId == name)
    return makeContext<ContextT>(getState(), m_value);

  return IWORKXMLContextPtr_t();
}

template<class ContextT, class PropertyT>
void ValuePropertyContextBase<ContextT, PropertyT>::endOfElement()
{
  if (bool(m_value))
    m_propMap.put<PropertyT>(get(m_value));
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

class StringElement : public IWORKXMLEmptyContextBase
{
public:
  StringElement(IWORKXMLParserState &state, optional<string> &str);

private:
  virtual void attribute(int name, const char *value);

private:
  optional<string> &m_string;
};

StringElement::StringElement(IWORKXMLParserState &state, optional<string> &str)
  : IWORKXMLEmptyContextBase(state)
  , m_string(str)
{
}

void StringElement::attribute(const int name, const char *const value)
{
  if ((IWORKToken::NS_URI_SFA | IWORKToken::string) == name)
    m_string = value;
}

}

namespace
{

class FontNameElement : public ValuePropertyContextBase<StringElement, property::FontName>
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

class TabstopElement : public IWORKXMLEmptyContextBase
{
public:
  TabstopElement(IWORKXMLParserState &state, optional<double> &pos);

private:
  virtual void attribute(int name, const char *value);

private:
  optional<double> &m_pos;
};

TabstopElement::TabstopElement(IWORKXMLParserState &state, optional<double> &pos)
  : IWORKXMLEmptyContextBase(state)
  , m_pos(pos)
{
}

void TabstopElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::align :
    // TODO: parse
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::pos :
    m_pos = lexical_cast<double>(value);
    break;
  default :
    break;
  }
}

}

namespace
{

class TabsElement : public IWORKXMLElementContextBase
{
public:
  TabsElement(IWORKXMLParserState &state, IWORKTabStops_t &tabs);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  IWORKTabStops_t &m_tabs;
  optional<double> m_current;
};

TabsElement::TabsElement(IWORKXMLParserState &state, IWORKTabStops_t &tabs)
  : IWORKXMLElementContextBase(state)
  , m_tabs(tabs)
  , m_current()
{
}

IWORKXMLContextPtr_t TabsElement::element(const int name)
{
  if (m_current)
  {
    m_tabs.push_back(IWORKTabStop(get(m_current)));
    m_current.reset();
  }

  if ((IWORKToken::NS_URI_SF | IWORKToken::tabstop) == name)
    return makeContext<TabstopElement>(getState(), m_current);

  return IWORKXMLContextPtr_t();
}

void TabsElement::endOfElement()
{
  if (m_current)
    m_tabs.push_back(IWORKTabStop(get(m_current)));

  if (getId())
    getState().getDictionary().m_tabs[get(getId())] = m_tabs;
}

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
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::tabs :
    return makeContext<TabsElement>(getState(), m_tabs);
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
}

}

namespace
{

class StylePropertyElement : public PropertyContextBase
{
public:
  StylePropertyElement(IWORKXMLParserState &state, int id, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  const int m_id;
};

StylePropertyElement::StylePropertyElement(IWORKXMLParserState &state, const int id, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
  , m_id(id)
{
}

IWORKXMLContextPtr_t StylePropertyElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle :
  case IWORKToken::NS_URI_SF | IWORKToken::liststyle :
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle :
  case IWORKToken::NS_URI_SF | IWORKToken::vector_style :
    return makeContext<IWORKStyleContext>(getState(), name, true);
  case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle_ref :
  case IWORKToken::NS_URI_SF | IWORKToken::liststyle_ref :
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle_ref :
  case IWORKToken::NS_URI_SF | IWORKToken::vector_style_ref :
    return makeContext<IWORKStyleRefContext>(getState(), name, true, true);
  }

  return IWORKXMLContextPtr_t();
}

void StylePropertyElement::endOfElement()
{
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
  if ((IWORKToken::NS_URI_SF | IWORKToken::string) == name)
    return makeContext<StringElement>(getState(), m_lang);
  return IWORKXMLContextPtr_t();
}

void LanguageElement::endOfElement()
{
  if (m_lang)
  {
    if (IWORKToken::__multilingual != getToken(get(m_lang).c_str()))
      m_propMap.put<property::Language>(get(m_lang));
  }
}

}

namespace
{

typedef NumericPropertyBase<bool, property::Bold> BoldElement;
typedef NumericPropertyBase<bool, property::Italic> ItalicElement;
typedef NumericPropertyBase<bool, property::KeepLinesTogether> KeepLinesTogetherElement;
typedef NumericPropertyBase<bool, property::KeepWithNext> KeepWithNextElement;
typedef NumericPropertyBase<bool, property::Outline> OutlineElement;
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
  case IWORKToken::NS_URI_SF | IWORKToken::SFTCellStylePropertyLayoutStyle :
  case IWORKToken::NS_URI_SF | IWORKToken::SFTCellStylePropertyParagraphStyle :
  case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyBorderVectorStyle :
  case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyCellLayoutStyle :
  case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyCellParagraphStyle :
  case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyCellStyle :
  case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyHeaderBorderVectorStyle :
  case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyHeaderColumnCellLayoutStyle :
  case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyHeaderColumnCellParagraphStyle :
  case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyHeaderColumnCellStyle :
  case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyHeaderRowCellLayoutStyle :
  case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyHeaderRowCellParagraphStyle :
  case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyHeaderRowCellStyle :
  case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyHeaderSeperatorVectorStyle :
  case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyHeaderVectorStyle :
  case IWORKToken::NS_URI_SF | IWORKToken::SFTableStylePropertyVectorStyle :
  case IWORKToken::NS_URI_SF | IWORKToken::TableCellStylePropertyFormatNegativeStyle :
  case IWORKToken::NS_URI_SF | IWORKToken::bulletListStyle :
  case IWORKToken::NS_URI_SF | IWORKToken::followingLayoutStyle :
  case IWORKToken::NS_URI_SF | IWORKToken::followingParagraphStyle :
  case IWORKToken::NS_URI_SF | IWORKToken::layoutParagraphStyle :
  case IWORKToken::NS_URI_SF | IWORKToken::layoutStyle :
  case IWORKToken::NS_URI_SF | IWORKToken::listStyle :
  case IWORKToken::NS_URI_SF | IWORKToken::tocStyle :
    return makeContext<StylePropertyElement>(getState(), name, m_propMap);

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
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphBorderType :
    return makeContext<ParagraphBorderTypeElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphFill :
    return makeContext<ParagraphFillElement>(getState(), m_propMap);
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
