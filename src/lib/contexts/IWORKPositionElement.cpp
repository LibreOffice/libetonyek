/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKPositionElement.h"

#include <boost/lexical_cast.hpp>

#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

using boost::lexical_cast;

IWORKPositionElement::IWORKPositionElement(IWORKXMLParserState &state, boost::optional<IWORKPosition> &position)
  : IWORKXMLEmptyContextBase(state)
  , m_position(position)
{
}

void IWORKPositionElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SFA | IWORKToken::x :
    m_x = lexical_cast<double>(value);
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::y :
    m_y = lexical_cast<double>(value);
    break;
  }
}

void IWORKPositionElement::endOfElement()
{
  if (m_x && m_y)
    m_position = IWORKPosition(get(m_x), get(m_y));
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
