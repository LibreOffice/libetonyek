/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKSizeElement.h"

#include "libetonyek_xml.h"
#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKSizeElement::IWORKSizeElement(IWORKXMLParserState &state, boost::optional<IWORKSize> &size)
  : IWORKXMLEmptyContextBase(state)
  , m_size(size)
  , m_width()
  , m_height()
{
}

void IWORKSizeElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SFA | IWORKToken::w :
    m_width = try_double_cast(value);
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::h :
    m_height = try_double_cast(value);
    break;
  }
}

void IWORKSizeElement::endOfElement()
{
  if (m_width && m_height)
    m_size = IWORKSize(get(m_width), get(m_height));
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
