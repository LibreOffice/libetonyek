/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKBinaryElement.h"

#include <memory>

#include "IWORKDataElement.h"
#include "IWORKDictionary.h"
#include "IWORKSizeElement.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKBinaryElement::IWORKBinaryElement(IWORKXMLParserState &state, IWORKMediaContentPtr_t &value)
  : IWORKXMLElementContextBase(state)
  , m_value(value)
  , m_size()
  , m_data()
  , m_fillColor()
{
}

IWORKXMLContextPtr_t IWORKBinaryElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::data :
    return std::make_shared<IWORKDataElement>(getState(), m_data, m_fillColor);
  case IWORKToken::NS_URI_SF | IWORKToken::size :
    return std::make_shared<IWORKSizeElement>(getState(), m_size);
  default:
    break;
  }
  return IWORKXMLContextPtr_t();
}

void IWORKBinaryElement::endOfElement()
{
  IWORKMediaContentPtr_t binary;

  if (bool(m_data) || bool(m_fillColor))
  {
    binary = std::make_shared<IWORKMediaContent>();
    binary->m_data = m_data;
    binary->m_fillColor = m_fillColor;
    binary->m_size = m_size;
  }

  m_value = binary;
  if (getId())
    getState().getDictionary().m_binaries[get(getId())] = m_value;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
