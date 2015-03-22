/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKRefContext.h"

#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKRefContext::IWORKRefContext(IWORKXMLParserState &state, boost::optional<ID_t> &ref)
  : IWORKXMLEmptyContextBase(state)
  , m_ref(ref)
{
}

void IWORKRefContext::endOfElement()
{
  m_ref = getRef();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
