/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKPROPERTYMAP_H_INCLUDED
#define IWORKPROPERTYMAP_H_INCLUDED

#include <boost/any.hpp>
#include <boost/unordered_map.hpp>

#include "IWORKPropertyInfo.h"

namespace libetonyek
{

/** Represents a (hierarchical) property map.
  */
class IWORKPropertyMap
{
public:
  class NotFoundException {};

private:
  typedef boost::unordered_map<IWORKPropertyID_t, boost::any> Map_t;

public:
  /** Construct an empty map.
    */
  IWORKPropertyMap();

  /** Construct an empty map with parent @c parent.
    *
    * @arg[in] parent the parent of this map
    */
  explicit IWORKPropertyMap(const IWORKPropertyMap *parent);

  /** Construct a map as a copy of existing one.
    *
    * @arg[in] other the original
    */
  IWORKPropertyMap(const IWORKPropertyMap &other);

  /** Assign the content from another map.
    *
    * @arg[in] other the source map
    * @returns this map
    */
  IWORKPropertyMap &operator=(const IWORKPropertyMap &other);

  /** Swap the content with another map.
    *
    * @arg[inout] the other map
    */
  void swap(IWORKPropertyMap &other);

  /** Set a new parent for this map.
    *
    * @arg[in] parent the new parent
    */
  void setParent(const IWORKPropertyMap *parent);

  /** Check for the presence of a property.
    *
    * If the property is not found in this map and @c lookInParent is @c
    * true, the parent map is searched (transitively).
    *
    * @arg[in] lookInParent should the parent map be searched if the
    * property is not found in this map?
    * @returns true if the property is present
    */
  template<class Property>
  bool has(bool lookInParent = false) const
  {
    const Map_t::const_iterator it = m_map.find(IWORKPropertyInfo<Property>::id);
    if (m_map.end() != it)
      return !it->second.empty();

    if (lookInParent && m_parent)
      return m_parent->has<Property>(lookInParent);

    return false;
  }

  template<class Property>
  bool clears(bool lookInParent = false) const
  {
    const Map_t::const_iterator it = m_map.find(IWORKPropertyInfo<Property>::id);
    if (m_map.end() != it)
      return it->second.empty();

    if (lookInParent && m_parent)
      return m_parent->clears<Property>(lookInParent);

    return false;
  }

  /** Retrieve the value of a property.
    *
    * If the property is not found in this map and @c lookInParent is @c
    * true, the parent map is searched (transitively).
    *
    * @arg[in] lookInParent should the parent map be searched if the
    * property is not found in this map?
    * @returns the found value
    */
  template<class Property>
  const typename IWORKPropertyInfo<Property>::ValueType &get(bool lookInParent = false) const
  {
    const Map_t::const_iterator it = m_map.find(IWORKPropertyInfo<Property>::id);
    if (m_map.end() != it)
    {
      if (!it->second.empty())
        return boost::any_cast<const typename IWORKPropertyInfo<Property>::ValueType &>(it->second);
    }
    else if (lookInParent && m_parent)
    {
      return m_parent->get<Property>(lookInParent);
    }

    throw NotFoundException();
  }

  /** Insert a new value for key @c key.
    *
    * @arg[in] value the value to insert
    */
  template<class Property>
  void put(const typename IWORKPropertyInfo<Property>::ValueType &value)
  {
    m_map[IWORKPropertyInfo<Property>::id] = value;
  }

  /** Clear property.
    *
    * This has the same effect on lookup as if the property did
    * exist in neither this property map, neither in any parent map.
    */
  template<class Property>
  void clear()
  {
    m_map[IWORKPropertyInfo<Property>::id] = boost::any();
  }

private:
  Map_t m_map;
  const IWORKPropertyMap *m_parent;
};

}

#endif // IWORKPROPERTYMAP_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
