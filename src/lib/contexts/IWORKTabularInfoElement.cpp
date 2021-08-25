/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKTabularInfoElement.h"

#include <memory>

#include "libetonyek_xml.h"

#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKGeometryElement.h"
#include "IWORKRefContext.h"
#include "IWORKStyleContainer.h"
#include "IWORKTable.h"
#include "IWORKTabularModelElement.h"
#include "IWORKToken.h"
#include "IWORKWrapElement.h"
#include "IWORKXMLParserState.h"
#include "IWORKXMLContextBase.h"
#include "IWORKStyle.h"

#include "PAG1Token.h"

namespace libetonyek
{

namespace
{
typedef IWORKStyleContainer<IWORKToken::NS_URI_SF | IWORKToken::tabular_style, IWORKToken::NS_URI_SF | IWORKToken::tabular_style_ref> TabularStyleContext;
}

IWORKTabularInfoElement::IWORKTabularInfoElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_tableRef()
  , m_style()
  , m_order()
  , m_wrap()
{
}

void IWORKTabularInfoElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case PAG1Token::order | PAG1Token::NS_URI_SL :
    m_order=try_int_cast(value);
    break;
  default:
    IWORKXMLElementContextBase::attribute(name, value);
  }
}

void IWORKTabularInfoElement::startOfElement()
{
  getState().m_tableData = std::make_shared<IWORKTableData>();
  assert(!getState().m_currentTable);
  getState().m_currentTable = getCollector().createTable(getState().m_tableNameMap, getState().m_formatNameMap, getState().m_langManager);
  if (isCollector())
    getCollector().startLevel();
}

IWORKXMLContextPtr_t IWORKTabularInfoElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::geometry | IWORKToken::NS_URI_SF :
    return std::make_shared<IWORKGeometryElement>(getState());
  case IWORKToken::style | IWORKToken::NS_URI_SF :
    return std::make_shared<TabularStyleContext>(getState(), m_style, getState().getDictionary().m_tabularStyles);
  case IWORKToken::tabular_model | IWORKToken::NS_URI_SF :
    return std::make_shared<IWORKTabularModelElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::tabular_model_ref :
    return std::make_shared<IWORKRefContext>(getState(), m_tableRef);
  case IWORKToken::NS_URI_SF | IWORKToken::wrap : // USEME
    return std::make_shared<IWORKWrapElement>(getState(), m_wrap);
  default:
    ETONYEK_DEBUG_MSG(("IWORKTabularInfoElement::element: find some unknown element\n"));
  }

  return IWORKXMLContextPtr_t();
}

void IWORKTabularInfoElement::endOfElement()
{
  if (!isCollector())
    return;

  if (m_tableRef)
  {
    IWORKTableMap_t::const_iterator it=getState().getDictionary().m_tabulars.find(get(m_tableRef));
    if (it!=getState().getDictionary().m_tabulars.end())
      getState().m_currentTable=it->second;
    else
    {
      ETONYEK_DEBUG_MSG(("IWORKTabularInfoElement::endOfElement: can not find the table %s\n", get(m_tableRef).c_str()));
    }
  }
  if (getState().m_currentTable)
  {
    if (m_order)
      getState().m_currentTable->setOrder(get(m_order));
    if (m_style)
      getState().m_currentTable->setStyle(m_style);
  }

  getCollector().collectTable(getState().m_currentTable);
  getState().m_currentTable.reset();
  getCollector().endLevel();
}
}
/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
