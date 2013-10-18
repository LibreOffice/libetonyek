/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEYPropertyMap.h"
#include "KEYStyleContext.h"

namespace libkeynote
{

KEYStyleContext::KEYStyleContext()
  : m_stack()
{
}

void KEYStyleContext::push(const KEYStylePtr_t &style)
{
  m_stack.push_front(style);
}

void KEYStyleContext::pop()
{
  m_stack.pop_front();
}

boost::any KEYStyleContext::find(const std::string &property, const bool lookInParent) const
{
  boost::any value;

  for (Stack_t::const_iterator it = m_stack.begin(); value.empty() && (m_stack.end() != it); ++it)
    value = (*it)->getPropertyMap().get(property, lookInParent);

  return value;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
