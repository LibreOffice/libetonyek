/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKColorElement.h"

#include <boost/lexical_cast.hpp>

#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

using boost::lexical_cast;

IWORKColorElement::IWORKColorElement(IWORKXMLParserState &state, boost::optional<IWORKColor> &color)
  : IWORKXMLEmptyContextBase(state)
  , m_color(color)
  , m_r(0)
  , m_g(0)
  , m_b(0)
  , m_a(0)
{
}

void IWORKColorElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  // TODO: check xsi:type too
  case IWORKToken::NS_URI_SFA | IWORKToken::a :
    m_a = lexical_cast<double>(value);
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::b :
    m_b = lexical_cast<double>(value);
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::g :
    m_g = lexical_cast<double>(value);
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::r :
    m_r = lexical_cast<double>(value);
    break;
  }
}

void IWORKColorElement::endOfElement()
{
  m_color = IWORKColor(m_r, m_g, m_b, m_a);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
