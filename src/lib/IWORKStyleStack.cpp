/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKStyleStack.h"

#include <cassert>

#include "IWORKPropertyMap.h"

namespace libetonyek
{

IWORKStyleStack::IWORKStyleStack()
  : m_stack()
{
}

IWORKStyleStack::~IWORKStyleStack()
{
  // FIXME: investigate
  // assert(m_stack.empty());
}

void IWORKStyleStack::push()
{
  m_stack.push_front(IWORKStylePtr_t());
}

void IWORKStyleStack::pop()
{
  m_stack.pop_front();
}

void IWORKStyleStack::set(const IWORKStylePtr_t &style)
{
  // FIXME: investigate
  // assert(!m_stack.front());

  m_stack.front() = style;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
