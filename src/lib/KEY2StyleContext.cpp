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
#include "IWORKPropertyMapContext.h"
#include "KEY2ParserState.h"
#include "KEY2Token.h"
#include "KEYCollector.h"
#include "KEYDictionary.h"
#include "KEYTypes.h"

namespace libetonyek
{

namespace
{

class PropertyMapContext : public KEY2XMLElementContextBase
{
public:
  PropertyMapContext(KEY2ParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  IWORKPropertyMapContext m_base;
  IWORKPropertyMap &m_propMap;
};

PropertyMapContext::PropertyMapContext(KEY2ParserState &state, IWORKPropertyMap &propMap)
  : KEY2XMLElementContextBase(state)
  , m_base(state, propMap)
  , m_propMap(propMap)
{
}

IWORKXMLContextPtr_t PropertyMapContext::element(const int name)
{
  // switch (name)
  // {
  // case KEY2Token::NS_URI_KEY | KEY2Token::headlineParagraphStyle :
  // return makeContext<StylePropertyContext>(getState(), name, m_propMap);
  // }

  return m_base.element(name);
}

}

KEY2StyleContext::KEY2StyleContext(KEY2ParserState &state, const int id, const bool nested)
  : KEY2XMLElementContextBase(state)
  , m_props()
  , m_base(state, id, m_props, nested)
  , m_id(id)
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
    return makeContext<PropertyMapContext>(getState(), m_props);
  }

  return IWORKXMLContextPtr_t();
}

void KEY2StyleContext::endOfElement()
{
  switch (m_id)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle :
  {
    const IWORKStylePtr_t style(new IWORKStyle(m_props, m_ident, m_parentIdent));
    if (getId())
      getDictionary().m_layoutStyles[get(getId())] = style;
    getCollector()->collectLayoutStyle(style, m_nested);
    break;
  }
  case IWORKToken::NS_URI_SF | IWORKToken::placeholder_style :
  {
    const IWORKStylePtr_t style(new IWORKStyle(m_props, m_ident, m_parentIdent));
    if (getId())
      getDictionary().m_placeholderStyles[get(getId())] = style;
    getCollector()->collectPlaceholderStyle(style, m_nested);
    break;
  }
  case IWORKToken::NS_URI_SF | IWORKToken::slide_style :
    getCollector()->collectSlideStyle(IWORKStylePtr_t(), m_nested);
    break;
  default :
    m_base.endOfElement();
    break;
  }
}

KEYDictionary &KEY2StyleContext::getDictionary()
{
  return getState().getDictionary();
}

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
    getCollector()->collectLayoutStyle(style, m_anonymous);
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
