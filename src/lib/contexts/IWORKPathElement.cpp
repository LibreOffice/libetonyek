/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKPathElement.h"

#include <utility>

#include <boost/numeric/conversion/cast.hpp>
#include <boost/optional.hpp>

#include "libetonyek_xml.h"
#include "IWORKBezierElement.h"
#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKPositionElement.h"
#include "IWORKRefContext.h"
#include "IWORKSizeElement.h"
#include "IWORKToken.h"
#include "IWORKTokenizer.h"
#include "IWORKTypes.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

using boost::numeric_cast;
using boost::optional;

namespace
{

class ConnectionPathElement : public IWORKXMLElementContextBase
{
public:
  explicit ConnectionPathElement(IWORKXMLParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  optional<IWORKSize> m_size;
  optional<IWORKPosition> m_point;
};

ConnectionPathElement::ConnectionPathElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_size()
  , m_point()
{
}

IWORKXMLContextPtr_t ConnectionPathElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::point :
    return std::make_shared<IWORKPositionElement>(getState(), m_point);
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    return std::make_shared<IWORKSizeElement>(getState(), m_size);
  default:
    ETONYEK_DEBUG_MSG(("ConnectionPathElement::element[IWORKPathElement.cpp]: find unknown element\n"));
  }

  return IWORKXMLContextPtr_t();
}

void ConnectionPathElement::endOfElement()
{
  if (m_size && isCollector())
  {
    getCollector().collectConnectionPath(get(m_size), m_point);
  }
}

}

namespace
{

class PointPathElement : public IWORKXMLElementContextBase
{
public:
  explicit PointPathElement(IWORKXMLParserState &state);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  bool m_star;
  bool m_doubleArrow;
  optional<IWORKSize> m_size;
  optional<IWORKPosition> m_point;
};

PointPathElement::PointPathElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_star(false)
  , m_doubleArrow(false) // right arrow is the default (by my decree .-)
  , m_size()
  , m_point()
{
}

void PointPathElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::type :
  {
    switch (getToken(value))
    {
    case IWORKToken::double_ :
      m_doubleArrow = true;
      break;
    case IWORKToken::right :
      break;
    case IWORKToken::star :
      m_star = true;
      break;
    default :
      ETONYEK_DEBUG_MSG(("unknown point path type: %s\n", value));
      break;
    }
    break;
  }
  default :
    IWORKXMLElementContextBase::attribute(name, value);
    break;
  }
}

IWORKXMLContextPtr_t PointPathElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::point :
    return std::make_shared<IWORKPositionElement>(getState(), m_point);
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    return std::make_shared<IWORKSizeElement>(getState(), m_size);
  default:
    ETONYEK_DEBUG_MSG(("PointPathElement::element[IWORKPathElement.cpp]: find unknown element\n"));
  }

  return IWORKXMLContextPtr_t();
}

void PointPathElement::endOfElement()
{
  IWORKSize size;
  if (m_size)
    size = get(m_size);

  if (isCollector())
  {
    if (m_star)
    {
      if (m_point)
        getCollector().collectStarPath(size, numeric_cast<unsigned>(get(m_point).m_x), get(m_point).m_y);
      else
        getCollector().collectStarPath(size, 0, 0);
    }
    else
    {
      if (m_point)
        getCollector().collectArrowPath(size, get(m_point).m_x, get(m_point).m_y, m_doubleArrow);
      else
        getCollector().collectArrowPath(size, 0, 0, m_doubleArrow);
    }
  }
}

}

namespace
{

class ScalarPathElement : public IWORKXMLElementContextBase
{
public:
  explicit ScalarPathElement(IWORKXMLParserState &state);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  optional<IWORKSize> m_size;
  bool m_polygon;
  double m_value;
};

ScalarPathElement::ScalarPathElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_size()
  , m_polygon(false)
  , m_value(0)
{
}

void ScalarPathElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::scalar :
    m_value = double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::type :
  {
    switch (getToken(value))
    {
    case IWORKToken::_0 :
      break;
    case IWORKToken::_1 :
      m_polygon = true;
      break;
    default :
      ETONYEK_DEBUG_MSG(("unknown scalar path type: %s\n", value));
      break;
    }
    break;
  }
  default :
    IWORKXMLElementContextBase::attribute(name, value);
    break;
  }
}

IWORKXMLContextPtr_t ScalarPathElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    return std::make_shared<IWORKSizeElement>(getState(), m_size);
  default:
    ETONYEK_DEBUG_MSG(("ScalarPathElement::element[IWORKPathElement.cpp]: find unknown element\n"));
  }

  return IWORKXMLContextPtr_t();
}

void ScalarPathElement::endOfElement()
{
  IWORKSize size;
  if (m_size)
    size = get(m_size);

  if (isCollector())
  {
    if (m_polygon)
      getCollector().collectPolygonPath(size, numeric_cast<unsigned>(m_value));
    else
      getCollector().collectRoundedRectanglePath(size, m_value);
  }
}

}

namespace
{

class BezierPathElement : public IWORKXMLElementContextBase
{
public:
  explicit BezierPathElement(IWORKXMLParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

  IWORKPathPtr_t m_path;
  boost::optional<ID_t> m_ref;
};

BezierPathElement::BezierPathElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_path()
  , m_ref()
{
}

IWORKXMLContextPtr_t BezierPathElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::bezier :
    return std::make_shared<IWORKBezierElement>(getState(), m_path);
  case IWORKToken::NS_URI_SF | IWORKToken::bezier_ref :
    return std::make_shared<IWORKRefContext>(getState(), m_ref);
  default:
    ETONYEK_DEBUG_MSG(("BezierPathElement::element[IWORKPathElement.cpp]: find unknown element\n"));
  }

  return IWORKXMLContextPtr_t();
}

void BezierPathElement::endOfElement()
{
  if (!isCollector())
    return;
  if (m_ref)
  {
    const IWORKPathMap_t::const_iterator it = getState().getDictionary().m_beziers.find(get(m_ref));
    if (getState().getDictionary().m_beziers.end() != it)
      m_path=it->second;
    else
    {
      ETONYEK_DEBUG_MSG(("BezierPathElement::endOfElement[IWORKPathElement.cpp]: can not find bezier path %s\n", get(m_ref).c_str()));
    }
  }
  getCollector().collectBezier(m_path);
  getCollector().collectBezierPath();
}

}

namespace
{

class Callout2PathElement : public IWORKXMLElementContextBase
{
public:
  explicit Callout2PathElement(IWORKXMLParserState &state);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  optional<IWORKSize> m_size;
  double m_cornerRadius;
  bool m_tailAtCenter;
  double m_tailPosX;
  double m_tailPosY;
  double m_tailSize;
};

Callout2PathElement::Callout2PathElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_size()
  , m_cornerRadius(0)
  , m_tailAtCenter(false)
  , m_tailPosX(0)
  , m_tailPosY(0)
  , m_tailSize(0)
{
}

void Callout2PathElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::cornerRadius :
    m_cornerRadius = double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::tailAtCenter :
    m_tailAtCenter = bool_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::tailPositionX :
    m_tailPosX = double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::tailPositionY :
    m_tailPosY = double_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::tailSize :
    m_tailSize = double_cast(value);
    break;
  default :
    IWORKXMLElementContextBase::attribute(name, value);
    break;
  }
}

IWORKXMLContextPtr_t Callout2PathElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    return std::make_shared<IWORKSizeElement>(getState(), m_size);
  default:
    ETONYEK_DEBUG_MSG(("Callout2PathElement::element[IWORKPathElement.cpp]: find unknown element\n"));
  }

  return IWORKXMLContextPtr_t();
}

void Callout2PathElement::endOfElement()
{
  if (isCollector())
    getCollector().collectCalloutPath(get_optional_value_or(m_size, IWORKSize()), m_cornerRadius, m_tailSize, m_tailPosX, m_tailPosY, m_tailAtCenter);
}

}

IWORKPathElement::IWORKPathElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t IWORKPathElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::bezier_path :
  case IWORKToken::NS_URI_SF | IWORKToken::editable_bezier_path :
    return std::make_shared<BezierPathElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::callout2_path :
    return std::make_shared<Callout2PathElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::connection_path :
    return std::make_shared<ConnectionPathElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::point_path :
    return std::make_shared<PointPathElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::scalar_path :
    return std::make_shared<ScalarPathElement>(getState());
  default:
    ETONYEK_DEBUG_MSG(("IWORKPathElement::element: find unknown element\n"));
  }

  return IWORKXMLContextPtr_t();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
