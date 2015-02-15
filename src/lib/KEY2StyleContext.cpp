/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEY2StyleContext.h"

#include <boost/lexical_cast.hpp>

#include "libetonyek_xml.h"
#include "IWORKToken.h"
#include "IWORKXMLContexts.h"
#include "IWORKXMLParserState.h"
#include "KEY2ParserUtils.h"
#include "KEY2Token.h"
#include "KEYCollector.h"
#include "KEYDefaults.h"
#include "KEYStyles.h"
#include "KEYTypes.h"

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

  void insert(const char *name, const any &value);

private:
  IWORKPropertyMap &m_propMap;
};

PropertyContextBase::PropertyContextBase(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : IWORKXMLElementContextBase(state)
  , m_propMap(propMap)
{
}

void PropertyContextBase::insert(const char *const name, const any &value)
{
  assert(name);
  assert(name[0]);

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
    return KEY2ParserUtils::bool_cast(value);
  }
};

template<>
struct NumberConverter<int>
{
  static int convert(const char *const value)
  {
    return KEY2ParserUtils::int_cast(value);
  }
};

template<>
struct NumberConverter<double>
{
  static double convert(const char *const value)
  {
    return KEY2ParserUtils::double_cast(value);
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
      ETONYEK_DEBUG_MSG(("unknown alignment %d\n", get(alignment)));
    }
  }

  insert("alignment", prop);
}

}

namespace
{

class FontColorContext : public PropertyContextBase
{
public:
  FontColorContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<IWORKColor> m_color;
};

FontColorContext::FontColorContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
{
}

IWORKXMLContextPtr_t FontColorContext::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::color) == name)
    return makeContext<IWORKColorContext>(getState(), m_color);

  return IWORKXMLContextPtr_t();
}

void FontColorContext::endOfElement()
{
  if (m_color)
    insert("fontColor", get(m_color));
}

}

namespace
{

class GeometryContext : public PropertyContextBase
{
public:
  GeometryContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  IWORKGeometryPtr_t m_geometry;
};

GeometryContext::GeometryContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
{
}

IWORKXMLContextPtr_t GeometryContext::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::geometry) == name)
    return makeContext<IWORKGeometryContext>(getState(), m_geometry);

  return IWORKXMLContextPtr_t();
}

void GeometryContext::endOfElement()
{
  insert("geometry", m_geometry);
}

}

namespace
{

class BoldContext : public PropertyContextBase
{
public:
  BoldContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<bool> m_bold;
};

BoldContext::BoldContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
{
}

IWORKXMLContextPtr_t BoldContext::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::number) == name)
    return makeContext<NumberContext<bool> >(getState(), m_bold);

  return IWORKXMLContextPtr_t();
}

void BoldContext::endOfElement()
{
  if (m_bold)
    insert("bold", get(m_bold));
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

class FontNameContext : public PropertyContextBase
{
public:
  FontNameContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  optional<string> m_fontName;
};

FontNameContext::FontNameContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : PropertyContextBase(state, propMap)
{
}

IWORKXMLContextPtr_t FontNameContext::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::string) == name)
    return makeContext<StringContext>(getState(), m_fontName);

  return IWORKXMLContextPtr_t();
}

void FontNameContext::endOfElement()
{
  if (m_fontName)
    insert("fontName", get(m_fontName));
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
    return makeContext<KEY2StyleContext>(getState(), name, true);
  case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle_ref :
  case IWORKToken::NS_URI_SF | IWORKToken::liststyle_ref :
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle_ref :
  case IWORKToken::NS_URI_SF | IWORKToken::vector_style_ref :
    return makeContext<KEY2StyleRefContext>(getState(), name, true, true);
  }

  return IWORKXMLContextPtr_t();
}

void StylePropertyContext::endOfElement()
{
}

}

namespace
{

class PropertyMapContext : public IWORKXMLElementContextBase
{
public:
  PropertyMapContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  IWORKPropertyMap &m_propMap;
};

PropertyMapContext::PropertyMapContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : IWORKXMLElementContextBase(state)
  , m_propMap(propMap)
{
}

IWORKXMLContextPtr_t PropertyMapContext::element(const int name)
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

  case KEY2Token::NS_URI_KEY | KEY2Token::headlineParagraphStyle :
    return makeContext<StylePropertyContext>(getState(), name, m_propMap);
  }

  return IWORKXMLContextPtr_t();
}

}

KEY2StyleContext::KEY2StyleContext(IWORKXMLParserState &state, const int id, const bool nested)
  : IWORKXMLElementContextBase(state)
  , m_id(id)
  , m_nested(nested)
  , m_props()
{
}

void KEY2StyleContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::ident :
    m_ident = value;
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::parent_ident :
    m_parentIdent = value;
    break;
  default :
    IWORKXMLElementContextBase::attribute(name, value);
    break;
  }
}

IWORKXMLContextPtr_t KEY2StyleContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::property_map :
    return makeContext<PropertyMapContext>(getState(), m_props);
  }

  return IWORKXMLContextPtr_t();
}

void KEY2StyleContext::endOfElement()
{
  switch (m_id)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::cell_style :
    getCollector()->collectCellStyle(getId(), m_props, m_ident, m_parentIdent, false, m_nested);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::characterstyle :
    getCollector()->collectCharacterStyle(getId(), m_props, m_ident, m_parentIdent, false, m_nested);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::connection_style :
    getCollector()->collectConnectionStyle(getId(), m_props, m_ident, m_parentIdent, false, m_nested);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::graphic_style :
    getCollector()->collectGraphicStyle(getId(), m_props, m_ident, m_parentIdent, false, m_nested);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle :
    getCollector()->collectLayoutStyle(getId(), m_props, m_ident, m_parentIdent, false, m_nested);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::liststyle :
    getCollector()->collectListStyle(getId(), m_props, m_ident, m_parentIdent, false, m_nested);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle :
    getCollector()->collectParagraphStyle(getId(), m_props, m_ident, m_parentIdent, false, m_nested);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::placeholder_style :
    getCollector()->collectPlaceholderStyle(getId(), m_props, m_ident, m_parentIdent, false, m_nested);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::slide_style :
    getCollector()->collectSlideStyle(getId(), m_props, m_ident, m_parentIdent, false, m_nested);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::tabular_style :
    getCollector()->collectTabularStyle(getId(), m_props, m_ident, m_parentIdent, false, m_nested);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::vector_style :
    getCollector()->collectVectorStyle(getId(), m_props, m_ident, m_parentIdent, false, m_nested);
    break;
  default :
    ETONYEK_DEBUG_MSG(("unhandled style %d\n", m_id));
    break;
  }
}

KEY2StyleRefContext::KEY2StyleRefContext(IWORKXMLParserState &state, const int id, const bool nested, const bool anonymous)
  : IWORKXMLEmptyContextBase(state)
  , m_id(id)
  , m_nested(nested)
  , m_anonymous(anonymous)
{
}

void KEY2StyleRefContext::endOfElement()
{
  const optional<string> dummyIdent;
  const optional<IWORKPropertyMap> dummyProps;

  // TODO: need to get the style
  switch (m_id)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::cell_style_ref :
    getCollector()->collectCellStyle(getRef(), dummyProps, dummyIdent, dummyIdent, true, m_anonymous);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::characterstyle_ref :
    getCollector()->collectCharacterStyle(getRef(), dummyProps, dummyIdent, dummyIdent, true, m_anonymous);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle_ref :
    getCollector()->collectLayoutStyle(getRef(), dummyProps, dummyIdent, dummyIdent, true, m_anonymous);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::liststyle_ref :
    getCollector()->collectListStyle(getRef(), dummyProps, dummyIdent, dummyIdent, true, m_anonymous);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle_ref :
    getCollector()->collectParagraphStyle(getRef(), dummyProps, dummyIdent, dummyIdent, true, m_anonymous);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::placeholder_style_ref :
    getCollector()->collectPlaceholderStyle(getRef(), dummyProps, dummyIdent, dummyIdent, true, m_anonymous);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::vector_style_ref :
    getCollector()->collectVectorStyle(getRef(), dummyProps, dummyIdent, dummyIdent, true, m_anonymous);
    break;
  default :
    break;
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
