/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */


#include "KEY1ContentElement.h"

#include "IWORKProperties.h"
#include "IWORKText.h"
#include "KEY1ParserState.h"
#include "KEY1StringConverter.h"
#include "KEY1Token.h"
#include "KEY1XMLContextBase.h"

#include "libetonyek_xml.h"

namespace libetonyek
{
KEY1ContentElement::KEY1ContentElement(KEY1ParserState &state)
  : KEY1XMLElementContextBase(state)
  , m_divStyle(state, IWORKStylePtr_t())
  , m_spanStyle(state, IWORKStylePtr_t())
  , m_delayedLineBreak(false)
{
}

void KEY1ContentElement::attribute(const int name, const char *const value)
{
  if (m_divStyle.readAttribute(name, value) ||
      m_spanStyle.readAttribute(name, value))
    return;
  switch (name)
  {
  case KEY1Token::id :
    setId(value);
    break;
  default:
    ETONYEK_DEBUG_MSG(("KEY1ContentElement::attribute: unknown attribute with value=%s\n", value));
  }
}

IWORKXMLContextPtr_t KEY1ContentElement::element(const int name)
{
  switch (name)
  {
  case KEY1Token::div | KEY1Token::NS_URI_KEY :
    return makeContext<KEY1DivElement>(getState(), m_spanStyle.getStyle(), m_divStyle.getStyle(), m_delayedLineBreak);
  case KEY1Token::span | KEY1Token::NS_URI_KEY :
    return makeContext<KEY1SpanElement>(getState(), m_spanStyle.getStyle(), m_delayedLineBreak);
  default:
    ETONYEK_DEBUG_MSG(("KEY1ContentElement::element: unknown element\n"));
  }
  return IWORKXMLContextPtr_t();
}

void KEY1ContentElement::CDATA(const char *value)
{
  if (bool(getState().m_currentText))
  {
    getState().m_currentText->setParagraphStyle(m_divStyle.getStyle());
    KEY1SpanElement::sendCDATA(value, getState().m_currentText, m_spanStyle.getStyle(), m_delayedLineBreak);
  }
}

void KEY1ContentElement::text(const char *value)
{
  if (bool(getState().m_currentText))
  {
    if (m_delayedLineBreak)
    {
      getState().m_currentText->flushParagraph();
      m_delayedLineBreak=false;
    }
    getState().m_currentText->setParagraphStyle(m_divStyle.getStyle());
    getState().m_currentText->setSpanStyle(m_spanStyle.getStyle());
    getState().m_currentText->insertText(value);
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
