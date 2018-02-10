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
#include "IWORKFieldElement.h"
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
  , m_delayedBreak(IWORK_BREAK_NONE)
{
}

void IWORKSpanElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::style :
    m_style=getState().getStyleByName(value, getState().getDictionary().m_characterStyles);
    break;
  default:
    ETONYEK_DEBUG_MSG(("IWORKSpanElement::attribute: find unknown attribute\n"));
  }

}

IWORKXMLContextPtr_t IWORKSpanElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::br : // ok to ignore ?
    return IWORKXMLContextPtr_t();
  case IWORKToken::NS_URI_SF | IWORKToken::crbr :
  case IWORKToken::NS_URI_SF | IWORKToken::intratopicbr :
  case IWORKToken::NS_URI_SF | IWORKToken::lnbr :
    ensureOpened();
    return makeContext<IWORKBrContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::contbr :
    m_delayedBreak=IWORK_BREAK_COLUMN;
    return IWORKXMLContextPtr_t();
  case IWORKToken::NS_URI_SF | IWORKToken::pgbr :
    m_delayedBreak=IWORK_BREAK_PAGE;
    return IWORKXMLContextPtr_t();
  case IWORKToken::NS_URI_SF | IWORKToken::tab :
    ensureOpened();
    return makeContext<IWORKTabElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::date_time :
    ensureOpened();
    return makeContext<IWORKFieldElement>(getState(),IWORK_FIELD_DATETIME);
  case IWORKToken::NS_URI_SF | IWORKToken::filename :
    ensureOpened();
    return makeContext<IWORKFieldElement>(getState(),IWORK_FIELD_FILENAME);
  case IWORKToken::NS_URI_SF | IWORKToken::page_count :
    ensureOpened();
    return makeContext<IWORKFieldElement>(getState(),IWORK_FIELD_PAGECOUNT);
  case IWORKToken::NS_URI_SF | IWORKToken::page_number :
    ensureOpened();
    return makeContext<IWORKFieldElement>(getState(),IWORK_FIELD_PAGENUMBER);
  case IWORKToken::NS_URI_SF | IWORKToken::insertion_point :
    return IWORKXMLContextPtr_t();
  default:
    ETONYEK_DEBUG_MSG(("IWORKSpanElement::element: find some unknown element\n"));
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
  if (bool(getState().m_currentText))
  {
    if (m_opened)
      getState().m_currentText->flushSpan();
    if (m_delayedBreak==IWORK_BREAK_PAGE)
      getState().m_currentText->insertPageBreak();
    else if (m_delayedBreak==IWORK_BREAK_COLUMN)
      getState().m_currentText->insertColumnBreak();
    // This is needed to handle mixed paragraph content correctly. If
    // there is a plain text following a span, it must not have the
    // style of the preceding span.
    getState().m_currentText->setSpanStyle(IWORKStylePtr_t());
  }
}

void IWORKSpanElement::ensureOpened()
{
  if (!m_opened)
  {
    if (bool(getState().m_currentText))
      getState().m_currentText->setSpanStyle(m_style);
    m_opened = true;
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
