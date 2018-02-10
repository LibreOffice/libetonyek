/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKListLabelGeometriesProperty.h"

#include "IWORKContainerContext.h"
#include "IWORKDictionary.h"
#include "IWORKListLabelGeometryElement.h"
#include "IWORKMutableArrayElement.h"
#include "IWORKPropertyMap.h"
#include "IWORKProperties.h"
#include "IWORKPushCollector.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

namespace
{

typedef IWORKMutableArrayElement<IWORKListLabelGeometry, IWORKListLabelGeometryElement, IWORKPushCollector, IWORKToken::NS_URI_SF | IWORKToken::list_label_geometry, IWORKToken::NS_URI_SF | IWORKToken::list_label_geometry_ref> MutableArrayElement;
}

IWORKListLabelGeometriesProperty::IWORKListLabelGeometriesProperty(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : IWORKXMLElementContextBase(state)
  , m_propertyMap(propMap)
  , m_elements()
  , m_ref()
{
}

IWORKXMLContextPtr_t IWORKListLabelGeometriesProperty::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::array:
  case IWORKToken::NS_URI_SF | IWORKToken::mutable_array :
    return makeContext<MutableArrayElement>(getState(), getState().getDictionary().m_listLabelGeometriesArrays,
                                            getState().getDictionary().m_listLabelGeometries,
                                            m_elements);
  case IWORKToken::NS_URI_SF | IWORKToken::array_ref:
  case IWORKToken::NS_URI_SF | IWORKToken::mutable_array_ref :
    return makeContext<IWORKRefContext>(getState(), m_ref);
  default:
    break;
  }
  ETONYEK_DEBUG_MSG(("IWORKListLabelGeometriesProperty::element: unknown element %d\n", name));
  return IWORKXMLContextPtr_t();
}

void IWORKListLabelGeometriesProperty::endOfElement()
{
  if (m_ref)
  {
    const std::unordered_map<ID_t, std::deque<IWORKListLabelGeometry> >::const_iterator it = getState().getDictionary().m_listLabelGeometriesArrays.find(get(m_ref));
    if (it != getState().getDictionary().m_listLabelGeometriesArrays.end())
      m_propertyMap.put<property::ListLabelGeometries>(it->second);
    else
    {
      ETONYEK_DEBUG_MSG(("IWORKListLabelGeometriesProperty::endOfElement: unknown element %s\n", get(m_ref).c_str()));
    }
  }
  else
    m_propertyMap.put<property::ListLabelGeometries>(m_elements);
}
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
