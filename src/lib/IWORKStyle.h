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

#include <boost/any.hpp>
#include <boost/optional.hpp>

#include "IWORKPropertyMap.h"
#include "IWORKStyle_fwd.h"
#include "IWORKStylesheet.h"

namespace libetonyek
{

class IWORKStyleContext;

/** Represents a hierarchical style.
  */
class IWORKStyle
{
public:
  IWORKStyle(const IWORKPropertyMap &props, const boost::optional<std::string> &ident, const boost::optional<std::string> &parentIdent);

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

  /** Look up a property in this style.
    *
    * @arg[in] property the property to look up
    * @arg[in] context the context for dynamic lookup
    * @returns the found property or empty boost::any
    */
  boost::any lookup(const char *property, const IWORKStyleContext &context) const;

private:
  IWORKPropertyMap m_props;

  const boost::optional<std::string> m_ident;
  const boost::optional<std::string> m_parentIdent;
  IWORKStylePtr_t m_parent;
};

}

#endif // IWORKSTYLE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
