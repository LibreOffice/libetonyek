/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKStyleContext.h"

#include "libetonyek_xml.h"
#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKPropertyMapElement.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKStyleContext::IWORKStyleContext(IWORKXMLParserState &state, IWORKStyleMap_t *style, const bool nested)
  : IWORKXMLElementContextBase(state)
  , m_style(style)
  , m_nested(nested)
  , m_ownProps()
  , m_props(m_ownProps)
{
}

IWORKStyleContext::IWORKStyleContext(IWORKXMLParserState &state, IWORKStyleMap_t *style, IWORKPropertyMap &props, const bool nested)
  : IWORKXMLElementContextBase(state)
  , m_style(style)
  , m_nested(nested)
  , m_ownProps()
  , m_props(props)
{
}

void IWORKStyleContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::ident :
    m_ident = value;
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::parent_ident :
    m_parentIdent = value;
    break;
  default :
    IWORKXMLElementContextBase::attribute(name, value);
    break;
  }
}

IWORKXMLContextPtr_t IWORKStyleContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::property_map :
    return makeContext<IWORKPropertyMapElement>(getState(), m_props);
  }

  return IWORKXMLContextPtr_t();
}

void IWORKStyleContext::endOfElement()
{
  const IWORKStylePtr_t style(new IWORKStyle(m_props, m_ident, m_parentIdent));
  if (getId())
    (*m_style)[get(getId())] = style;
  getCollector()->collectStyle(style, m_nested);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
