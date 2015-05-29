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
#include "KEY2ParserState.h"
#include "KEY2Token.h"
#include "KEYCollector.h"
#include "KEYDictionary.h"
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
  virtual IWORKXMLContextPtr_t element(int name);

private:
  IWORKPropertyMapElement m_base;
  IWORKPropertyMap &m_propMap;
};

PropertyMapElement::PropertyMapElement(KEY2ParserState &state, IWORKPropertyMap &propMap)
  : KEY2XMLElementContextBase(state)
  , m_base(state, propMap)
  , m_propMap(propMap)
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

KEY2StyleContext::KEY2StyleContext(KEY2ParserState &state, IWORKStyleMap_t *style, const bool nested)
  : KEY2XMLElementContextBase(state)
  , m_props()
  , m_base(state, style, m_props, nested)
  , m_style(style)
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
  if (getId())
    (*m_style)[get(getId())] = style;
  getCollector()->collectStyle(style, m_nested);
//   switch (m_id)
//   {
//   case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle :
//   {
//     const IWORKStylePtr_t style(new IWORKStyle(m_props, m_ident, m_parentIdent));
//     if (getId())
//       getDictionary().m_layoutStyles[get(getId())] = style;
//     getCollector()->collectStyle(style, m_nested);
//     break;
//   }
//   case IWORKToken::NS_URI_SF | IWORKToken::placeholder_style :
//   {
//     const IWORKStylePtr_t style(new IWORKStyle(m_props, m_ident, m_parentIdent));
//     if (getId())
//       getDictionary().m_placeholderStyles[get(getId())] = style;
//     getCollector()->collectStyle(style, m_nested);
//     break;
//   }
//   case IWORKToken::NS_URI_SF | IWORKToken::slide_style :
//     getCollector()->collectStyle(IWORKStylePtr_t(), m_nested);
//     break;
//   default :
//     m_base.endOfElement();
//     break;
//   }
}

KEYDictionary &KEY2StyleContext::getDictionary()
{
  return getState().getDictionary();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
