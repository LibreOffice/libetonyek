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
#include "IWORKStyleContext.h"
#include "IWORKStyleRefContext.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"
#include "IWORKToken.h"

namespace libetonyek
{

using boost::any;
using boost::lexical_cast;
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
  static bool convert(const char *const value)
  {
    return bool_cast(value);
  }
};

template<>
struct NumberConverter<int>
{
  static int convert(const char *const value)
  {
    return int_cast(value);
  }
};

template<>
struct NumberConverter<double>
{
  static double convert(const char *const value)
  {
    return double_cast(value);
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

class AlignmentElement : public PropertyContextBase
{
public:
  AlignmentElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<int> m_value;
};

AlignmentElement::AlignmentElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
{
}

IWORKXMLContextPtr_t AlignmentElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::number :
    return makeContext<NumberElement<int> >(getState(), m_value);
  }

  return IWORKXMLContextPtr_t();
}

void AlignmentElement::endOfElement()
{
  IWORKAlignment prop;

  if (m_value)
  {
    switch (get(m_value))
    {
    case 0 :
      prop = IWORK_ALIGNMENT_LEFT;
      break;
    case 1 :
      prop = IWORK_ALIGNMENT_RIGHT;
      break;
    case 2 :
      prop = IWORK_ALIGNMENT_CENTER;
      break;
    case 3 :
      prop = IWORK_ALIGNMENT_JUSTIFY;
      break;
    default :
      ETONYEK_DEBUG_MSG(("unknown alignment %d\n", get(m_value)));
    }
  }

  m_propMap.put<property::Alignment>(prop);
}

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

class BoldElement : public ValuePropertyContextBase<NumberElement<bool>, property::Bold>
{
public:
  BoldElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap);
};

BoldElement::BoldElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : ValuePropertyContextBase(state, propMap, IWORKToken::NS_URI_SF | IWORKToken::number)
{
}

}

namespace
{

class CapitalizationElement : public PropertyContextBase
{
public:
  CapitalizationElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<int> m_capitalization;
};

CapitalizationElement::CapitalizationElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
{
}

IWORKXMLContextPtr_t CapitalizationElement::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::number) == name)
    return makeContext<NumberElement<int> >(getState(), m_capitalization);

  return IWORKXMLContextPtr_t();
}

void CapitalizationElement::endOfElement()
{
  if (m_capitalization)
  {
    IWORKCapitalization prop;
    switch (get(m_capitalization))
    {
    case 0 :
      prop = IWORK_CAPITALIZATION_NONE;
      break;
    case 1 :
      prop = IWORK_CAPITALIZATION_ALL_CAPS;
      break;
    case 2 :
      prop = IWORK_CAPITALIZATION_SMALL_CAPS;
      break;
    case 3 :
      prop = IWORK_CAPITALIZATION_TITLE;
      break;
    default :
      ETONYEK_DEBUG_MSG(("unknown capitalization %d\n", get(m_capitalization)));
    }

    m_propMap.put<property::Capitalization>(prop);
  }
}

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

class FontSizeElement : public PropertyContextBase
{
public:
  FontSizeElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<double> m_fontSize;
};

FontSizeElement::FontSizeElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
{
}

IWORKXMLContextPtr_t FontSizeElement::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::number) == name)
    return makeContext<NumberElement<double> >(getState(), m_fontSize);

  return IWORKXMLContextPtr_t();
}

void FontSizeElement::endOfElement()
{
  if (m_fontSize)
    m_propMap.put<property::FontSize>(get(m_fontSize));
}

}

namespace
{

class ItalicElement : public PropertyContextBase
{
public:
  ItalicElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<bool> m_italic;
};

ItalicElement::ItalicElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
{
}

IWORKXMLContextPtr_t ItalicElement::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::number) == name)
    return makeContext<NumberElement<bool> >(getState(), m_italic);

  return IWORKXMLContextPtr_t();
}

void ItalicElement::endOfElement()
{
  if (m_italic)
    m_propMap.put<property::Italic>(get(m_italic));
}

}

namespace
{

class OutlineElement : public PropertyContextBase
{
public:
  OutlineElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<bool> m_outline;
};

OutlineElement::OutlineElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
{
}

IWORKXMLContextPtr_t OutlineElement::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::number) == name)
    return makeContext<NumberElement<bool> >(getState(), m_outline);

  return IWORKXMLContextPtr_t();
}

void OutlineElement::endOfElement()
{
  if (m_outline)
    m_propMap.put<property::Outline>(get(m_outline));
}

}

namespace
{

class StrikethruElement : public PropertyContextBase
{
public:
  StrikethruElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<bool> m_strikethru;
};

StrikethruElement::StrikethruElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
{
}

IWORKXMLContextPtr_t StrikethruElement::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::number) == name)
    return makeContext<NumberElement<bool> >(getState(), m_strikethru);

  return IWORKXMLContextPtr_t();
}

void StrikethruElement::endOfElement()
{
  if (m_strikethru)
    m_propMap.put<property::Strikethru>(get(m_strikethru));
}

}

namespace
{

class SuperscriptElement : public PropertyContextBase
{
public:
  SuperscriptElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<int> m_superscript;
};

SuperscriptElement::SuperscriptElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
{
}

IWORKXMLContextPtr_t SuperscriptElement::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::number) == name)
    return makeContext<NumberElement<int> >(getState(), m_superscript);

  return IWORKXMLContextPtr_t();
}

void SuperscriptElement::endOfElement()
{
  if (m_superscript)
  {
    IWORKBaseline prop;
    switch (get(m_superscript))
    {
    case 1 :
      prop = IWORK_BASELINE_SUPER;
      break;
    case 2 :
      prop = IWORK_BASELINE_SUB;
      break;
    default :
      ETONYEK_DEBUG_MSG(("unknown superscript %d\n", get(m_superscript)));
    }

    m_propMap.put<property::Baseline>(prop);
  }
}

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

class TabsElement : public PropertyContextBase
{
public:
  TabsElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  IWORKTabStops_t m_tabs;
  optional<double> m_current;
};

TabsElement::TabsElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
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

  if (!m_tabs.empty())
    m_propMap.put<property::Tabs>(m_tabs);
}

}

namespace
{

class UnderlineElement : public PropertyContextBase
{
public:
  UnderlineElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<bool> m_underline;
};

UnderlineElement::UnderlineElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
{
}

IWORKXMLContextPtr_t UnderlineElement::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::number) == name)
    return makeContext<NumberElement<bool> >(getState(), m_underline);

  return IWORKXMLContextPtr_t();
}

void UnderlineElement::endOfElement()
{
  if (m_underline)
    m_propMap.put<property::Underline>(get(m_underline));
}

}

namespace
{

class BaselineShiftElement : public PropertyContextBase
{
public:
  BaselineShiftElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<double> m_baselineShift;
};

BaselineShiftElement::BaselineShiftElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
{
}

IWORKXMLContextPtr_t BaselineShiftElement::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::number) == name)
    return makeContext<NumberElement<double> >(getState(), m_baselineShift);

  return IWORKXMLContextPtr_t();
}

void BaselineShiftElement::endOfElement()
{
  if (m_baselineShift)
    m_propMap.put<property::BaselineShift>(get(m_baselineShift));
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
  case IWORKToken::NS_URI_SF | IWORKToken::outline :
    return makeContext<OutlineElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::strikethru :
    return makeContext<StrikethruElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::superscript :
    return makeContext<SuperscriptElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::tabs :
    return makeContext<TabsElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::underline :
    return makeContext<UnderlineElement>(getState(), m_propMap);
  }

  return IWORKXMLContextPtr_t();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
