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

KEY2StyleRefContext::KEY2StyleRefContext(KEY2ParserState &state, const int id, const bool nested, const bool anonymous)
  : KEY2XMLEmptyContextBase(state)
  , m_base(state, id, nested)
  , m_id(id)
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
  const boost::optional<std::string> dummyIdent;
  const boost::optional<IWORKPropertyMap> dummyProps;

  // TODO: need to get the style
  switch (m_id)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle_ref :
  {
    IWORKStylePtr_t style;
    if (getRef())
    {
      KEYDictionary &dict = getDictionary();
      const IWORKStyleMap_t::const_iterator it = dict.m_layoutStyles.find(get(getRef()));
      if (dict.m_layoutStyles.end() != it)
        style = it->second;
    }
    getCollector()->collectStyle(style, m_anonymous);
    // TODO: this call is in the wrong place
    getCollector()->setLayoutStyle(style);
    break;
  }
  case IWORKToken::NS_URI_SF | IWORKToken::placeholder_style_ref :
  {
    IWORKStylePtr_t style;
    if (getRef())
    {
      KEYDictionary &dict = getDictionary();
      const IWORKStyleMap_t::const_iterator it = dict.m_placeholderStyles.find(get(getRef()));
      if (dict.m_placeholderStyles.end() != it)
        style = it->second;
    }
    getCollector()->collectPlaceholderStyle(style, m_anonymous);
    break;
  }
  default :
    m_base.endOfElement();
    break;
  }
}

KEYDictionary &KEY2StyleRefContext::getDictionary()
{
  return getState().getDictionary();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
