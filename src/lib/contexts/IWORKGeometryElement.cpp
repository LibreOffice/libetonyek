/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKGeometryElement.h"

#include <boost/lexical_cast.hpp>

#include "libetonyek_xml.h"
#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKPositionElement.h"
#include "IWORKSizeElement.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

using boost::lexical_cast;

IWORKGeometryElement::IWORKGeometryElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_geometry(nullptr)
  , m_naturalSize()
  , m_size()
  , m_pos()
  , m_angle()
  , m_shearXAngle()
  , m_shearYAngle()
  , m_aspectRatioLocked()
  , m_sizesLocked()
  , m_horizontalFlip()
  , m_verticalFlip()
{
}

IWORKGeometryElement::IWORKGeometryElement(IWORKXMLParserState &state, IWORKGeometryPtr_t &geometry)
  : IWORKXMLElementContextBase(state)
  , m_geometry(&geometry)
  , m_naturalSize()
  , m_size()
  , m_pos()
  , m_angle()
  , m_shearXAngle()
  , m_shearYAngle()
  , m_aspectRatioLocked()
  , m_sizesLocked()
  , m_horizontalFlip()
  , m_verticalFlip()
{
}

void IWORKGeometryElement::attribute(const int name, const char *const value)
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

IWORKXMLContextPtr_t IWORKGeometryElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::naturalSize :
    return makeContext<IWORKSizeElement>(getState(), m_naturalSize);
  case IWORKToken::NS_URI_SF | IWORKToken::position :
    return makeContext<IWORKPositionElement>(getState(), m_pos);
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    return makeContext<IWORKSizeElement>(getState(), m_size);
  default:
    ETONYEK_DEBUG_MSG(("IWORKGeometryElement::element: find unknown element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}

void IWORKGeometryElement::endOfElement()
{
  IWORKGeometryPtr_t geometry(new IWORKGeometry());
  if (m_naturalSize)
  {
    geometry->m_naturalSize = get(m_naturalSize);
    geometry->m_size = get(m_naturalSize);
  }
  if (m_size)
    geometry->m_size = get(m_size);
  if (m_pos)
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
  else if (isCollector())
    getCollector().collectGeometry(geometry);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
