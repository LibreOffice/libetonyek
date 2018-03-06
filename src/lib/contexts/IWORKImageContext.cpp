/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKImageContext.h"

#include <memory>

#include "IWORKDataElement.h"
#include "IWORKSizeElement.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKImageContext::IWORKImageContext(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content)
  : IWORKXMLElementContextBase(state)
  , m_content(content)
  , m_size()
  , m_data()
  , m_fillColor()
{
}

IWORKXMLContextPtr_t IWORKImageContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::data :
    return std::make_shared<IWORKDataElement>(getState(), m_data, m_fillColor);
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    return std::make_shared<IWORKSizeElement>(getState(), m_size);
  default:
    ETONYEK_DEBUG_MSG(("IWORKImageContext::element: find unknown element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}

void IWORKImageContext::endOfElement()
{
  m_content = std::make_shared<IWORKMediaContent>();
  m_content->m_size = m_size;
  m_content->m_data = m_data;
  m_content->m_fillColor = m_fillColor;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
