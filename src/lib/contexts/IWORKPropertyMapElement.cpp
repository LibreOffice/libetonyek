/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKPropertyMapElement.h"

#include <memory>

#include "libetonyek_xml.h"
#include "IWORKCollector.h"
#include "IWORKColorElement.h"
#include "IWORKContainerContext.h"
#include "IWORKCoreImageFilterDescriptorElement.h"
#include "IWORKDictionary.h"
#include "IWORKDirectCollector.h"
#include "IWORKFilteredImageElement.h"
#include "IWORKGeometryElement.h"
#include "IWORKListstyleElement.h"
#include "IWORKNumericPropertyContext.h"
#include "IWORKProperties.h"
#include "IWORKPropertyContext.h"
#include "IWORKPropertyMap.h"
#include "IWORKPtrPropertyContext.h"
#include "IWORKRefContext.h"
#include "IWORKStringElement.h"
#include "IWORKStyleContext.h"
#include "IWORKStyleRefContext.h"
#include "IWORKTabsElement.h"
#include "IWORKToken.h"
#include "IWORKTokenizer.h"
#include "IWORKValueContext.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

using boost::none;
using boost::optional;

using std::deque;
using std::make_shared;
using std::string;

namespace
{

template<typename Property, int TokenId, int RefTokenId>
class StylePropertyContext : public IWORKPropertyContextBase
{
public:
  StylePropertyContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap, IWORKStyleMap_t &styleMap);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  IWORKStyleMap_t &m_styleMap;
  IWORKPropertyMap m_stylePropMap;
  optional<ID_t> m_ref;
};

template<typename Property, int TokenId, int RefTokenId>
StylePropertyContext<Property, TokenId, RefTokenId>::StylePropertyContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap, IWORKStyleMap_t &styleMap)
  : IWORKPropertyContextBase(state, propMap)
  , m_styleMap(styleMap)
  , m_stylePropMap()
  , m_ref()
{
}

template<typename Property, int TokenId, int RefTokenId>
IWORKXMLContextPtr_t StylePropertyContext<Property, TokenId, RefTokenId>::element(const int name)
{
  switch (name)
  {
  case TokenId :
    return makeContext<IWORKStyleContext>(getState(), m_stylePropMap, &m_styleMap, true);
  case RefTokenId :
    return makeContext<IWORKRefContext>(getState(), m_ref);
  }

  return IWORKXMLContextPtr_t();
}

template<typename Property, int TokenId, int RefTokenId>
void StylePropertyContext<Property, TokenId, RefTokenId>::endOfElement()
{
  if (m_ref)
  {
    auto it = m_styleMap.find(get(m_ref));
    if (m_styleMap.end() != it)
      m_propMap.put<Property>(it->second);
    else if (!get(m_ref).empty())
    {
      ETONYEK_DEBUG_MSG(("StylePropertyContext<...>::endOfElement: unknown style %s\n", get(m_ref).c_str()));
    }
  }
  else
  {
    m_propMap.put<Property>(make_shared<IWORKStyle>(m_stylePropMap, none, none));
  }
}

}

namespace
{

class TabsProperty : public IWORKPropertyContextBase
{
public:
  TabsProperty(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  IWORKTabStops_t m_tabs;
  optional<ID_t> m_ref;
};

TabsProperty::TabsProperty(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : IWORKPropertyContextBase(state, propMap)
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

class LinespacingElement : public IWORKXMLEmptyContextBase
{
public:
  LinespacingElement(IWORKXMLParserState &state, optional<IWORKLineSpacing> &value);

private:
  void attribute(int name, const char *value) override;
  void endOfElement() override;

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

class ElementElement : public IWORKXMLEmptyContextBase
{
public:
  ElementElement(IWORKXMLParserState &state, optional<double> &value);

private:
  void attribute(int name, const char *value) override;

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
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

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
  PatternElement(IWORKXMLParserState &state, optional<IWORKStrokeType> &type, deque<double> &value);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;

private:
  optional<IWORKStrokeType> &m_type;
  deque<double> &m_value;
};

PatternElement::PatternElement(IWORKXMLParserState &state, optional<IWORKStrokeType> &type, deque<double> &value)
  : IWORKXMLElementContextBase(state)
  , m_type(type)
  , m_value(value)
{
}

void PatternElement::attribute(const int name, const char *const value)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::type))
  {
    switch (getState().getTokenizer().getId(value))
    {
    case IWORKToken::empty :
      m_type = IWORK_STROKE_TYPE_NONE;
      break;
    case IWORKToken::solid :
      m_type = IWORK_STROKE_TYPE_SOLID;
      break;
    case IWORKToken::pattern :
      m_type = IWORK_STROKE_TYPE_DASHED;
      break;
    default :
      ETONYEK_DEBUG_MSG(("unknown pattern type %s\n", value));
      break;
    }
  }
  return IWORKXMLElementContextBase::attribute(name, value);
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
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  optional<IWORKStroke> &m_value;
  optional<IWORKStrokeType> m_type;
  optional<double> m_width;
  optional<IWORKColor> m_color;
  optional<IWORKLineCap> m_cap;
  optional<IWORKLineJoin> m_join;
  deque<double> m_pattern;
};

StrokeElement::StrokeElement(IWORKXMLParserState &state, optional<IWORKStroke> &value)
  : IWORKXMLElementContextBase(state)
  , m_value(value)
  , m_type()
  , m_width()
  , m_color()
  , m_cap()
  , m_join()
  , m_pattern()
{
}

void StrokeElement::attribute(const int name, const char *const value)
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
    }
    break;
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
    return makeContext<PatternElement>(getState(), m_type, m_pattern);
  }

  return IWORKXMLContextPtr_t();
}

void StrokeElement::endOfElement()
{
  if (m_width)
  {
    m_value = IWORKStroke();
    IWORKStroke &value = get(m_value);
    value.m_type = get_optional_value_or(m_type, IWORK_STROKE_TYPE_SOLID);
    value.m_width = get(m_width);
    if (m_color)
      value.m_color = get(m_color);
    value.m_pattern = m_pattern;
  }
}

}

namespace
{

class GradientStopElement : public IWORKXMLElementContextBase
{
public:
  GradientStopElement(IWORKXMLParserState &state, deque<IWORKGradientStop> &stops);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  deque<IWORKGradientStop> &m_stops;
  optional<IWORKColor> m_color;
  optional<double> m_fraction;
  optional<double> m_inflection;
};

GradientStopElement::GradientStopElement(IWORKXMLParserState &state, deque<IWORKGradientStop> &stops)
  : IWORKXMLElementContextBase(state)
  , m_stops(stops)
  , m_color()
  , m_fraction()
  , m_inflection()
{
}

void GradientStopElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::fraction :
    m_fraction = double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::inflection :
    m_inflection = double_cast(value);
    break;
  }
}

IWORKXMLContextPtr_t GradientStopElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::color :
    return makeContext<IWORKColorElement>(getState(), m_color);
  }

  return IWORKXMLContextPtr_t();
}

void GradientStopElement::endOfElement()
{
  if (m_fraction)
  {
    m_stops.push_back(IWORKGradientStop());
    m_stops.back().m_color = get_optional_value_or(m_color, IWORKColor());
    m_stops.back().m_fraction = get(m_fraction);
    m_stops.back().m_inflection = get_optional_value_or(m_inflection, 0.5);
  }
}

}

namespace
{

typedef IWORKContainerContext<IWORKGradientStop, GradientStopElement, IWORKDirectCollector, IWORKToken::NS_URI_SF | IWORKToken::gradient_stop> StopsElement;

}

namespace
{

class AngleGradientElement : public IWORKXMLElementContextBase
{
public:
  AngleGradientElement(IWORKXMLParserState &state, optional<IWORKGradient> &value);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  optional<IWORKGradient> &m_value;
  optional<IWORKGradientType> m_type;
  optional<double> m_opacity;
  optional<double> m_angle;
  deque<IWORKGradientStop> m_stops;
};

AngleGradientElement::AngleGradientElement(IWORKXMLParserState &state, optional<IWORKGradient> &value)
  : IWORKXMLElementContextBase(state)
  , m_value(value)
  , m_type()
  , m_opacity()
  , m_angle()
  , m_stops()
{
}

void AngleGradientElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::angle :
    m_angle = double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::opacity :
    m_opacity = double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::type :
    switch (getState().getTokenizer().getId(value))
    {
    case IWORKToken::linear :
      m_type = IWORK_GRADIENT_TYPE_LINEAR;
      break;
    case IWORKToken::radial :
      m_type = IWORK_GRADIENT_TYPE_RADIAL;
      break;
    }
    break;
  }
}

IWORKXMLContextPtr_t AngleGradientElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::stops :
    return makeContext<StopsElement>(getState(), m_stops);
  }

  return IWORKXMLContextPtr_t();
}

void AngleGradientElement::endOfElement()
{
  if (m_type && !m_stops.empty())
  {
    m_value = IWORKGradient();
    get(m_value).m_type = get(m_type);
    get(m_value).m_angle = get(m_angle);
    get(m_value).m_stops = m_stops;
  }
}

}

namespace
{

class TransformGradientElement : public IWORKXMLElementContextBase
{
public:
  TransformGradientElement(IWORKXMLParserState &state, optional<IWORKGradient> &value);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  optional<IWORKGradient> &m_value;
  optional<double> m_opacity;
  optional<IWORKGradientType> m_type;
  deque<IWORKGradientStop> m_stops;
};

TransformGradientElement::TransformGradientElement(IWORKXMLParserState &state, optional<IWORKGradient> &value)
  : IWORKXMLElementContextBase(state)
  , m_value(value)
  , m_opacity()
  , m_type()
  , m_stops()
{
}

void TransformGradientElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::opacity :
    m_opacity = double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::type :
    switch (getState().getTokenizer().getId(value))
    {
    case IWORKToken::linear :
      m_type = IWORK_GRADIENT_TYPE_LINEAR;
      break;
    case IWORKToken::radial :
      m_type = IWORK_GRADIENT_TYPE_RADIAL;
      break;
    }
    break;
  }
}

IWORKXMLContextPtr_t TransformGradientElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::stops :
    return makeContext<StopsElement>(getState(), m_stops);
  }

  return IWORKXMLContextPtr_t();
}

void TransformGradientElement::endOfElement()
{
  if (m_type && !m_stops.empty())
  {
    m_value = IWORKGradient();
    get(m_value).m_type = get(m_type);
    get(m_value).m_stops = m_stops;
  }
}

}

namespace
{

class TexturedFillElement : public IWORKXMLElementContextBase
{
public:
  TexturedFillElement(IWORKXMLParserState &state, optional<IWORKFillImage> &value);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  optional<IWORKFillImage> &m_value;
  optional<IWORKFillImageType> m_type;
  IWORKMediaContentPtr_t m_content;
};

TexturedFillElement::TexturedFillElement(IWORKXMLParserState &state, optional<IWORKFillImage> &value)
  : IWORKXMLElementContextBase(state)
  , m_value(value)
  , m_type()
  , m_content()
{
}

void TexturedFillElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::technique :
    switch (getState().getTokenizer().getId(value))
    {
    case IWORKToken::natural :
      m_type = IWORK_FILL_IMAGE_TYPE_ORIGINAL_SIZE;
      break;
    case IWORKToken::stretch :
      m_type = IWORK_FILL_IMAGE_TYPE_STRETCH;
      break;
    case IWORKToken::tile :
      m_type = IWORK_FILL_IMAGE_TYPE_TILE;
      break;
    case IWORKToken::fill :
      m_type = IWORK_FILL_IMAGE_TYPE_SCALE_TO_FILL;
      break;
    case IWORKToken::fit :
      m_type = IWORK_FILL_IMAGE_TYPE_SCALE_TO_FIT;
      break;
    }
    break;
  }
}

IWORKXMLContextPtr_t TexturedFillElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::filtered_image :
    return makeContext<IWORKFilteredImageElement>(getState(), m_content);
  }

  return IWORKXMLContextPtr_t();
}

void TexturedFillElement::endOfElement()
{
  if (m_type && bool(m_content) && bool(m_content->m_size) && bool(m_content->m_data) && bool(m_content->m_data->m_stream))
  {
    m_value = IWORKFillImage();
    get(m_value).m_type = get(m_type);
    get(m_value).m_size = get(m_content->m_size);
    get(m_value).m_stream = m_content->m_data->m_stream;
    get(m_value).m_mimeType = m_content->m_data->m_mimeType;
  }
}

}

namespace
{

class FillElement : public IWORKXMLElementContextBase
{
public:
  FillElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  IWORKPropertyMap &m_propMap;
  optional<IWORKColor> m_color;
  optional<IWORKGradient> m_gradient;
  optional<IWORKFillImage> m_bitmap;
};

FillElement::FillElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : IWORKXMLElementContextBase(state)
  , m_propMap(propMap)
  , m_color()
  , m_gradient()
  , m_bitmap()
{
}

IWORKXMLContextPtr_t FillElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::angle_gradient :
    return makeContext<AngleGradientElement>(getState(), m_gradient);
  case IWORKToken::NS_URI_SF | IWORKToken::color :
    return makeContext<IWORKColorElement>(getState(), m_color);
  case IWORKToken::NS_URI_SF | IWORKToken::textured_fill :
    return makeContext<TexturedFillElement>(getState(), m_bitmap);
  case IWORKToken::NS_URI_SF | IWORKToken::transform_gradient :
    return makeContext<TransformGradientElement>(getState(), m_gradient);
  }

  return IWORKXMLContextPtr_t();
}

void FillElement::endOfElement()
{
  if (m_color)
    m_propMap.put<property::Fill>(get(m_color));
  else if (m_gradient)
    m_propMap.put<property::Fill>(get(m_gradient));
  else if (m_bitmap)
    m_propMap.put<property::Fill>(get(m_bitmap));
}

}

namespace
{

class ColumnElement : public IWORKXMLEmptyContextBase
{
public:
  ColumnElement(IWORKXMLParserState &state, IWORKColumns::Column &value);

private:
  void attribute(int name, const char *value) override;
  void endOfElement() override;

private:
  IWORKColumns::Column &m_value;
  IWORKColumns::Column m_builder;
};

ColumnElement::ColumnElement(IWORKXMLParserState &state, IWORKColumns::Column &value)
  : IWORKXMLEmptyContextBase(state)
  , m_value(value)
  , m_builder()
{
}

void ColumnElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::spacing :
    m_builder.m_spacing = get_optional_value_or(try_double_cast(value), 0);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::width :
    m_builder.m_width = get_optional_value_or(try_double_cast(value), 0);
    break;
  }
}

void ColumnElement::endOfElement()
{
  m_value = m_builder;
}

}

namespace
{

class ColumnsElement : public IWORKXMLElementContextBase
{
public:
  ColumnsElement(IWORKXMLParserState &state, optional<IWORKColumns> &value);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  optional<IWORKColumns> &m_value;
  IWORKColumns m_builder;
};

ColumnsElement::ColumnsElement(IWORKXMLParserState &state, optional<IWORKColumns> &value)
  : IWORKXMLElementContextBase(state)
  , m_value(value)
  , m_builder()
{
}

void ColumnsElement::attribute(const int name, const char *const value)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::equal_columns))
    m_builder.m_equal = get_optional_value_or(try_bool_cast(value), false);
}

IWORKXMLContextPtr_t ColumnsElement::element(const int name)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::column))
  {
    m_builder.m_columns.push_back(IWORKColumns::Column());
    return makeContext<ColumnElement>(getState(), m_builder.m_columns.back());
  }
  return IWORKXMLContextPtr_t();
}

void ColumnsElement::endOfElement()
{
  m_value = m_builder;
}

}

namespace
{

class PaddingElement : public IWORKXMLEmptyContextBase
{
public:
  PaddingElement(IWORKXMLParserState &state, optional<IWORKPadding> &value);

private:
  void attribute(int name, const char *value) override;
  void endOfElement() override;

private:
  optional<IWORKPadding> &m_value;
  IWORKPadding m_builder;
};

PaddingElement::PaddingElement(IWORKXMLParserState &state, optional<IWORKPadding> &value)
  : IWORKXMLEmptyContextBase(state)
  , m_value(value)
  , m_builder()
{
}

void PaddingElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::bottom :
    m_builder.m_bottom = try_double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::left :
    m_builder.m_left = try_double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::right :
    m_builder.m_right = try_double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::top :
    m_builder.m_top = try_double_cast(value);
    break;
  }
}

void PaddingElement::endOfElement()
{
  m_value = m_builder;
}

}

namespace
{

class LanguageElement : public IWORKPropertyContextBase
{
public:
  LanguageElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  optional<string> m_lang;
};

LanguageElement::LanguageElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : IWORKPropertyContextBase(state, propMap)
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
    {
      const string &tag = getState().m_langManager.addLocale(get(m_lang));
      if (tag.empty())
        m_propMap.clear<property::Language>();
      else
        m_propMap.put<property::Language>(tag);
    }
  }
  else if (m_default)
  {
    m_propMap.clear<property::Language>();
  }
}

}

namespace
{

class NumberFormatElement : public IWORKXMLEmptyContextBase
{
public:
  NumberFormatElement(IWORKXMLParserState &state, optional<IWORKNumberFormat> &value);

private:
  void attribute(int name, const char *value) override;
  void endOfElement() override;

private:
  optional<IWORKNumberFormat> &m_value;
  IWORKNumberFormat m_builder;
};

NumberFormatElement::NumberFormatElement(IWORKXMLParserState &state, optional<IWORKNumberFormat> &value)
  : IWORKXMLEmptyContextBase(state)
  , m_value(value)
  , m_builder()
{
}

void NumberFormatElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::format_type :
    m_builder.m_type = get(IWORKNumberConverter<IWORKCellNumberType>::convert(value));
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::format_string :
    m_builder.m_string = value;
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::format_decimal_places :
    m_builder.m_decimalPlaces = int_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::format_currency_code :
    m_builder.m_currencyCode = value;
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::format_negative_style :
    m_builder.m_negativeStyle = int_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::format_show_thousands_separator :
    m_builder.m_thousandsSeperator = bool_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::format_fraction_accuracy :
    m_builder.m_fractionAccuracy = int_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::format_use_accounting_style :
    m_builder.m_accountingStyle = bool_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::format_base :
    m_builder.m_base = int_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::format_base_places :
    m_builder.m_basePlaces = int_cast(value);
    break;
  }
}

void NumberFormatElement::endOfElement()
{
  m_value = m_builder;
}

}

namespace
{

class DateTimeFormatElement : public IWORKXMLEmptyContextBase
{
public:
  DateTimeFormatElement(IWORKXMLParserState &state, optional<IWORKDateTimeFormat> &value);

private:
  void attribute(int name, const char *value) override;
  void endOfElement() override;

private:
  optional<IWORKDateTimeFormat> &m_value;
  IWORKDateTimeFormat m_builder;
};

DateTimeFormatElement::DateTimeFormatElement(IWORKXMLParserState &state, optional<IWORKDateTimeFormat> &value)
  : IWORKXMLEmptyContextBase(state)
  , m_value(value)
  , m_builder()
{
}

void DateTimeFormatElement::attribute(const int name, const char *const value)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::fmt) == name)
    m_builder.m_format = value;
}

void DateTimeFormatElement::endOfElement()
{
  m_value = m_builder;
}

}

namespace
{

class DurationFormatElement : public IWORKXMLEmptyContextBase
{
public:
  DurationFormatElement(IWORKXMLParserState &state, optional<IWORKDurationFormat> &value);

private:
  void attribute(int name, const char *value) override;
  void endOfElement() override;

private:
  optional<IWORKDurationFormat> &m_value;
  IWORKDurationFormat m_builder;
};

DurationFormatElement::DurationFormatElement(IWORKXMLParserState &state, optional<IWORKDurationFormat> &value)
  : IWORKXMLEmptyContextBase(state)
  , m_value(value)
  , m_builder()
{
}

void DurationFormatElement::attribute(const int name, const char *const value)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::fmt) == name)
    m_builder.m_format = value;
}

void DurationFormatElement::endOfElement()
{
  m_value = m_builder;
}

}

namespace
{

class OverridesElement : public IWORKXMLElementContextBase
{
public:
  OverridesElement(IWORKXMLParserState &state, IWORKShadow &value);

private:
  typedef IWORKValueContext<double, IWORKNumberElement<double>, IWORKToken::NS_URI_SF | IWORKToken::number> NumberProperty;
  typedef IWORKValueContext<IWORKColor, IWORKColorElement, IWORKToken::NS_URI_SF | IWORKToken::color> ColorProperty;

private:
  IWORKXMLContextPtr_t element(int name) override;

private:
  IWORKShadow &m_value;
  IWORKShadow m_builder;
};

OverridesElement::OverridesElement(IWORKXMLParserState &state, IWORKShadow &value)
  : IWORKXMLElementContextBase(state)
  , m_value(value)
{
}

IWORKXMLContextPtr_t OverridesElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::inputAngle :
    return makeContext<NumberProperty>(getState(), m_value.m_angle);
  case IWORKToken::NS_URI_SF | IWORKToken::inputColor :
    return makeContext<ColorProperty>(getState(), m_value.m_color);
  case IWORKToken::NS_URI_SF | IWORKToken::inputOpacity :
    return makeContext<NumberProperty>(getState(), m_value.m_opacity);
  case IWORKToken::NS_URI_SF | IWORKToken::inputDistance :
    return makeContext<NumberProperty>(getState(), m_value.m_offset);
  }
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class CoreImageFilterInfoElement : public IWORKXMLElementContextBase
{
public:
  CoreImageFilterInfoElement(IWORKXMLParserState &state, deque<IWORKShadow> &elements);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  deque<IWORKShadow> &m_elements;
  bool m_isShadow;
  optional<ID_t> m_descriptorRef;
  IWORKShadow m_value;
};

CoreImageFilterInfoElement::CoreImageFilterInfoElement(IWORKXMLParserState &state, deque<IWORKShadow> &elements)
  : IWORKXMLElementContextBase(state)
  , m_elements(elements)
  , m_isShadow(false)
  , m_descriptorRef()
  , m_value()
{
}

IWORKXMLContextPtr_t CoreImageFilterInfoElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::core_image_filter_descriptor :
    return makeContext<IWORKCoreImageFilterDescriptorElement>(getState(), m_isShadow);
  case IWORKToken::NS_URI_SF | IWORKToken::core_image_filter_descriptor_ref :
    return makeContext<IWORKRefContext>(getState(), m_descriptorRef);
  case IWORKToken::NS_URI_SF | IWORKToken::overrides :
    return makeContext<OverridesElement>(getState(), m_value);
  }
  return IWORKXMLContextPtr_t();
}

void CoreImageFilterInfoElement::endOfElement()
{
  if (m_descriptorRef)
  {
    const IWORKFilterDescriptorMap_t::const_iterator it = getState().getDictionary().m_filterDescriptors.find(get(m_descriptorRef));
    if (it != getState().getDictionary().m_filterDescriptors.end())
      m_isShadow = it->second.m_isShadow;
  }
  if (m_isShadow)
    m_elements.push_back(m_value);
}

}

namespace
{

class FiltersElement : public IWORKXMLElementContextBase
{
  typedef IWORKContainerContext<IWORKShadow, CoreImageFilterInfoElement, IWORKDirectCollector, IWORKToken::NS_URI_SF | IWORKToken::core_image_filter_info> MutableArrayElement;

public:
  FiltersElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  IWORKPropertyMap &m_propMap;
  deque<IWORKShadow> m_elements;
};

FiltersElement::FiltersElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : IWORKXMLElementContextBase(state)
  , m_propMap(propMap)
  , m_elements()
{
}

IWORKXMLContextPtr_t FiltersElement::element(const int name)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::mutable_array))
    return makeContext<MutableArrayElement>(getState(), m_elements);
  return IWORKXMLContextPtr_t();
}

void FiltersElement::endOfElement()
{
  if (m_elements.empty())
    m_propMap.clear<property::Shadow>();
  else
    m_propMap.put<property::Shadow>(m_elements.back());
}

}

namespace
{

typedef IWORKPropertyContext<property::Columns, ColumnsElement, IWORKToken::NS_URI_SF | IWORKToken::columns> ColumnsProperty;
typedef IWORKPropertyContext<property::FontColor, IWORKColorElement, IWORKToken::NS_URI_SF | IWORKToken::color> FontColorElement;
typedef IWORKPropertyContext<property::FontName, IWORKStringElement, IWORKToken::NS_URI_SF | IWORKToken::string> FontNameElement;
typedef IWORKPropertyContext<property::LayoutMargins, PaddingElement, IWORKToken::NS_URI_SF | IWORKToken::padding> LayoutMarginsElement;
typedef IWORKPropertyContext<property::LineSpacing, LinespacingElement, IWORKToken::NS_URI_SF | IWORKToken::linespacing> LineSpacingElement;
typedef IWORKPropertyContext<property::ParagraphFill, IWORKColorElement, IWORKToken::NS_URI_SF | IWORKToken::color> ParagraphFillElement;
typedef IWORKPropertyContext<property::ParagraphStroke, StrokeElement, IWORKToken::NS_URI_SF | IWORKToken::stroke> ParagraphStrokeElement;
typedef IWORKPropertyContext<property::SFTCellStylePropertyDateTimeFormat, DateTimeFormatElement, IWORKToken::NS_URI_SF | IWORKToken::date_format> SFTCellStylePropertyDateTimeFormatElement;
typedef IWORKPropertyContext<property::SFTCellStylePropertyDurationFormat, DurationFormatElement, IWORKToken::NS_URI_SF | IWORKToken::duration_format> SFTCellStylePropertyDurationFormatElement;
typedef IWORKPropertyContext<property::SFTCellStylePropertyNumberFormat, NumberFormatElement, IWORKToken::NS_URI_SF | IWORKToken::number_format> SFTCellStylePropertyNumberFormatElement;
typedef IWORKPropertyContext<property::SFTStrokeProperty, StrokeElement, IWORKToken::NS_URI_SF | IWORKToken::stroke> SFTStrokePropertyElement;
typedef IWORKPropertyContext<property::Stroke, StrokeElement, IWORKToken::NS_URI_SF | IWORKToken::stroke> StrokeProperty;
typedef IWORKPropertyContext<property::TextBackground, IWORKColorElement, IWORKToken::NS_URI_SF | IWORKToken::color> TextBackgroundElement;

typedef IWORKPtrPropertyContext<property::Geometry, IWORKGeometryElement, IWORKToken::NS_URI_SF | IWORKToken::geometry> GeometryElement;

typedef StylePropertyContext<property::FollowingLayoutStyle, IWORKToken::NS_URI_SF | IWORKToken::layoutstyle, IWORKToken::NS_URI_SF | IWORKToken::layoutstyle_ref> FollowingLayoutStyleElement;
typedef StylePropertyContext<property::FollowingParagraphStyle, IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle, IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle_ref> FollowingParagraphStyleElement;
typedef StylePropertyContext<property::LayoutParagraphStyle, IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle, IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle_ref> LayoutParagraphStyleElement;
typedef StylePropertyContext<property::SFTDefaultBodyCellStyleProperty, IWORKToken::NS_URI_SF | IWORKToken::cell_style, IWORKToken::NS_URI_SF | IWORKToken::cell_style_ref> SFTDefaultBodyCellStylePropertyElement;
typedef StylePropertyContext<property::SFTDefaultFooterRowCellStyleProperty, IWORKToken::NS_URI_SF | IWORKToken::cell_style, IWORKToken::NS_URI_SF | IWORKToken::cell_style_ref> SFTDefaultFooterRowCellStylePropertyElement;
typedef StylePropertyContext<property::SFTDefaultHeaderColumnCellStyleProperty, IWORKToken::NS_URI_SF | IWORKToken::cell_style, IWORKToken::NS_URI_SF | IWORKToken::cell_style_ref> SFTDefaultHeaderColumnCellStylePropertyElement;
typedef StylePropertyContext<property::SFTDefaultHeaderRowCellStyleProperty, IWORKToken::NS_URI_SF | IWORKToken::cell_style, IWORKToken::NS_URI_SF | IWORKToken::cell_style_ref> SFTDefaultHeaderRowCellStylePropertyElement;
typedef StylePropertyContext<property::SFTCellStylePropertyParagraphStyle, IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle, IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle_ref> SFTCellStylePropertyParagraphStylePropertyElement;
typedef StylePropertyContext<property::SFTCellStylePropertyLayoutStyle, IWORKToken::NS_URI_SF | IWORKToken::layoutstyle, IWORKToken::NS_URI_SF | IWORKToken::layoutstyle_ref> SFTCellStylePropertyLayoutStylePropertyElement;

typedef IWORKNumericPropertyContext<property::Alignment> AlignmentElement;
typedef IWORKNumericPropertyContext<property::Baseline> SuperscriptElement;
typedef IWORKNumericPropertyContext<property::BaselineShift> BaselineShiftElement;
typedef IWORKNumericPropertyContext<property::Bold> BoldElement;
typedef IWORKNumericPropertyContext<property::Capitalization> CapitalizationElement;
typedef IWORKNumericPropertyContext<property::FirstLineIndent> FirstLineIndentElement;
typedef IWORKNumericPropertyContext<property::FontSize> FontSizeElement;
typedef IWORKNumericPropertyContext<property::Italic> ItalicElement;
typedef IWORKNumericPropertyContext<property::KeepLinesTogether> KeepLinesTogetherElement;
typedef IWORKNumericPropertyContext<property::KeepWithNext> KeepWithNextElement;
typedef IWORKNumericPropertyContext<property::LeftIndent> LeftIndentElement;
typedef IWORKNumericPropertyContext<property::Opacity> OpacityElement;
typedef IWORKNumericPropertyContext<property::Outline> OutlineElement;
typedef IWORKNumericPropertyContext<property::PageBreakBefore> PageBreakBeforeElement;
typedef IWORKNumericPropertyContext<property::ParagraphBorderType> ParagraphBorderTypeElement;
typedef IWORKNumericPropertyContext<property::RightIndent> RightIndentElement;
typedef IWORKNumericPropertyContext<property::SFTHeaderColumnRepeatsProperty> SFTHeaderColumnRepeatsPropertyElement;
typedef IWORKNumericPropertyContext<property::SFTHeaderRowRepeatsProperty> SFTHeaderRowRepeatsPropertyElement;
typedef IWORKNumericPropertyContext<property::SFTTableBandedRowsProperty> SFTTableBandedRowsPropertyElement;
typedef IWORKNumericPropertyContext<property::SpaceAfter> SpaceAfterElement;
typedef IWORKNumericPropertyContext<property::SpaceBefore> SpaceBeforeElement;
typedef IWORKNumericPropertyContext<property::Strikethru> StrikethruElement;
typedef IWORKNumericPropertyContext<property::Tracking> TrackingElement;
typedef IWORKNumericPropertyContext<property::Underline> UnderlineElement;
typedef IWORKNumericPropertyContext<property::WidowControl> WidowControlElement;

}

namespace
{

class ListStyleProperty : public IWORKPropertyContextBase
{
public:
  ListStyleProperty(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  IWORKListStyle_t m_style;
  boost::optional<ID_t> m_ref;
};

ListStyleProperty::ListStyleProperty(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : IWORKPropertyContextBase(state, propMap)
  , m_style()
  , m_ref()
{
}

IWORKXMLContextPtr_t ListStyleProperty::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::liststyle :
    m_default = false;
    return makeContext<IWORKListstyleElement>(getState(), m_style);
  case IWORKToken::NS_URI_SF | IWORKToken::liststyle_ref :
    return makeContext<IWORKRefContext>(getState(), m_ref);
  }
  return IWORKXMLContextPtr_t();
}

void ListStyleProperty::endOfElement()
{
  if (m_ref)
  {
    const IWORKListStyleMap_t::const_iterator it = getState().getDictionary().m_listStyles.find(get(m_ref));
    if (it != getState().getDictionary().m_listStyles.end())
      m_style = it->second;
    // argh, no sure how to retrieve the list styles here...
    else
    {
      ETONYEK_DEBUG_MSG(("ListStyleProperty::attribute: unknown style %s\n", get(m_ref).c_str()));
    }
  }

  m_propMap.put<property::ListLevelStyles>(m_style);
}

}

IWORKPropertyMapElement::IWORKPropertyMapElement(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : IWORKXMLElementContextBase(state)
  , m_propMap(&propMap)
  , m_propHandler(nullptr)
{
}

IWORKPropertyMapElement::IWORKPropertyMapElement(IWORKXMLParserState &state, IWORKPropertyHandler &propHandler)
  : IWORKXMLElementContextBase(state)
  , m_propMap(nullptr)
  , m_propHandler(&propHandler)
{
}

IWORKXMLContextPtr_t IWORKPropertyMapElement::element(const int name)
{
  if (m_propHandler)
    return m_propHandler->handle(name);

  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::alignment :
    return makeContext<AlignmentElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::baselineShift :
    return makeContext<BaselineShiftElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::bold :
    return makeContext<BoldElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::capitalization :
    return makeContext<CapitalizationElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::columns :
    return makeContext<ColumnsProperty>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::fill :
    return makeContext<FillElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::filters :
    return makeContext<FiltersElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::firstLineIndent :
    return makeContext<FirstLineIndentElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::followingLayoutStyle :
    return makeContext<FollowingLayoutStyleElement>(getState(), *m_propMap, getState().getDictionary().m_layoutStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::followingParagraphStyle :
    return makeContext<FollowingParagraphStyleElement>(getState(), *m_propMap, getState().getDictionary().m_paragraphStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::fontColor :
    return makeContext<FontColorElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::fontName :
    return makeContext<FontNameElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::fontSize :
    return makeContext<FontSizeElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<GeometryElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::italic :
    return makeContext<ItalicElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::keepLinesTogether :
    return makeContext<KeepLinesTogetherElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::keepWithNext :
    return makeContext<KeepWithNextElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::language :
    return makeContext<LanguageElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::layoutMargins :
    return makeContext<LayoutMarginsElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::layoutParagraphStyle :
    return makeContext<LayoutParagraphStyleElement>(getState(), *m_propMap, getState().getDictionary().m_paragraphStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::leftIndent :
    return makeContext<LeftIndentElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::lineSpacing :
    return makeContext<LineSpacingElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::listStyle :
    return makeContext<ListStyleProperty>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::opacity :
    return makeContext<OpacityElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::outline :
    return makeContext<OutlineElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::pageBreakBefore :
    return makeContext<PageBreakBeforeElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphBorderType :
    return makeContext<ParagraphBorderTypeElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphFill :
    return makeContext<ParagraphFillElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphStroke :
    return makeContext<ParagraphStrokeElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::rightIndent :
    return makeContext<RightIndentElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTCellStylePropertyNumberFormat :
    return makeContext<SFTCellStylePropertyNumberFormatElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTCellStylePropertyDateTimeFormat :
    return makeContext<SFTCellStylePropertyDateTimeFormatElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTCellStylePropertyDurationFormat :
    return makeContext<SFTCellStylePropertyDurationFormatElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTCellStylePropertyLayoutStyle :
    return makeContext<SFTCellStylePropertyLayoutStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_layoutStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTCellStylePropertyParagraphStyle :
    return makeContext<SFTCellStylePropertyParagraphStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_paragraphStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultBodyCellStyleProperty :
    return makeContext<SFTDefaultBodyCellStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_cellStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultFooterRowCellStyleProperty :
    return makeContext<SFTDefaultFooterRowCellStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_cellStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultHeaderColumnCellStyleProperty :
    return makeContext<SFTDefaultHeaderColumnCellStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_cellStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTDefaultHeaderRowCellStyleProperty :
    return makeContext<SFTDefaultHeaderRowCellStylePropertyElement>(getState(), *m_propMap, getState().getDictionary().m_cellStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTHeaderColumnRepeatsProperty :
    return makeContext<SFTHeaderColumnRepeatsPropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTHeaderRowRepeatsProperty :
    return makeContext<SFTHeaderRowRepeatsPropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTStrokeProperty :
    return makeContext<SFTStrokePropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::SFTTableBandedRowsProperty :
    return makeContext<SFTTableBandedRowsPropertyElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::spaceAfter :
    return makeContext<SpaceAfterElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::spaceBefore :
    return makeContext<SpaceBeforeElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::strikethru :
    return makeContext<StrikethruElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::stroke :
    return makeContext<StrokeProperty>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::superscript :
    return makeContext<SuperscriptElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::tabs :
    return makeContext<TabsProperty>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::textBackground :
    return makeContext<TextBackgroundElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::tracking :
    return makeContext<TrackingElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::underline :
    return makeContext<UnderlineElement>(getState(), *m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::widowControl :
    return makeContext<WidowControlElement>(getState(), *m_propMap);
  }

  return IWORKXMLContextPtr_t();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
