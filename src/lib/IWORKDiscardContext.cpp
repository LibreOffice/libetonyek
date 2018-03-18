/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKDiscardContext.h"

#include <boost/optional.hpp>

#include "IWORKBezierElement.h"
#include "IWORKBinaryElement.h"
#include "IWORKCoreImageFilterDescriptorElement.h"
#include "IWORKDataElement.h"
#include "IWORKDictionary.h"
#include "IWORKListLabelGeometryElement.h"
#include "IWORKListLabelIndentsProperty.h"
#include "IWORKListLabelTypeinfoElement.h"
#include "IWORKStyleContext.h"
#include "IWORKTabsElement.h"
#include "IWORKTextLabelElement.h"
#include "IWORKToken.h"
#include "IWORKTypes.h"
#include "IWORKUnfilteredElement.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

struct IWORKDiscardContext::Data
{
  Data()
    : m_data()
    , m_mediaContent()
    , m_tabStops()
    , m_isShadow()
    , m_path()
    , m_propertyMap()
    , m_fillColor()
    , m_listLabelGeometry()
    , m_listLabelTypeInfo()
  {
  }

  IWORKDataPtr_t m_data;
  IWORKMediaContentPtr_t m_mediaContent;
  IWORKTabStops_t m_tabStops;
  bool m_isShadow;
  IWORKPathPtr_t m_path;
  IWORKPropertyMap m_propertyMap;
  boost::optional<IWORKColor> m_fillColor;
  boost::optional<IWORKListLabelGeometry> m_listLabelGeometry;
  boost::optional<IWORKListLabelTypeInfo_t> m_listLabelTypeInfo;
};

IWORKDiscardContext::IWORKDiscardContext(IWORKXMLParserState &state)
  : m_state(state)
  , m_level(0)
  , m_enableCollector(false)
  , m_data(new Data())
{
}

void IWORKDiscardContext::startOfElement()
{
  if (m_level == 0)
  {
    m_enableCollector = m_state.m_enableCollector;
    m_state.m_enableCollector = false;
  }
  ++m_level;
}

void IWORKDiscardContext::attribute(int, const char *)
{
}

IWORKXMLContextPtr_t IWORKDiscardContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::bezier :
    return std::make_shared<IWORKBezierElement>(m_state, m_data->m_path);
  case IWORKToken::NS_URI_SF | IWORKToken::binary :
    return std::make_shared<IWORKBinaryElement>(m_state, m_data->m_mediaContent);
  case IWORKToken::NS_URI_SF | IWORKToken::cell_style :
    return std::make_shared<IWORKStyleContext>(m_state, &m_state.getDictionary().m_cellStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::characterstyle :
    return std::make_shared<IWORKStyleContext>(m_state, &m_state.getDictionary().m_characterStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::core_image_filter_descriptor :
    return std::make_shared<IWORKCoreImageFilterDescriptorElement>(m_state, m_data->m_isShadow);
  case IWORKToken::NS_URI_SF | IWORKToken::data :
    m_data->m_data.reset();
    return std::make_shared<IWORKDataElement>(m_state, m_data->m_data, m_data->m_fillColor);
  case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle :
    return std::make_shared<IWORKStyleContext>(m_state, &m_state.getDictionary().m_layoutStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::liststyle :
    return std::make_shared<IWORKStyleContext>(m_state, &m_state.getDictionary().m_listStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::listLabelIndents :
    return std::make_shared<IWORKListLabelIndentsProperty>(m_state, m_data->m_propertyMap);
  case IWORKToken::NS_URI_SF | IWORKToken::list_label_geometry :
    return std::make_shared<IWORKListLabelGeometryElement>(m_state, m_data->m_listLabelGeometry);
  case IWORKToken::NS_URI_SF | IWORKToken::list_label_typeinfo :
    return std::make_shared<IWORKListLabelTypeinfoElement>(m_state, m_data->m_listLabelTypeInfo);
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle :
    return std::make_shared<IWORKStyleContext>(m_state, &m_state.getDictionary().m_paragraphStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::slide_style :
    return std::make_shared<IWORKStyleContext>(m_state, &m_state.getDictionary().m_slideStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::tabs :
    m_data->m_tabStops.clear();
    return std::make_shared<IWORKTabsElement>(m_state, m_data->m_tabStops);
  case IWORKToken::NS_URI_SF | IWORKToken::tabular_style :
    return std::make_shared<IWORKStyleContext>(m_state, &m_state.getDictionary().m_tabularStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::text_label :
    return std::make_shared<IWORKTextLabelElement>(m_state, m_data->m_listLabelTypeInfo);
  case IWORKToken::NS_URI_SF | IWORKToken::unfiltered :
    m_data->m_mediaContent.reset();
    return std::make_shared<IWORKUnfilteredElement>(m_state, m_data->m_mediaContent);
  default:
    break;
  }

  return shared_from_this();
}

void IWORKDiscardContext::text(const char *)
{
}

void IWORKDiscardContext::endOfElement()
{
  assert(m_level > 0);

  --m_level;
  if (m_level == 0)
    m_state.m_enableCollector = m_enableCollector;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
