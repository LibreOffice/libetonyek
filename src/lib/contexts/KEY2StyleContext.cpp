/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEY2StyleContext.h"

#include "libetonyek_xml.h"
#include "IWORKPropertyMapElement.h"
#include "IWORKToken.h"
#include "KEY2Dictionary.h"
#include "KEY2ParserState.h"
#include "KEY2Token.h"
#include "KEYCollector.h"
#include "KEYTypes.h"

namespace libetonyek
{

namespace
{

class PropertyMapElement : public KEY2XMLElementContextBase
{
public:
  PropertyMapElement(KEY2ParserState &state, IWORKPropertyMap &propMap);

private:
  IWORKXMLContextPtr_t element(int name) override;

private:
  IWORKPropertyMapElement m_base;
};

PropertyMapElement::PropertyMapElement(KEY2ParserState &state, IWORKPropertyMap &propMap)
  : KEY2XMLElementContextBase(state)
  , m_base(state, propMap)
{
}

IWORKXMLContextPtr_t PropertyMapElement::element(const int name)
{
  // switch (name)
  // {
  // case KEY2Token::NS_URI_KEY | KEY2Token::headlineParagraphStyle :
  // return makeContext<StylePropertyElement>(getState(), name, m_propMap);
  // }

  return m_base.element(name);
}

}

KEY2StyleContext::KEY2StyleContext(KEY2ParserState &state, IWORKStyleMap_t *const styleMap, const bool nested)
  : KEY2XMLElementContextBase(state)
  , m_props()
  , m_base(state, m_props, styleMap, nested)
  , m_styleMap(styleMap)
  , m_nested(nested)
{
}

void KEY2StyleContext::attribute(const int name, const char *const value)
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
    KEY2XMLElementContextBase::attribute(name, value);
    break;
  }

  m_base.attribute(name, value);
}

IWORKXMLContextPtr_t KEY2StyleContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::property_map :
    return makeContext<PropertyMapElement>(getState(), m_props);
  }

  return IWORKXMLContextPtr_t();
}

void KEY2StyleContext::endOfElement()
{
  const IWORKStylePtr_t style(new IWORKStyle(m_props, m_ident, m_parentIdent));
  if (getId() && bool(m_styleMap))
    (*m_styleMap)[get(getId())] = style;
  if (m_ident && !m_nested && getState().m_stylesheet)
    getState().m_stylesheet->m_styles[get(m_ident)] = style;
  if (isCollector())
    getCollector().collectStyle(style);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
