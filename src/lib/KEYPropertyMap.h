/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYPROPERTYMAP_H_INCLUDED
#define KEYPROPERTYMAP_H_INCLUDED

#include <boost/any.hpp>
#include <boost/unordered_map.hpp>

namespace libetonyek
{

/** Represents a (hierarchical) property map.
  */
class KEYPropertyMap
{
  typedef boost::unordered_map<std::string, boost::any> Map_t;

public:
  /** Construct an empty map.
    */
  KEYPropertyMap();

  /** Construct an empty map with parent @c parent.
    *
    * @arg[in] parent the parent of this map
    */
  explicit KEYPropertyMap(const KEYPropertyMap *parent);

  /** Construct a map as a copy of existing one.
    *
    * @arg[in] other the original
    */
  KEYPropertyMap(const KEYPropertyMap &other);

  /** Assign the content from another map.
    *
    * @arg[in] other the source map
    * @returns this map
    */
  KEYPropertyMap &operator=(const KEYPropertyMap &other);

  /** Swap the content with another map.
    *
    * @arg[inout] the other map
    */
  void swap(KEYPropertyMap &other);

  /** Set a new parent for this map.
    *
    * @arg[in] parent the new parent
    */
  void setParent(const KEYPropertyMap *parent);

  /** Retrieve value associated with @c key.
    *
    * If the value is not found in this map and @c lookInParent is @c
    * true, the parent map is searched (transitively).
    *
    * @arg[in] key the search key
    * @arg[in] lookInParent should the parent map be searched if the
    * key is not found in this map?
    * @returns the found value or empty value
    */
  boost::any get(const std::string &key, bool lookInParent = false) const;

  /** Set a new value for key @key.
    *
    * @arg[in] key the key
    * @arg[in] value the value to set
    */
  void set(const std::string &key, const boost::any &value);

private:
  Map_t m_map;
  const KEYPropertyMap *m_parent;
};

}

#endif // KEYPROPERTYMAP_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
