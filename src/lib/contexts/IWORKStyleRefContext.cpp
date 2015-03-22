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

IWORKStyleRefContext::IWORKStyleRefContext(IWORKXMLParserState &state, const int id, const bool nested, const bool anonymous)
  : IWORKXMLEmptyContextBase(state)
  , m_id(id)
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
  const boost::optional<std::string> dummyIdent;
  const boost::optional<IWORKPropertyMap> dummyProps;

  // TODO: need to get the style
  switch (m_id)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::cell_style_ref :
    getCollector()->collectStyle(IWORKStylePtr_t(), m_anonymous);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::characterstyle_ref :
  {
    IWORKStylePtr_t style;
    if (getRef())
    {
      const IWORKStyleMap_t::const_iterator it = getDictionary().m_characterStyles.find(get(getRef()));
      if (getDictionary().m_characterStyles.end() != it)
        style = it->second;
    }
    getCollector()->collectStyle(style, m_anonymous);
    break;
  }
  case IWORKToken::NS_URI_SF | IWORKToken::liststyle_ref :
    getCollector()->collectStyle(IWORKStylePtr_t(), m_anonymous);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle_ref :
  {
    IWORKStylePtr_t style;
    if (getRef())
    {
      const IWORKStyleMap_t::const_iterator it = getDictionary().m_paragraphStyles.find(get(getRef()));
      if (getDictionary().m_paragraphStyles.end() != it)
        style = it->second;
    }
    getCollector()->collectStyle(style, m_anonymous);
    break;
  }
  case IWORKToken::NS_URI_SF | IWORKToken::vector_style_ref :
    getCollector()->collectStyle(IWORKStylePtr_t(), m_anonymous);
    break;
  default :
    break;
  }
}

IWORKDictionary &IWORKStyleRefContext::getDictionary()
{
  return getState().getDictionary();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
