/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKStyleRefContext.h"

#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKStyleRefContext::IWORKStyleRefContext(IWORKXMLParserState &state, const IWORKStyleMap_t &styleMap, const bool nested, const bool anonymous)
  : IWORKXMLEmptyContextBase(state)
  , m_styleMap(styleMap)
  , m_nested(nested)
  , m_anonymous(anonymous)
{
}

void IWORKStyleRefContext::attribute(int name, const char *value)
{
  IWORKXMLEmptyContextBase::attribute(name, value);
}

void IWORKStyleRefContext::endOfElement()
{
  if (getRef())
  {
    IWORKStylePtr_t style;
    const IWORKStyleMap_t::const_iterator it = m_styleMap.find(get(getRef()));
    if (m_styleMap.end() != it)
      style = it->second;
    if (isCollector())
      getCollector().collectStyle(style, m_anonymous);
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
