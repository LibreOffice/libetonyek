/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKSTYLE_H_INCLUDED
#define IWORKSTYLE_H_INCLUDED

#include "IWORKStyle_fwd.h"

#include <boost/any.hpp>
#include <boost/optional.hpp>

#include "IWORKPropertyMap.h"
#include "IWORKStylesheet.h"

namespace libetonyek
{

class IWORKStyleStack;

/** Represents a hierarchical style.
  */
class IWORKStyle
{
public:
  IWORKStyle(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent);
  IWORKStyle(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const IWORKStylePtr_t &parent);

  /** Find the parent style by its ID.
    *
    * @arg[in] stylesheet the stylesheet to use for looking for parent.
    * @return @c true if the parent style has been found, @c false otherwise.
    */
  bool link(const IWORKStylesheetPtr_t &stylesheet);

  /** Copy attributes from parent style (recursively).
    *
    * @todo Will this have any effect on performance anyway?
    */
  void flatten();

  /** Get the style's property map.
    */
  const IWORKPropertyMap &getPropertyMap() const;

  /** Check for the presence of a property.
    *
    * @returns true if the property is present
    */
  template<class Property>
  bool has() const
  {
    return m_props.has<Property>(true);
  }

  /** Retrieve the value of a property.
    *
    * @returns the found value
    */
  template<class Property>
  const typename IWORKPropertyInfo<Property>::ValueType &get() const
  {
    return m_props.get<Property>(true);
  }

  const boost::optional<std::string> &getIdent() const;

private:
  IWORKPropertyMap m_props;

  const boost::optional<std::string> m_ident;
  const boost::optional<std::string> m_parentIdent;
  IWORKStylePtr_t m_parent;
};

}

#endif // IWORKSTYLE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
