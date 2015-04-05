/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKPropertyMap.h"

namespace libetonyek
{

IWORKPropertyMap::IWORKPropertyMap()
  : m_map()
  , m_parent(0)
{
}

IWORKPropertyMap::IWORKPropertyMap(const IWORKPropertyMap *const parent)
  : m_map()
  , m_parent(parent)
{
}

IWORKPropertyMap::IWORKPropertyMap(const IWORKPropertyMap &other)
  : m_map(other.m_map)
  , m_parent(other.m_parent)
{
}

IWORKPropertyMap &IWORKPropertyMap::operator=(const IWORKPropertyMap &other)
{
  IWORKPropertyMap copy(other);
  swap(copy);
  return *this;
}

void IWORKPropertyMap::swap(IWORKPropertyMap &other)
{
  using std::swap;
  swap(m_map, other.m_map);
  swap(m_parent, other.m_parent);
}

void IWORKPropertyMap::setParent(const IWORKPropertyMap *const parent)
{
  m_parent = parent;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
