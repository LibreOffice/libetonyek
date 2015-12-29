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
#include "IWORKPushCollector.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

namespace
{

typedef IWORKContainerContext<IWORKListLabelTypeInfo_t, IWORKListLabelTypeinfoElement, IWORKPushCollector, IWORKToken::NS_URI_SF | IWORKToken::list_label_typeinfo, IWORKToken::NS_URI_SF | IWORKToken::list_label_typeinfo_ref> ArrayElement;

}

IWORKListLabelTypesProperty::IWORKListLabelTypesProperty(IWORKXMLParserState &state, std::deque<IWORKListLabelTypeInfo_t> &elements)
  : IWORKXMLElementContextBase(state)
  , m_elements(elements)
{
}

IWORKXMLContextPtr_t IWORKListLabelTypesProperty::element(const int name)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::mutable_array))
    return makeContext<ArrayElement>(getState(), m_elements);
  return IWORKXMLContextPtr_t();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
