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
#include "IWORKPropertyMapContext.h"
#include "IWORKStyles.h"
#include "IWORKToken.h"
#include "IWORKXMLContexts.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKStyleContext::IWORKStyleContext(IWORKXMLParserState &state, const int id, const bool nested)
  : IWORKXMLElementContextBase(state)
  , m_id(id)
  , m_nested(nested)
  , m_ownProps()
  , m_props(m_ownProps)
{
}

IWORKStyleContext::IWORKStyleContext(IWORKXMLParserState &state, const int id, IWORKPropertyMap &props, const bool nested)
  : IWORKXMLElementContextBase(state)
  , m_id(id)
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
  switch (m_id)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::cell_style :
    getCollector()->collectCellStyle(IWORKStylePtr_t(), m_nested);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::characterstyle :
  {
    const IWORKStylePtr_t style(new IWORKStyle(m_props, m_ident, m_parentIdent));
    if (getId())
      getDictionary().m_characterStyles[get(getId())] = style;
    getCollector()->collectCharacterStyle(style, m_nested);
    break;
  }
  case IWORKToken::NS_URI_SF | IWORKToken::connection_style :
    getCollector()->collectConnectionStyle(IWORKStylePtr_t(), m_nested);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::graphic_style :
  {
    const IWORKStylePtr_t style(new IWORKStyle(m_props, m_ident, m_parentIdent));
    if (getId())
      getDictionary().m_graphicStyles[get(getId())] = style;
    getCollector()->collectGraphicStyle(style, m_nested);
    break;
  }
  case IWORKToken::NS_URI_SF | IWORKToken::liststyle :
    getCollector()->collectListStyle(IWORKStylePtr_t(), m_nested);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::paragraphstyle :
  {
    const IWORKStylePtr_t style(new IWORKStyle(m_props, m_ident, m_parentIdent));
    if (getId())
      getDictionary().m_paragraphStyles[get(getId())] = style;
    getCollector()->collectParagraphStyle(style, m_nested);
    break;
  }
  case IWORKToken::NS_URI_SF | IWORKToken::tabular_style :
    getCollector()->collectTabularStyle(IWORKStylePtr_t(), m_nested);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::vector_style :
    getCollector()->collectVectorStyle(IWORKStylePtr_t(), m_nested);
    break;
  default :
    ETONYEK_DEBUG_MSG(("unhandled style %d\n", m_id));
    break;
  }
}

IWORKDictionary &IWORKStyleContext::getDictionary()
{
  return getState().getDictionary();
}

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
    getCollector()->collectCellStyle(IWORKStylePtr_t(), m_anonymous);
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
    getCollector()->collectCharacterStyle(style, m_anonymous);
    break;
  }
  case IWORKToken::NS_URI_SF | IWORKToken::liststyle_ref :
    getCollector()->collectListStyle(IWORKStylePtr_t(), m_anonymous);
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
    getCollector()->collectParagraphStyle(style, m_anonymous);
    break;
  }
  case IWORKToken::NS_URI_SF | IWORKToken::vector_style_ref :
    getCollector()->collectVectorStyle(IWORKStylePtr_t(), m_anonymous);
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
