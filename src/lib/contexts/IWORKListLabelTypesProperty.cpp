/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKListLabelTypesProperty.h"

#include "IWORKContainerContext.h"
#include "IWORKDictionary.h"
#include "IWORKListLabelTypeinfoElement.h"
#include "IWORKMutableArrayElement.h"
#include "IWORKPropertyMap.h"
#include "IWORKProperties.h"
#include "IWORKPushCollector.h"
#include "IWORKToken.h"
#include "IWORKTypes.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

namespace
{
typedef IWORKMutableArrayElement<IWORKListLabelTypeInfo_t, IWORKListLabelTypeinfoElement, IWORKPushCollector, IWORKToken::NS_URI_SF | IWORKToken::list_label_typeinfo, IWORKToken::NS_URI_SF | IWORKToken::list_label_typeinfo_ref> MutableArrayElement;
}

IWORKListLabelTypesProperty::IWORKListLabelTypesProperty(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : IWORKXMLElementContextBase(state)
  , m_propertyMap(propMap)
  , m_elements()
  , m_ref()
{
}

IWORKXMLContextPtr_t IWORKListLabelTypesProperty::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::array:
  case IWORKToken::NS_URI_SF | IWORKToken::mutable_array :
    return std::make_shared<MutableArrayElement>(getState(), getState().getDictionary().m_listLabelTypesArrays,
                                                 getState().getDictionary().m_listLabelTypeInfos, m_elements);
  case IWORKToken::NS_URI_SF | IWORKToken::array_ref :
  case IWORKToken::NS_URI_SF | IWORKToken::mutable_array_ref :
    return std::make_shared<IWORKRefContext>(getState(), m_ref);
  default:
    break;
  }
  ETONYEK_DEBUG_MSG(("IWORKListLabelTypesProperty::element: unknown element %d\n", name));
  return IWORKXMLContextPtr_t();
}

void IWORKListLabelTypesProperty::endOfElement()
{
  if (m_ref)
  {
    const std::unordered_map<ID_t, std::deque<IWORKListLabelTypeInfo_t> >::const_iterator it = getState().getDictionary().m_listLabelTypesArrays.find(get(m_ref));
    if (it != getState().getDictionary().m_listLabelTypesArrays.end())
      m_propertyMap.put<property::ListLabelTypes>(it->second);
    else
    {
      ETONYEK_DEBUG_MSG(("IWORKListLabelTypesProperty::endOfElement: unknown element %s\n", get(m_ref).c_str()));
    }
  }
  else
    m_propertyMap.put<property::ListLabelTypes>(m_elements);
}
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
