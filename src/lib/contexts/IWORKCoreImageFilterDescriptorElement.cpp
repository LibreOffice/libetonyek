/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKCoreImageFilterDescriptorElement.h"

#include "IWORKDictionary.h"
#include "IWORKToken.h"
#include "IWORKTokenizer.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKCoreImageFilterDescriptorElement::IWORKCoreImageFilterDescriptorElement(IWORKXMLParserState &state, bool &isShadow)
  : IWORKXMLEmptyContextBase(state)
  , m_isShadow(isShadow)
{
}

void IWORKCoreImageFilterDescriptorElement::attribute(const int name, const char *const value)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::filterClassName) == name)
    m_isShadow = getState().getTokenizer().getId(value) == IWORKToken::SFIUDropShadow;
  else
    IWORKXMLEmptyContextBase::attribute(name, value);
}

void IWORKCoreImageFilterDescriptorElement::endOfElement()
{
  if (getId())
    getState().getDictionary().m_filterDescriptors[get(getId())].m_isShadow = m_isShadow;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
