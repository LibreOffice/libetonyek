/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKDiscardContext.h"

#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKDiscardContext::IWORKDiscardContext(IWORKXMLParserState &state)
  : m_state(state)
  , m_level(0)
  , m_enableCollector(false)
{
}

void IWORKDiscardContext::startOfElement()
{
  if (m_level == 0)
  {
    m_enableCollector = m_state.m_enableCollector;
    m_state.m_enableCollector = false;
  }
  ++m_level;
}

void IWORKDiscardContext::attribute(int, const char *)
{
}

IWORKXMLContextPtr_t IWORKDiscardContext::element(int)
{
  return shared_from_this();
}

void IWORKDiscardContext::text(const char *)
{
}

void IWORKDiscardContext::endOfElement()
{
  assert(m_level > 0);

  --m_level;
  if (m_level == 0)
    m_state.m_enableCollector = m_enableCollector;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
