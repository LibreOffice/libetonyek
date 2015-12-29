/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKPElement.h"

#include "IWORKBrContext.h"
#include "IWORKDictionary.h"
#include "IWORKLinkElement.h"
#include "IWORKSpanElement.h"
#include "IWORKStyle.h"
#include "IWORKTabElement.h"
#include "IWORKText.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"
#include "libetonyek_xml.h"

namespace libetonyek
{

IWORKPElement::IWORKPElement(IWORKXMLParserState &state)
  : IWORKXMLMixedContextBase(state)
  , m_style()
  , m_opened(false)
{
}

void IWORKPElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::list_level :
    m_listLevel = try_int_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::style :
  {
    const IWORKStyleMap_t::const_iterator it = getState().getDictionary().m_paragraphStyles.find(value);
    if (getState().getDictionary().m_paragraphStyles.end() != it)
      m_style = it->second;
    break;
  }
  }
}

IWORKXMLContextPtr_t IWORKPElement::element(const int name)
{
  ensureOpened();

  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::crbr :
  case IWORKToken::NS_URI_SF | IWORKToken::intratopicbr :
  case IWORKToken::NS_URI_SF | IWORKToken::lnbr :
    return makeContext<IWORKBrContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::span :
    return makeContext<IWORKSpanElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::tab :
    return makeContext<IWORKTabElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::link :
    return makeContext<IWORKLinkElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void IWORKPElement::text(const char *const value)
{
  ensureOpened();
  if (bool(getState().m_currentText))
    getState().m_currentText->insertText(value);
}

void IWORKPElement::endOfElement()
{
  ensureOpened();
  if (bool(getState().m_currentText))
    getState().m_currentText->flushParagraph();
}

void IWORKPElement::ensureOpened()
{
  if (!m_opened)
  {
    if (bool(getState().m_currentText))
    {
      getState().m_currentText->setParagraphStyle(m_style);
      getState().m_currentText->setListLevel(get_optional_value_or(m_listLevel, 0));
      getState().m_currentText->setListStyle(m_style);
    }
    m_opened = true;
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
