/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKHeaderFooterContext.h"

#include "IWORKCollector.h"
#include "IWORKTextStorageElement.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKHeaderFooterContext::IWORKHeaderFooterContext(IWORKXMLParserState &state, const CollectFunction_t &collect)
  : IWORKXMLElementContextBase(state)
  , m_collect(collect)
  , m_name()
{
}

void IWORKHeaderFooterContext::startOfElement()
{
  if (isCollector())
    getCollector().startText();
}

void IWORKHeaderFooterContext::attribute(const int name, const char *const value)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::name))
    m_name = value;
  else
    IWORKXMLElementContextBase::attribute(name, value);
}

IWORKXMLContextPtr_t IWORKHeaderFooterContext::element(const int name)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::text_storage))
    return makeContext<IWORKTextStorageElement>(getState());
  return IWORKXMLContextPtr_t();
}

void IWORKHeaderFooterContext::endOfElement()
{
  if (isCollector())
  {
    if (m_name)
      m_collect(get(m_name));
    getCollector().endText();
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
