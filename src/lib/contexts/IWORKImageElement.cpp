/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKImageElement.h"

#include "libetonyek_xml.h"
#include "IWORKBinaryElement.h"
#include "IWORKCollector.h"
#include "IWORKDataElement.h"
#include "IWORKDictionary.h"
#include "IWORKFilteredImageElement.h"
#include "IWORKGeometryElement.h"
#include "IWORKRefContext.h"
#include "IWORKSizeElement.h"
#include "IWORKStyleContainer.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

namespace
{
typedef IWORKStyleContainer<IWORKToken::NS_URI_SF | IWORKToken::graphic_style, IWORKToken::NS_URI_SF | IWORKToken::graphic_style_ref> GraphicStyleContext;
}

IWORKImageElement::IWORKImageElement(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content)
  : IWORKXMLElementContextBase(state)
  , m_locked()
  , m_content(content)
  , m_localContent()
  , m_filteredImage()
  , m_data()
  , m_size()
  , m_binaryRef()
  , m_style()
  , m_cropGeometry()
  , m_placeholderSize()
{
}

IWORKImageElement::IWORKImageElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_locked()
  , m_content(m_localContent)
  , m_localContent()
  , m_filteredImage()
  , m_data()
  , m_size()
  , m_binaryRef()
  , m_style()
  , m_cropGeometry()
  , m_placeholderSize()
{
}

void IWORKImageElement::startOfElement()
{
  if (isCollector())
    getCollector().startLevel();
}

void IWORKImageElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::locked :
    m_locked = bool_cast(value);
    break;
  default :
    IWORKXMLElementContextBase::attribute(name, value);
    break;
  }
}

IWORKXMLContextPtr_t IWORKImageElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::binary :
    return makeContext<IWORKBinaryElement>(getState(), m_content);
  case IWORKToken::NS_URI_SF | IWORKToken::binary_ref :
    return makeContext<IWORKRefContext>(getState(), m_binaryRef);
  case IWORKToken::NS_URI_SF | IWORKToken::crop_geometry : // USEME
    return makeContext<IWORKGeometryElement>(getState(), m_cropGeometry);
  case IWORKToken::NS_URI_SF | IWORKToken::data :
    return makeContext<IWORKDataElement>(getState(), m_data);
  case IWORKToken::NS_URI_SF | IWORKToken::filtered_image :
    return makeContext<IWORKFilteredImageElement>(getState(), m_filteredImage);
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::masking_shape_path_source :
  {
    static bool first=true;
    if (first)
    {
      ETONYEK_DEBUG_MSG(("IWORKImageElement::element: find some masking shape's paths\n"));
      first=false;
    }
    break;
  }
  case IWORKToken::NS_URI_SF | IWORKToken::placeholder_size : // USEME
    return makeContext<IWORKSizeElement>(getState(),m_placeholderSize);
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    return makeContext<IWORKSizeElement>(getState(),m_size);
  case IWORKToken::NS_URI_SF | IWORKToken::style : // USEME
    return makeContext<GraphicStyleContext>(getState(), m_style, getState().getDictionary().m_graphicStyles);
  default:
    ETONYEK_DEBUG_MSG(("IWORKImageElement::element: find some unknown element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}

void IWORKImageElement::endOfElement()
{
  if (m_binaryRef)
  {
    const IWORKMediaContentMap_t::const_iterator it = getState().getDictionary().m_binaries.find(get(m_binaryRef));
    if (getState().getDictionary().m_binaries.end() != it)
      m_content = it->second;
    else
    {
      ETONYEK_DEBUG_MSG(("IWORKImageElement::endOfElement: can not find image %s\n", get(m_binaryRef).c_str()));
    }
  }
  if (!m_content && m_filteredImage)
    m_content=m_filteredImage;
  else if (!m_content && m_data)
  {
    m_content.reset(new IWORKMediaContent());
    m_content->m_size = m_size;
    m_content->m_data = m_data;
  }
  if (getId() && m_content)
    getState().getDictionary().m_images[get(getId())]=m_content;
  if (isCollector())
  {
    if (m_style) getCollector().setGraphicStyle(m_style);
    getCollector().collectImage(m_content, m_locked ? get(m_locked) : false);
    getCollector().endLevel();
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
