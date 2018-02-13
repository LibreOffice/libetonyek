/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKListLabelIndentsProperty.h"

#include "IWORKDictionary.h"
#include "IWORKMutableArrayElement.h"
#include "IWORKNumberElement.h"
#include "IWORKPropertyMap.h"
#include "IWORKProperties.h"
#include "IWORKPushCollector.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

namespace
{

typedef IWORKMutableArrayElement<double, IWORKNumberElement<double>, IWORKPushCollector, IWORKToken::NS_URI_SF | IWORKToken::number> MutableArrayElement;

}

IWORKListLabelIndentsProperty::IWORKListLabelIndentsProperty(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : IWORKXMLElementContextBase(state)
  , m_propertyMap(propMap)
  , m_elements()
  , m_ref()
{
}

IWORKXMLContextPtr_t IWORKListLabelIndentsProperty::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::array :
  case IWORKToken::NS_URI_SF | IWORKToken::mutable_array :
    return std::make_shared<MutableArrayElement>(getState(), getState().getDictionary().m_doubleArrays, m_elements);
  case IWORKToken::NS_URI_SF | IWORKToken::mutable_array_ref :
  case IWORKToken::NS_URI_SF | IWORKToken::array_ref :
    return std::make_shared<IWORKRefContext>(getState(), m_ref);
  default:
    break;
  }
  ETONYEK_DEBUG_MSG(("IWORKListLabelIndentsProperty::element: unknown element %d\n", name));
  return IWORKXMLContextPtr_t();
}

void IWORKListLabelIndentsProperty::endOfElement()
{
  if (m_ref)
  {
    const std::unordered_map<ID_t, std::deque<double> >::const_iterator it = getState().getDictionary().m_doubleArrays.find(get(m_ref));
    if (it != getState().getDictionary().m_doubleArrays.end())
      m_propertyMap.put<property::ListLabelIndents>(it->second);
    else
    {
      ETONYEK_DEBUG_MSG(("IWORKListLabelIndentsProperty::endOfElement: unknown element %s\n", get(m_ref).c_str()));
    }
  }
  else
    m_propertyMap.put<property::ListLabelIndents>(m_elements);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
