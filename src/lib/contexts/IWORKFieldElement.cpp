/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKFieldElement.h"

#include "libetonyek_xml.h"
#include "IWORKSpanElement.h"
#include "IWORKText.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKFieldElement::IWORKFieldElement(IWORKXMLParserState &state, IWORKFieldType type)
  : IWORKXMLMixedContextBase(state)
  , m_type(type)
  , m_value()
  , m_val()
{
}

void IWORKFieldElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::val | IWORKToken::NS_URI_SF : // date-time
    m_val=value;
    break;
  case IWORKToken::value | IWORKToken::NS_URI_SF : // page-count, page-number
    m_value=int_cast(value);
    break;
  default:
    break;
  }

}

IWORKXMLContextPtr_t IWORKFieldElement::element(const int name)
{
  // filename has also a span child with text, let ignore it
  if (m_type!=IWORK_FIELD_DATETIME)
    return IWORKXMLContextPtr_t();

  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::span :
    return makeContext<IWORKSpanElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void IWORKFieldElement::text(const char *const value)
{
  if (bool(getState().m_currentText) && m_type==IWORK_FIELD_DATETIME)
    getState().m_currentText->insertText(value);
}

void IWORKFieldElement::endOfElement()
{
  if (m_type!=IWORK_FIELD_DATETIME)
    getState().m_currentText->insertField(m_type);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
