/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKTextBodyElement.h"

#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKLayoutElement.h"
#include "IWORKPElement.h"
#include "IWORKText.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKTextBodyElement::IWORKTextBodyElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_layout(false)
  , m_para(false)
{
}

IWORKXMLContextPtr_t IWORKTextBodyElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::layout :
    if (m_layout || m_para)
    {
      ETONYEK_DEBUG_MSG(("IWORKTextBodyElement::element: layout following another element, not allowed, skipping\n"));
    }
    else
    {
      m_layout = true;
      return std::make_shared<IWORKLayoutElement>(getState());
    }
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::p :
    if (m_layout)
    {
      ETONYEK_DEBUG_MSG(("IWORKTextBodyElement::element: paragraph following layout, not allowed, skipping\n"));
    }
    else if (m_para)
    {
      return std::make_shared<IWORKPElement>(getState());
    }
    else
    {
      m_para = true;
      return std::make_shared<IWORKPElement>(getState());
    }
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::insertion_point :
  case IWORKToken::NS_URI_SF | IWORKToken::page_start : // with attribute page-index
  case IWORKToken::NS_URI_SF | IWORKToken::selection_end :
    break;
  default:
    ETONYEK_DEBUG_MSG(("IWORKTextBodyElement::element: find some unknown element\n"));
  }

  return IWORKXMLContextPtr_t();
}

void IWORKTextBodyElement::endOfElement()
{
  if (bool(getState().m_currentText))
    getState().m_currentText->flushList();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
