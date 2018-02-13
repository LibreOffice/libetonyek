/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKTextElement.h"

#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKTextStorageElement.h"
#include "IWORKText.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKTextElement::IWORKTextElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_layoutStyleRef()
  , m_stylesheet()
{
}

void IWORKTextElement::attribute(const int name, const char *value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SFA | IWORKToken::ID : // TODO: storeme ?
    IWORKXMLElementContextBase::attribute(name, value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::layoutstyle :
    m_layoutStyleRef = value;
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::tscale : // find one time with value 90
    break;
  default:
    ETONYEK_DEBUG_MSG(("IWORKTextElement::attribute: find some unknown attribute\n"));
  }
}

IWORKXMLContextPtr_t IWORKTextElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::text_storage :
    return std::make_shared<IWORKTextStorageElement>(getState(), m_stylesheet);
  default:
    ETONYEK_DEBUG_MSG(("IWORKTextElement::element: find some unknown element\n"));
  }

  return IWORKXMLContextPtr_t();
}

void IWORKTextElement::endOfElement()
{
  if (!isCollector() || !bool(m_layoutStyleRef))
    return;
  IWORKStylePtr_t style= getState().getStyleByName(get(m_layoutStyleRef).c_str(), getState().getDictionary().m_layoutStyles,false);
  if (!style && m_stylesheet)
    style = m_stylesheet->find(get(m_layoutStyleRef));
  if (!style)
  {
    ETONYEK_DEBUG_MSG(("IWORKTextElement::endOfElement: can not find style %s\n", get(m_layoutStyleRef).c_str()));
  }
  if (bool(getState().m_currentText))
    getState().m_currentText->setLayoutStyle(style);
}
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
