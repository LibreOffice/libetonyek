/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKStringElement.h"

#include "IWORKToken.h"

namespace libetonyek
{

IWORKStringElement::IWORKStringElement(IWORKXMLParserState &state, boost::optional<std::string> &str)
  : IWORKXMLEmptyContextBase(state)
  , m_string(str)
{
}

void IWORKStringElement::attribute(const int name, const char *const value)
{
  if ((IWORKToken::NS_URI_SFA | IWORKToken::string) == name)
    m_string = value;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
