/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKTextStorageElement.h"

#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKRefContext.h"
#include "IWORKTextBodyElement.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKTextStorageElement::IWORKTextStorageElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_stylesheetId()
  , m_hasStylesheet(false)
  , m_stylesheet(nullptr)
{
}

IWORKTextStorageElement::IWORKTextStorageElement(IWORKXMLParserState &state, IWORKStylesheetPtr_t &mainMap)
  : IWORKXMLElementContextBase(state)
  , m_stylesheetId()
  , m_hasStylesheet(false)
  , m_stylesheet(&mainMap)
{
}

IWORKXMLContextPtr_t IWORKTextStorageElement::element(const int name)
{
  sendStylesheet();

  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::stylesheet_ref :
    return std::make_shared<IWORKRefContext>(getState(), m_stylesheetId);
  case IWORKToken::NS_URI_SF | IWORKToken::text_body :
    return std::make_shared<IWORKTextBodyElement>(getState());
  default:
    ETONYEK_DEBUG_MSG(("IWORKTextStorageElement::element: find some unknown element\n"));
  }

  return IWORKXMLContextPtr_t();
}

void IWORKTextStorageElement::endOfElement()
{
  if (isCollector() && m_hasStylesheet)
  {
    getCollector().popStylesheet();
    getState().m_stylesheet=getCollector().getStylesheet();
    m_hasStylesheet=false;
  }
}

void IWORKTextStorageElement::sendStylesheet()
{
  if (!isCollector())
    return;

  if (m_stylesheetId) // a stylesheet has been found
  {
    const IWORKStylesheetMap_t::iterator it = getState().getDictionary().m_stylesheets.find(get(m_stylesheetId));
    if (it != getState().getDictionary().m_stylesheets.end())
    {
      getCollector().pushStylesheet(it->second);
      getState().m_stylesheet=it->second;
      if (m_stylesheet) *m_stylesheet=it->second;
      m_hasStylesheet = true;
    }
    else
    {
      ETONYEK_DEBUG_MSG(("IWORKTextStorageElement::sendStylesheet: can not find stylesheet %s\n", get(m_stylesheetId).c_str()));
    }
    m_stylesheetId.reset();
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
