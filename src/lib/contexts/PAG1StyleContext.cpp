/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "PAG1StyleContext.h"

#include <string>

#include <boost/optional.hpp>

#include "libetonyek_xml.h"
#include "IWORKProperties.h"
#include "IWORKPropertyContext.h"
#include "IWORKPropertyMapElement.h"
#include "IWORKStyle.h"
#include "IWORKToken.h"
#include "PAG1ParserState.h"
#include "PAG1Token.h"
#include "PAGCollector.h"
#include "PAGProperties.h"
#include "PAGTypes.h"

namespace libetonyek
{

using boost::optional;

using std::string;

namespace
{

class PagemasterElement : public IWORKXMLEmptyContextBase
{
public:
  PagemasterElement(IWORKXMLParserState &state, optional<IWORKPageMaster> &value);

private:
  void attribute(int name, const char *value) override;
  void endOfElement() override;

private:
  optional<IWORKPageMaster> &m_value;
  optional<string> m_header;
  optional<string> m_footer;
};

PagemasterElement::PagemasterElement(IWORKXMLParserState &state, optional<IWORKPageMaster> &value)
  : IWORKXMLEmptyContextBase(state)
  , m_value(value)
  , m_header()
  , m_footer()
{
}

void PagemasterElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case PAG1Token::NS_URI_SL | PAG1Token::footer :
    m_footer = value;
    break;
  case PAG1Token::NS_URI_SL | PAG1Token::header :
    m_header = value;
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::ID : // store me?
    IWORKXMLEmptyContextBase::attribute(name, value);
    break;
  default :
    ETONYEK_DEBUG_MSG(("PagemasterElement::attribute[PAG1StyleContext.cpp]: found unexpected attribute\n"));
    IWORKXMLEmptyContextBase::attribute(name, value);
    break;
  }
}

void PagemasterElement::endOfElement()
{
  if (m_header || m_footer)
  {
    IWORKPageMaster pageMaster;
    if (m_header)
      pageMaster.m_header = get(m_header);
    if (m_footer)
      pageMaster.m_footer = get(m_footer);
    m_value = pageMaster;
  }
}

}

namespace
{

typedef IWORKPropertyContext<property::EvenPageMaster, PagemasterElement, IWORKToken::NS_URI_SF | IWORKToken::pagemaster> EvenPageMasterElement;
typedef IWORKPropertyContext<property::FirstPageMaster, PagemasterElement, IWORKToken::NS_URI_SF | IWORKToken::pagemaster> FirstPageMasterElement;
typedef IWORKPropertyContext<property::OddPageMaster, PagemasterElement, IWORKToken::NS_URI_SF | IWORKToken::pagemaster> OddPageMasterElement;

}

namespace
{

class PropertyMapElement : public PAG1XMLElementContextBase
{
public:
  PropertyMapElement(PAG1ParserState &state, IWORKPropertyMap &propMap);

private:
  IWORKXMLContextPtr_t element(int name) override;

private:
  IWORKPropertyMapElement m_base;
  IWORKPropertyMap &m_propMap;
};

PropertyMapElement::PropertyMapElement(PAG1ParserState &state, IWORKPropertyMap &propMap)
  : PAG1XMLElementContextBase(state)
  , m_base(state, propMap)
  , m_propMap(propMap)
{
}

IWORKXMLContextPtr_t PropertyMapElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::evenPageMaster :
    return std::make_shared<EvenPageMasterElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::firstPageMaster :
    return std::make_shared<FirstPageMasterElement>(getState(), m_propMap);
  case IWORKToken::NS_URI_SF | IWORKToken::oddPageMaster :
    return std::make_shared<OddPageMasterElement>(getState(), m_propMap);
  default:
    break;
  }

  return m_base.element(name);
}

}

PAG1StyleContext::PAG1StyleContext(PAG1ParserState &state, IWORKStyleMap_t *const styleMap, const char *const defaultParent, const bool /*nested*/)
  : PAG1XMLElementContextBase(state)
  , m_props()
  , m_base(state, m_props, styleMap, defaultParent/*, nested*/)
  , m_ident()
  , m_parentIdent()
{
}

void PAG1StyleContext::attribute(const int name, const char *const value)
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
    PAG1XMLElementContextBase::attribute(name, value);
    break;
  }

  m_base.attribute(name, value);
}

IWORKXMLContextPtr_t PAG1StyleContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::property_map :
    return std::make_shared<PropertyMapElement>(getState(), m_props);
  default:
    ETONYEK_DEBUG_MSG(("PAG1StyleContext::element: found unexpected element %d\n", name));
  }

  return IWORKXMLContextPtr_t();
}

void PAG1StyleContext::endOfElement()
{
  m_base.endOfElement();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
