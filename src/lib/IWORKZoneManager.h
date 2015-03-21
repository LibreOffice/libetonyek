/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKZONEMANAGER_H_INCLUDED
#define IWORKZONEMANAGER_H_INCLUDED

#include <map>
#include <stack>

#include "IWORKOutputElements.h"
#include "IWORKTypes.h"

namespace libetonyek
{

class IWORKZoneManager
{
  // disable copying
  IWORKZoneManager(const IWORKZoneManager &);
  IWORKZoneManager &operator=(const IWORKZoneManager &);

  typedef std::map<IWORKZoneID_t, IWORKOutputElements> ZoneList_t;

public:
  IWORKZoneManager();
  ~IWORKZoneManager();

  IWORKZoneID_t open();
  void close();

  IWORKZoneID_t openTransient();
  void closeTransient();

  bool active() const;
  bool exists(IWORKZoneID_t id) const;

  IWORKOutputElements &get(IWORKZoneID_t id);
  const IWORKOutputElements &get(IWORKZoneID_t id) const;

  IWORKOutputElements &getCurrent();
  const IWORKOutputElements &getCurrent() const;

  IWORKZoneID_t getCurrentId() const;

private:
  IWORKZoneID_t doOpen(bool transient);
  void doClose(bool transient);

private:
  ZoneList_t m_zoneList;
  std::stack<IWORKZoneID_t> m_activeZones;
  std::stack<bool> m_activeZonesTransiency;
  IWORKOutputElements *m_current;
  unsigned m_counter;
};

}

#endif // IWORKZONEMANAGER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
