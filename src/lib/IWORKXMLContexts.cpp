/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKXMLContexts.h"

#include <string>

#include <boost/lexical_cast.hpp>

#include "libetonyek_utils.h"
#include "libetonyek_xml.h"
#include "IWORKDictionary.h"
#include "IWORKPropertyMap.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"
#include "KEYCollector.h"

namespace libetonyek
{

using boost::lexical_cast;
using boost::optional;

using std::string;

namespace
{

class BrContext : public IWORKXMLEmptyContextBase
{
public:
  explicit BrContext(IWORKXMLParserState &state);

private:
  virtual void endOfElement();
};

BrContext::BrContext(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
{
}

void BrContext::endOfElement()
{
  getCollector()->collectLineBreak();
}

}

namespace
{

class TabContext : public IWORKXMLEmptyContextBase
{
public:
  explicit TabContext(IWORKXMLParserState &state);

private:
  virtual void endOfElement();
};

TabContext::TabContext(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
{
}

void TabContext::endOfElement()
{
  getCollector()->collectTab();
}

}

namespace
{

class SpanContext : public IWORKXMLElementContextBase
{
public:
  explicit SpanContext(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void text(const char *value);

private:
  optional<ID_t> m_style;
};

SpanContext::SpanContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

void SpanContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::style :
    m_style = value;
    break;
  }
}

IWORKXMLContextPtr_t SpanContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::br :
  case IWORKToken::NS_URI_SF | IWORKToken::crbr :
  case IWORKToken::NS_URI_SF | IWORKToken::intratopicbr :
  case IWORKToken::NS_URI_SF | IWORKToken::lnbr :
    return makeContext<BrContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::tab :
    return makeContext<TabContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void SpanContext::text(const char *const value)
{
  IWORKStylePtr_t style;
  if (m_style)
  {
    const IWORKStyleMap_t::const_iterator it = getState().getDictionary().m_characterStyles.find(get(m_style));
    if (getState().getDictionary().m_characterStyles.end() != it)
      style = it->second;
  }
  getCollector()->collectText(style, value);
}

}

namespace
{

class LinkContext : public IWORKXMLMixedContextBase
{
public:
  explicit LinkContext(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void text(const char *value);
};

LinkContext::LinkContext(IWORKXMLParserState &state)
  : IWORKXMLMixedContextBase(state)
{
}

void LinkContext::attribute(int, const char *)
{
}

IWORKXMLContextPtr_t LinkContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::br :
    return makeContext<BrContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::span :
    return makeContext<SpanContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void LinkContext::text(const char *const value)
{
  getCollector()->collectText(IWORKStylePtr_t(), value);
}

}

namespace
{

class PContext : public IWORKXMLMixedContextBase
{
public:
  explicit PContext(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual void endOfAttributes();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
  virtual void text(const char *value);

private:
  IWORKStylePtr_t m_style;
};

PContext::PContext(IWORKXMLParserState &state)
  : IWORKXMLMixedContextBase(state)
{
}

void PContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::style :
  {
    const IWORKStyleMap_t::const_iterator it = getState().getDictionary().m_paragraphStyles.find(value);
    if (getState().getDictionary().m_paragraphStyles.end() != it)
      m_style = it->second;
    break;
  }
  }
}

void PContext::endOfAttributes()
{
  getCollector()->startParagraph(m_style);
}

IWORKXMLContextPtr_t PContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::br :
  case IWORKToken::NS_URI_SF | IWORKToken::crbr :
  case IWORKToken::NS_URI_SF | IWORKToken::intratopicbr :
  case IWORKToken::NS_URI_SF | IWORKToken::lnbr :
    return makeContext<BrContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::span :
    return makeContext<SpanContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::tab :
    return makeContext<TabContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::link :
    return makeContext<LinkContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void PContext::text(const char *const value)
{
  getCollector()->collectText(m_style, value);
}

void PContext::endOfElement()
{
  getCollector()->endParagraph();
}

}

namespace
{

class LayoutContext : public IWORKXMLElementContextBase
{
public:
  explicit LayoutContext(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
};

LayoutContext::LayoutContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

void LayoutContext::attribute(const int name, const char *)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::style) == name)
    getCollector()->collectLayoutStyle(IWORKStylePtr_t(), false);
}

IWORKXMLContextPtr_t LayoutContext::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::p) == name)
    return makeContext<PContext>(getState());

  return IWORKXMLContextPtr_t();
}

}

IWORKColorContext::IWORKColorContext(IWORKXMLParserState &state, boost::optional<IWORKColor> &color)
  : IWORKXMLEmptyContextBase(state)
  , m_color(color)
  , m_r(0)
  , m_g(0)
  , m_b(0)
  , m_a(0)
{
}

void IWORKColorContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  // TODO: check xsi:type too
  case IWORKToken::NS_URI_SFA | IWORKToken::a :
    m_a = lexical_cast<double>(value);
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::b :
    m_b = lexical_cast<double>(value);
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::g :
    m_g = lexical_cast<double>(value);
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::r :
    m_r = lexical_cast<double>(value);
    break;
  }
}

void IWORKColorContext::endOfElement()
{
  m_color = IWORKColor(m_r, m_g, m_b, m_a);
}

IWORKGeometryContext::IWORKGeometryContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_geometry(0)
{
}

IWORKGeometryContext::IWORKGeometryContext(IWORKXMLParserState &state, IWORKGeometryPtr_t &geometry)
  : IWORKXMLElementContextBase(state)
  , m_geometry(&geometry)
{
}

void IWORKGeometryContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::angle :
    m_angle = deg2rad(lexical_cast<double>(value));
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::aspectRatioLocked :
    m_aspectRatioLocked = bool_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::horizontalFlip :
    m_horizontalFlip = bool_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::shearXAngle :
    m_shearXAngle = deg2rad(lexical_cast<double>(value));
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::shearYAngle :
    m_shearYAngle = deg2rad(lexical_cast<double>(value));
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::sizesLocked :
    m_sizesLocked = bool_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::verticalFlip :
    m_verticalFlip = bool_cast(value);
    break;
  default :
    IWORKXMLElementContextBase::attribute(name, value);
    break;
  }
}

IWORKXMLContextPtr_t IWORKGeometryContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::naturalSize :
    return makeContext<IWORKSizeContext>(getState(), m_naturalSize);
  case IWORKToken::NS_URI_SF | IWORKToken::position :
    return makeContext<IWORKPositionContext>(getState(), m_pos);
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    return makeContext<IWORKSizeContext>(getState(), m_size);
  }

  return IWORKXMLContextPtr_t();
}

void IWORKGeometryContext::endOfElement()
{
  IWORKGeometryPtr_t geometry(new IWORKGeometry());
  geometry->m_naturalSize = get(m_naturalSize);
  geometry->m_size = bool(m_size) ? get(m_size) : get(m_naturalSize);
  geometry->m_position = get(m_pos);
  geometry->m_angle = m_angle;
  geometry->m_shearXAngle = m_shearXAngle;
  geometry->m_shearYAngle = m_shearYAngle;
  geometry->m_horizontalFlip = m_horizontalFlip;
  geometry->m_verticalFlip = m_verticalFlip;
  geometry->m_aspectRatioLocked = m_aspectRatioLocked;
  geometry->m_sizesLocked = m_sizesLocked;

  if (m_geometry)
    *m_geometry = geometry;
  else
    getCollector()->collectGeometry(geometry);
}

IWORKPositionContext::IWORKPositionContext(IWORKXMLParserState &state, optional<IWORKPosition> &position)
  : IWORKXMLEmptyContextBase(state)
  , m_position(position)
{
}

void IWORKPositionContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SFA | IWORKToken::x :
    m_x = lexical_cast<double>(value);
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::y :
    m_y = lexical_cast<double>(value);
    break;
  }
}

void IWORKPositionContext::endOfElement()
{
  if (m_x && m_y)
    m_position = IWORKPosition(get(m_x), get(m_y));
}

IWORKSizeContext::IWORKSizeContext(IWORKXMLParserState &state, boost::optional<IWORKSize> &size)
  : IWORKXMLEmptyContextBase(state)
  , m_size(size)
  , m_width()
  , m_height()
{
}

void IWORKSizeContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SFA | IWORKToken::w :
    m_width = lexical_cast<double>(value);
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::h :
    m_height = lexical_cast<double>(value);
    break;
  }
}

void IWORKSizeContext::endOfElement()
{
  if (m_width && m_height)
    m_size = IWORKSize(get(m_width), get(m_height));
}

IWORKTextBodyContext::IWORKTextBodyContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_layout(false)
  , m_para(false)
{
}

IWORKXMLContextPtr_t IWORKTextBodyContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::layout :
    if (m_layout || m_para)
    {
      ETONYEK_DEBUG_MSG(("layout following another element, not allowed, skipping\n"));
    }
    else
    {
      m_layout = true;
      return makeContext<LayoutContext>(getState());
    }
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::p :
    if (m_layout)
    {
      ETONYEK_DEBUG_MSG(("paragraph following layout, not allowed, skipping\n"));
    }
    else if (m_para)
    {
      return makeContext<PContext>(getState());
    }
    else
    {
      m_para = true;
      return makeContext<PContext>(getState());
    }
    break;
  }

  return IWORKXMLContextPtr_t();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
