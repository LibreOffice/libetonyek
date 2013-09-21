/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNPROPERTYMAP_H_INCLUDED
#define KNPROPERTYMAP_H_INCLUDED

#include <boost/any.hpp>
#include <boost/unordered_map.hpp>

namespace libkeynote
{

/** Represents a (hierarchical) property map.
  */
class KNPropertyMap
{
  typedef boost::unordered_map<std::string, boost::any> Map_t;

public:
  /** Construct an empty map.
    */
  KNPropertyMap();

  /** Construct an empty map with parent @c parent.
    *
    * @arg[in] parent the parent of this map
    */
  explicit KNPropertyMap(const KNPropertyMap *parent);

  /** Construct a map as a copy of existing one.
    *
    * @arg[in] other the original
    */
  KNPropertyMap(const KNPropertyMap &other);

  /** Assign the content from another map.
    *
    * @arg[in] other the source map
    * @returns this map
    */
  KNPropertyMap &operator=(const KNPropertyMap &other);

  /** Swap the content with another map.
    *
    * @arg[inout] the other map
    */
  void swap(KNPropertyMap &other);

  /** Set a new parent for this map.
    *
    * @arg[in] parent the new parent
    */
  void setParent(const KNPropertyMap *parent);

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
  const KNPropertyMap *m_parent;
};

}

#endif // KNPROPERTYMAP_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
