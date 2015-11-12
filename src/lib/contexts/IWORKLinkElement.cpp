/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKLinkElement.h"

#include "IWORKSpanElement.h"
#include "IWORKText.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKLinkElement::IWORKLinkElement(IWORKXMLParserState &state)
  : IWORKXMLMixedContextBase(state)
  , m_opened(false)
{
}

void IWORKLinkElement::attribute(const int name, const char *const value)
{
  if (IWORKToken::href == name)
  {
    if (bool(getState().m_currentText))
      getState().m_currentText->openLink(value);
    m_opened = true;
  }
}

IWORKXMLContextPtr_t IWORKLinkElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::span :
    return makeContext<IWORKSpanElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void IWORKLinkElement::text(const char *const value)
{
  if (bool(getState().m_currentText))
    getState().m_currentText->insertText(value);
}

void IWORKLinkElement::endOfElement()
{
  if (m_opened && bool(getState().m_currentText))
    getState().m_currentText->closeLink();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
