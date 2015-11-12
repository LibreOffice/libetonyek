/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKLayoutElement.h"

#include "IWORKDictionary.h"
#include "IWORKPElement.h"
#include "IWORKStyle.h"
#include "IWORKText.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKLayoutElement::IWORKLayoutElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_opened(false)
  , m_style()
{
}

void IWORKLayoutElement::attribute(const int name, const char *const value)
{
  if ((IWORKToken::NS_URI_SF | IWORKToken::style) == name)
  {
    const IWORKStyleMap_t::const_iterator it = getState().getDictionary().m_layoutStyles.find(value);
    if (it != getState().getDictionary().m_layoutStyles.end())
      m_style = it->second;
  }
}

IWORKXMLContextPtr_t IWORKLayoutElement::element(const int name)
{
  if (!m_opened)
    open();

  if ((IWORKToken::NS_URI_SF | IWORKToken::p) == name)
    return makeContext<IWORKPElement>(getState());

  return IWORKXMLContextPtr_t();
}

void IWORKLayoutElement::endOfElement()
{
  if (m_opened)
  {
    if (bool(getState().m_currentText))
      getState().m_currentText->closeLayout();
  }
}

void IWORKLayoutElement::open()
{
  assert(!m_opened);

  if (bool(getState().m_currentText))
    getState().m_currentText->openLayout(m_style);
  m_opened = true;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
