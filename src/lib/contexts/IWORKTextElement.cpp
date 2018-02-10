/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKTextElement.h"

#include "IWORKCollector.h"
#include "IWORKTextStorageElement.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKTextElement::IWORKTextElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

void IWORKTextElement::attribute(const int name, const char *value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SFA | IWORKToken::ID : // TODO: storeme ?
    IWORKXMLElementContextBase::attribute(name, value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle :
    // TODO: handle
    if (isCollector())
      getCollector().collectStyle(IWORKStylePtr_t());
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::tscale : // find one time with value 90
    break;
  default:
    ETONYEK_DEBUG_MSG(("IWORKTextElement::attribute: find some unknown attribute\n"));
  }
}

IWORKXMLContextPtr_t IWORKTextElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::text_storage :
    return makeContext<IWORKTextStorageElement>(getState());
  default:
    ETONYEK_DEBUG_MSG(("IWORKTextElement::element: find some unknown element\n"));
  }

  return IWORKXMLContextPtr_t();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
