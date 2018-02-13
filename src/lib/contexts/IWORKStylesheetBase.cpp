/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKStylesheetBase.h"

#include <memory>

#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKStylesContext.h"
#include "IWORKStylesheet.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKStylesheetBase::IWORKStylesheetBase(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

void IWORKStylesheetBase::startOfElement()
{
  assert(!getState().m_stylesheet);

  getState().m_stylesheet = std::make_shared<IWORKStylesheet>();
}

IWORKXMLContextPtr_t IWORKStylesheetBase::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::styles :
    return makeContext<IWORKStylesContext>(getState(), false);
  case IWORKToken::NS_URI_SF | IWORKToken::anon_styles :
    return makeContext<IWORKStylesContext>(getState(), true);
  default:
    ETONYEK_DEBUG_MSG(("IWORKStylesContext::element: find some unknown element\n"));
  }

  return IWORKXMLContextPtr_t();
}

void IWORKStylesheetBase::endOfElement()
{
  assert(getState().m_stylesheet);

  if (getId())
    getState().getDictionary().m_stylesheets[get(getId())] = getState().m_stylesheet;
  if (isCollector())
    getCollector().collectStylesheet(getState().m_stylesheet);
  getState().m_stylesheet.reset();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
