/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKSpanElement.h"

#include "IWORKBrContext.h"
#include "IWORKDictionary.h"
#include "IWORKTabElement.h"
#include "IWORKText.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKSpanElement::IWORKSpanElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_style()
  , m_opened(false)
{
}

void IWORKSpanElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::style :
  {
    const IWORKStyleMap_t::const_iterator it = getState().getDictionary().m_characterStyles.find(value);
    if (getState().getDictionary().m_characterStyles.end() != it)
      m_style = it->second;
    break;
  }
  }
}

IWORKXMLContextPtr_t IWORKSpanElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::crbr :
  case IWORKToken::NS_URI_SF | IWORKToken::intratopicbr :
  case IWORKToken::NS_URI_SF | IWORKToken::lnbr :
    ensureOpened();
    return makeContext<IWORKBrContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::tab :
    ensureOpened();
    return makeContext<IWORKTabElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void IWORKSpanElement::text(const char *const value)
{
  ensureOpened();
  if (bool(getState().m_currentText))
    getState().m_currentText->insertText(value);
}

void IWORKSpanElement::endOfElement()
{
  if (m_opened && bool(getState().m_currentText))
    getState().m_currentText->closeSpan();
}

void IWORKSpanElement::ensureOpened()
{
  if (!m_opened)
  {
    if (bool(getState().m_currentText))
      getState().m_currentText->openSpan(m_style);
    m_opened = true;
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
