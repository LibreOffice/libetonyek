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
  : m_zoneList()
  , m_activeZones()
  , m_activeZonesTransiency()
  , m_current(0)
  , m_counter(0)
{
}

IWORKZoneManager::~IWORKZoneManager()
{
  assert(m_activeZones.empty());
  assert(m_activeZonesTransiency.empty());
}

IWORKZoneID_t IWORKZoneManager::open()
{
  return doOpen(false);
}

void IWORKZoneManager::close()
{
  doClose(false);
}

IWORKZoneID_t IWORKZoneManager::openTransient()
{
  return doOpen(true);
}

void IWORKZoneManager::closeTransient()
{
  doClose(true);
}

bool IWORKZoneManager::active() const
{
  return !m_activeZones.empty();
}

bool IWORKZoneManager::exists(IWORKZoneID_t id) const
{
  return m_zoneList.find(id) != m_zoneList.end();
}

IWORKOutputElements &IWORKZoneManager::get(IWORKZoneID_t id)
{
  ZoneList_t::iterator it = m_zoneList.find(id);
  assert(m_zoneList.end() != it);
  return it->second;
}

const IWORKOutputElements &IWORKZoneManager::get(IWORKZoneID_t id) const
{
  return const_cast<IWORKZoneManager *>(this)->get(id);
}

IWORKOutputElements &IWORKZoneManager::getCurrent()
{
  assert(m_current);
  return *m_current;
}

const IWORKOutputElements &IWORKZoneManager::getCurrent() const
{
  return const_cast<IWORKZoneManager *>(this)->getCurrent();
}

IWORKZoneID_t IWORKZoneManager::getCurrentId() const
{
  assert(!m_activeZones.empty());
  return m_activeZones.top();
}

IWORKZoneID_t IWORKZoneManager::doOpen(const bool transient)
{
  const IWORKZoneID_t currentId = m_counter;

  m_current = &m_zoneList.insert(ZoneList_t::value_type(m_counter++, IWORKOutputElements())).first->second;
  m_activeZones.push(currentId);
  m_activeZonesTransiency.push(transient);

  return currentId;
}

void IWORKZoneManager::doClose(const bool transient)
{
  assert(!m_activeZones.empty());
  assert(!m_activeZonesTransiency.empty());
  assert(m_activeZonesTransiency.top() == transient);

  if (m_activeZonesTransiency.top())
  {
    const ZoneList_t::iterator it = m_zoneList.find(m_activeZones.top());
    assert(m_zoneList.end() != it);
    m_zoneList.erase(it);
  }

  m_activeZones.pop();
  m_activeZonesTransiency.pop();

  if (m_activeZones.empty())
  {
    m_current = 0;
  }
  else
  {
    const ZoneList_t::iterator it = m_zoneList.find(m_activeZones.top());
    assert(m_zoneList.end() != it);
    m_current = &it->second;
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
