/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEY2StyleRefContext.h"

#include "KEY2ParserState.h"
#include "KEY2Token.h"
#include "KEYCollector.h"
#include "KEYDictionary.h"
#include "KEYTypes.h"

namespace libetonyek
{

KEY2StyleRefContext::KEY2StyleRefContext(KEY2ParserState &state,const IWORKStyleMap_t &styleMap, const bool nested, const bool anonymous)
  : KEY2XMLEmptyContextBase(state)
  , m_base(state, styleMap, nested)
  , m_styleMap(styleMap)
  , m_nested(nested)
  , m_anonymous(anonymous)
{
}

void KEY2StyleRefContext::attribute(const int name, const char *const value)
{
  KEY2XMLEmptyContextBase::attribute(name, value);
  m_base.attribute(name, value);
}

void KEY2StyleRefContext::endOfElement()
{
  IWORKStylePtr_t style;
  if (getRef())
  {
    const IWORKStyleMap_t::const_iterator it = m_styleMap.find(get(getRef()));
    if (m_styleMap.end() != it)
      style = it->second;
  }
  getCollector()->collectStyle(style, m_anonymous);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
