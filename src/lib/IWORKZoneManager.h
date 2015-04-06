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

#include <deque>
#include <stack>

#include "IWORKOutputElements.h"
#include "IWORKTypes.h"

namespace libetonyek
{

/** A manager for output zones.
  *
  * It keeps a stack of currently opened zones and a list of saved
  * zones.
  *
  * There is always at least one zone on the stack.
  */
class IWORKZoneManager
{
  // disable copying
  IWORKZoneManager(const IWORKZoneManager &);
  IWORKZoneManager &operator=(const IWORKZoneManager &);

  typedef std::deque<IWORKOutputElements> ZoneList_t;
  typedef std::stack<IWORKOutputElements> ZoneStack_t;

public:
  IWORKZoneManager();
  ~IWORKZoneManager();

  /** Push a new zone onto the stack.
    */
  void push();

  /** Pop a zone from the stack
    */
  void pop();

  /** Save the current zone.
    *
    * It remains on the stack.
    *
    * @return The ID of the saved zone.
    */
  IWORKZoneID_t save();

  /** Get zone with ID @c id.
    *
    * @arg[in] id ID of the zone to get.
    */
  IWORKOutputElements &get(IWORKZoneID_t id);
  /** Get zone with ID @c id.
    *
    * @arg[in] id ID of the zone to get.
    */
  const IWORKOutputElements &get(IWORKZoneID_t id) const;

  /** Get the current zone.
    */
  IWORKOutputElements &getCurrent();
  /** Get the current zone.
    */
  const IWORKOutputElements &getCurrent() const;

private:
  ZoneStack_t m_active;
  ZoneList_t m_saved;
};

}

#endif // IWORKZONEMANAGER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
