/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKLayoutElement.h"

#include "IWORKCollector.h"
#include "IWORKPElement.h"
#include "IWORKStyle.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKLayoutElement::IWORKLayoutElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

void IWORKLayoutElement::attribute(const int name, const char *)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::style) == name)
  {
    // TODO: fetch the style
    if (isCollector())
      getCollector().collectStyle(IWORKStylePtr_t(), false);
  }
}

IWORKXMLContextPtr_t IWORKLayoutElement::element(const int name)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::p) == name)
    return makeContext<IWORKPElement>(getState());

  return IWORKXMLContextPtr_t();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
