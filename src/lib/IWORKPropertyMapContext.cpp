/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKPropertyMapContext.h"

#include <boost/lexical_cast.hpp>

#include "libetonyek_xml.h"
#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKStyleContext.h"
#include "IWORKStyles.h"
#include "IWORKToken.h"
#include "IWORKXMLContexts.h"
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

  void insert(const string &name, const any &value);

private:
  IWORKPropertyMap &m_propMap;
};

PropertyContextBase::PropertyContextBase(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : IWORKXMLElementContextBase(state)
  , m_propMap(propMap)
{
}

void PropertyContextBase::insert(const string &name, const any &value)
{
  assert(!name.empty());

  if (!value.empty())
    m_propMap.set(name, value);
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
class NumberContext : public IWORKXMLEmptyContextBase
{
public:
  NumberContext(IWORKXMLParserState &state, optional<T> &value);

private:
  virtual void attribute(int name, const char *value);

private:
  optional<T> &m_value;
};

template<typename T>
NumberContext<T>::NumberContext(IWORKXMLParserState &state, optional<T> &value)
  : IWORKXMLEmptyContextBase(state)
  , m_value(value)
{
}

template<typename T>
void NumberContext<T>::attribute(const int name, const char *const value)
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

template<class ContextT, class ValueT>
class DirectPropertyContextBase : public PropertyContextBase
{
public:
  DirectPropertyContextBase(IWORKXMLParserState &state, IWORKPropertyMap &propMap, int propId, const string &propName);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  const string m_propName;
  const int m_propId;
  ValueT m_value;
};

template<class ContextT, class ValueT>
DirectPropertyContextBase<ContextT, ValueT>::DirectPropertyContextBase(IWORKXMLParserState &state, IWORKPropertyMap &propMap, const int propId, const string &propName)
  : PropertyContextBase(state, propMap)
  , m_propName(propName)
  , m_propId(propId)
  , m_value()
{
}

template<class ContextT, class ValueT>
IWORKXMLContextPtr_t DirectPropertyContextBase<ContextT, ValueT>::element(const int name)
{
  if (m_propId == name)
    return makeContext<ContextT>(getState(), m_value);

  return IWORKXMLContextPtr_t();
}

template<class ContextT, class ValueT>
void DirectPropertyContextBase<ContextT, ValueT>::endOfElement()
{
  if (bool(m_value))
    insert(m_propName, m_value);
}

}

namespace
{

template<class ContextT, class ValueT>
class ValuePropertyContextBase : public PropertyContextBase
{
public:
  ValuePropertyContextBase(IWORKXMLParserState &state, IWORKPropertyMap &propMap, int propId, const string &propName);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  const string m_propName;
  const int m_propId;
  optional<ValueT> m_value;
};

template<class ContextT, class ValueT>
ValuePropertyContextBase<ContextT, ValueT>::ValuePropertyContextBase(IWORKXMLParserState &state, IWORKPropertyMap &propMap, const int propId, const string &propName)
  : PropertyContextBase(state, propMap)
  , m_propName(propName)
  , m_propId(propId)
  , m_value()
{
}

template<class ContextT, class ValueT>
IWORKXMLContextPtr_t ValuePropertyContextBase<ContextT, ValueT>::element(const int name)
{
  if (m_propId == name)
    return makeContext<ContextT>(getState(), m_value);

  return IWORKXMLContextPtr_t();
}

template<class ContextT, class ValueT>
void ValuePropertyContextBase<ContextT, ValueT>::endOfElement()
{
  if (bool(m_value))
    insert(m_propName, get(m_value));
}

}

namespace
{

class AlignmentContext : public PropertyContextBase
{
public:
  AlignmentContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<int> m_value;
};

AlignmentContext::AlignmentContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
{
}

IWORKXMLContextPtr_t AlignmentContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::number :
    return makeContext<NumberContext<int> >(getState(), m_value);
  }

  return IWORKXMLContextPtr_t();
}

void AlignmentContext::endOfElement()
{
  any prop;

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

  insert("alignment", prop);
}

}

namespace
{

class FontColorContext : public ValuePropertyContextBase<IWORKColorContext, IWORKColor>
{
public:
  FontColorContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap);
};

FontColorContext::FontColorContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : ValuePropertyContextBase(state, propMap, IWORKToken::NS_URI_SF | IWORKToken::color, "fontColor")
{
}

}

namespace
{

class GeometryContext : public DirectPropertyContextBase<IWORKGeometryContext, IWORKGeometryPtr_t>
{
public:
  GeometryContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap);
};

GeometryContext::GeometryContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : DirectPropertyContextBase(state, propMap, IWORKToken::NS_URI_SF | IWORKToken::geometry, "geometry")
{
}

}

namespace
{

class BoldContext : public ValuePropertyContextBase<NumberContext<bool>, bool>
{
public:
  BoldContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap);
};

BoldContext::BoldContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : ValuePropertyContextBase(state, propMap, IWORKToken::NS_URI_SF | IWORKToken::number, "bold")
{
}

}

namespace
{

class CapitalizationContext : public PropertyContextBase
{
public:
  CapitalizationContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<int> m_capitalization;
};

CapitalizationContext::CapitalizationContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
{
}

IWORKXMLContextPtr_t CapitalizationContext::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::number) == name)
    return makeContext<NumberContext<int> >(getState(), m_capitalization);

  return IWORKXMLContextPtr_t();
}

void CapitalizationContext::endOfElement()
{
  if (m_capitalization)
  {
    any prop;
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

    insert("capitalization", prop);
  }
}

}

namespace
{

class StringContext : public IWORKXMLEmptyContextBase
{
public:
  StringContext(IWORKXMLParserState &state, optional<string> &str);

private:
  virtual void attribute(int name, const char *value);

private:
  optional<string> &m_string;
};

StringContext::StringContext(IWORKXMLParserState &state, optional<string> &str)
  : IWORKXMLEmptyContextBase(state)
  , m_string(str)
{
}

void StringContext::attribute(const int name, const char *const value)
{
  if ((IWORKToken::NS_URI_SFA | IWORKToken::string) == name)
    m_string = value;
}

}

namespace
{

class FontNameContext : public ValuePropertyContextBase<StringContext, string>
{
public:
  FontNameContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap);
};

FontNameContext::FontNameContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : ValuePropertyContextBase(state, propMap, IWORKToken::NS_URI_SF | IWORKToken::string, "fontName")
{
}

}

namespace
{

class FontSizeContext : public PropertyContextBase
{
public:
  FontSizeContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<double> m_fontSize;
};

FontSizeContext::FontSizeContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
{
}

IWORKXMLContextPtr_t FontSizeContext::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::number) == name)
    return makeContext<NumberContext<double> >(getState(), m_fontSize);

  return IWORKXMLContextPtr_t();
}

void FontSizeContext::endOfElement()
{
  if (m_fontSize)
    insert("fontSize", get(m_fontSize));
}

}

namespace
{

class ItalicContext : public PropertyContextBase
{
public:
  ItalicContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<bool> m_italic;
};

ItalicContext::ItalicContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
{
}

IWORKXMLContextPtr_t ItalicContext::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::number) == name)
    return makeContext<NumberContext<bool> >(getState(), m_italic);

  return IWORKXMLContextPtr_t();
}

void ItalicContext::endOfElement()
{
  if (m_italic)
    insert("italic", get(m_italic));
}

}

namespace
{

class OutlineContext : public PropertyContextBase
{
public:
  OutlineContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<bool> m_outline;
};

OutlineContext::OutlineContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
{
}

IWORKXMLContextPtr_t OutlineContext::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::number) == name)
    return makeContext<NumberContext<bool> >(getState(), m_outline);

  return IWORKXMLContextPtr_t();
}

void OutlineContext::endOfElement()
{
  if (m_outline)
    insert("outline", get(m_outline));
}

}

namespace
{

class StrikethruContext : public PropertyContextBase
{
public:
  StrikethruContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<bool> m_strikethru;
};

StrikethruContext::StrikethruContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
{
}

IWORKXMLContextPtr_t StrikethruContext::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::number) == name)
    return makeContext<NumberContext<bool> >(getState(), m_strikethru);

  return IWORKXMLContextPtr_t();
}

void StrikethruContext::endOfElement()
{
  if (m_strikethru)
    insert("strikethru", get(m_strikethru));
}

}

namespace
{

class SuperscriptContext : public PropertyContextBase
{
public:
  SuperscriptContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<int> m_superscript;
};

SuperscriptContext::SuperscriptContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
{
}

IWORKXMLContextPtr_t SuperscriptContext::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::number) == name)
    return makeContext<NumberContext<int> >(getState(), m_superscript);

  return IWORKXMLContextPtr_t();
}

void SuperscriptContext::endOfElement()
{
  if (m_superscript)
  {
    any prop;
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

    if (!prop.empty())
      insert("superscript", prop);
  }
}

}

namespace
{

class TabstopContext : public IWORKXMLEmptyContextBase
{
public:
  TabstopContext(IWORKXMLParserState &state, optional<double> &pos);

private:
  virtual void attribute(int name, const char *value);

private:
  optional<double> &m_pos;
};

TabstopContext::TabstopContext(IWORKXMLParserState &state, optional<double> &pos)
  : IWORKXMLEmptyContextBase(state)
  , m_pos(pos)
{
}

void TabstopContext::attribute(const int name, const char *const value)
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

class TabsContext : public PropertyContextBase
{
public:
  TabsContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  IWORKTabStops_t m_tabs;
  optional<double> m_current;
};

TabsContext::TabsContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
{
}

IWORKXMLContextPtr_t TabsContext::element(const int name)
{
  if (m_current)
  {
    m_tabs.push_back(IWORKTabStop(get(m_current)));
    m_current.reset();
  }

  if ((IWORKToken::NS_URI_SF | IWORKToken::tabstop) == name)
    return makeContext<TabstopContext>(getState(), m_current);

  return IWORKXMLContextPtr_t();
}

void TabsContext::endOfElement()
{
  if (m_current)
    m_tabs.push_back(IWORKTabStop(get(m_current)));

  if (!m_tabs.empty())
    insert("tabs", m_tabs);
}

}

namespace
{

class UnderlineContext : public PropertyContextBase
{
public:
  UnderlineContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<bool> m_underline;
};

UnderlineContext::UnderlineContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
{
}

IWORKXMLContextPtr_t UnderlineContext::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::number) == name)
    return makeContext<NumberContext<bool> >(getState(), m_underline);

  return IWORKXMLContextPtr_t();
}

void UnderlineContext::endOfElement()
{
  if (m_underline)
    insert("underline", get(m_underline));
}

}

namespace
{

class BaselineShiftContext : public PropertyContextBase
{
public:
  BaselineShiftContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<double> m_baselineShift;
};

BaselineShiftContext::BaselineShiftContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
{
}

IWORKXMLContextPtr_t BaselineShiftContext::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::number) == name)
    return makeContext<NumberContext<double> >(getState(), m_baselineShift);

  return IWORKXMLContextPtr_t();
}

void BaselineShiftContext::endOfElement()
{
  if (m_baselineShift)
    insert("baselineShift", m_baselineShift);
}

}

namespace
{

class StylePropertyContext : public PropertyContextBase
{
public:
  StylePropertyContext(IWORKXMLParserState &state, int id, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  const int m_id;
};

StylePropertyContext::StylePropertyContext(IWORKXMLParserState &state, const int id, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
  , m_id(id)
{
}

IWORKXMLContextPtr_t StylePropertyContext::element(const int name)
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

void StylePropertyContext::endOfElement()
{
}

}

IWORKPropertyMapContext::IWORKPropertyMapContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : IWORKXMLElementContextBase(state)
  , m_propMap(propMap)
{
}

IWORKXMLContextPtr_t IWORKPropertyMapContext::element(const int name)
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
    return makeContext<StylePropertyContext>(getState(), name, m_propMap);

  case IWORKToken::NS_URI_SF | IWORKToken::alignment :
    return makeContext<AlignmentContext>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::baselineShift :
    return makeContext<BaselineShiftContext>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::bold :
    return makeContext<BoldContext>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::capitalization :
    return makeContext<CapitalizationContext>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::fontColor :
    return makeContext<FontColorContext>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::fontName :
    return makeContext<FontNameContext>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::fontSize :
    return makeContext<FontSizeContext>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<GeometryContext>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::italic :
    return makeContext<ItalicContext>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::outline :
    return makeContext<OutlineContext>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::strikethru :
    return makeContext<StrikethruContext>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::superscript :
    return makeContext<SuperscriptContext>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::tabs :
    return makeContext<TabsContext>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::underline :
    return makeContext<UnderlineContext>(getState(), m_propMap);
  }

  return IWORKXMLContextPtr_t();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
