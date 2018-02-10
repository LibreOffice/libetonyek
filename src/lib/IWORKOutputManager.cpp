/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKOutputManager.h"

namespace libetonyek
{

IWORKOutputManager::IWORKOutputManager()
  : m_active()
  , m_saved()
{
  push();
}

IWORKOutputManager::~IWORKOutputManager()
{
  pop();
  assert(m_active.empty());
}

void IWORKOutputManager::push()
{
  m_active.push(IWORKOutputElements());
}

void IWORKOutputManager::pop()
{
  assert(!m_active.empty());
  m_active.pop();
}

IWORKOutputID_t IWORKOutputManager::save()
{
  assert(!m_active.empty());
  m_saved.push_back(m_active.top());
  return IWORKOutputID_t(m_saved.size() - 1);
}

IWORKOutputElements &IWORKOutputManager::get(IWORKOutputID_t id)
{
  assert(m_saved.size() > id);
  return m_saved.at(id);
}

const IWORKOutputElements &IWORKOutputManager::get(IWORKOutputID_t id) const
{
  return const_cast<IWORKOutputManager *>(this)->get(id);
}

IWORKOutputElements &IWORKOutputManager::getCurrent()
{
  assert(!m_active.empty());
  return m_active.top();
}

const IWORKOutputElements &IWORKOutputManager::getCurrent() const
{
  return const_cast<IWORKOutputManager *>(this)->getCurrent();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
