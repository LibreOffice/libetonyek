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
#include "IWORKPushCollector.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

namespace
{

typedef IWORKMutableArrayElement<double, IWORKNumberElement<double>, IWORKPushCollector, IWORKToken::NS_URI_SF | IWORKToken::number> MutableArrayElement;

}

IWORKListLabelIndentsProperty::IWORKListLabelIndentsProperty(IWORKXMLParserState &state, std::deque<double> &elements)
  : IWORKXMLElementContextBase(state)
  , m_elements(elements)
  , m_ref()
{
}

IWORKXMLContextPtr_t IWORKListLabelIndentsProperty::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::mutable_array :
    return makeContext<MutableArrayElement>(getState(), getState().getDictionary().m_doubleArrays, m_elements);
  case IWORKToken::NS_URI_SF | IWORKToken::mutable_array_ref :
    return makeContext<IWORKRefContext>(getState(), m_ref);
  }
  return IWORKXMLContextPtr_t();
}

void IWORKListLabelIndentsProperty::endOfElement()
{
  if (m_ref)
  {
    const boost::unordered_map<ID_t, std::deque<double> >::const_iterator it = getState().getDictionary().m_doubleArrays.find(get(m_ref));
    if (it != getState().getDictionary().m_doubleArrays.end())
      m_elements = it->second;
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
