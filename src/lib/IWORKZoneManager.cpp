/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKZoneManager.h"

namespace libetonyek
{

IWORKZoneManager::IWORKZoneManager()
  : m_active()
  , m_saved()
{
  push();
}

IWORKZoneManager::~IWORKZoneManager()
{
  pop();
  assert(m_active.empty());
}

void IWORKZoneManager::push()
{
  m_active.push(IWORKOutputElements());
}

void IWORKZoneManager::pop()
{
  assert(!m_active.empty());
  m_active.pop();
}

IWORKZoneID_t IWORKZoneManager::save()
{
  assert(!m_active.empty());
  m_saved.push_back(m_active.top());
  return m_saved.size() - 1;
}

IWORKOutputElements &IWORKZoneManager::get(IWORKZoneID_t id)
{
  assert(m_saved.size() > id);
  return m_saved.at(id);
}

const IWORKOutputElements &IWORKZoneManager::get(IWORKZoneID_t id) const
{
  return const_cast<IWORKZoneManager *>(this)->get(id);
}

IWORKOutputElements &IWORKZoneManager::getCurrent()
{
  assert(!m_active.empty());
  return m_active.top();
}

const IWORKOutputElements &IWORKZoneManager::getCurrent() const
{
  return const_cast<IWORKZoneManager *>(this)->getCurrent();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
