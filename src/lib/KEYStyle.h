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

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

#include "KEYStylesheet.h"
#include "KEYTypes_fwd.h"

namespace libetonyek
{

class IWORKPropertyMap;

/** Represents a hierarchical style.
  */
class KEYStyle
{
public:
  virtual ~KEYStyle() = 0;

  /** Find the parent style by its ID.
    *
    * @arg[in] stylesheet the stylesheet to use for looking for parent.
    * @return @c true if the parent style has been found, @c false otherwise.
    */
  virtual bool link(const KEYStylesheetPtr_t &stylesheet) = 0;

  /** Copy attributes from parent style (recursively).
    *
    * @todo Will this have any effect on performance anyway?
    */
  virtual void flatten() = 0;

  /** Get the style's property map.
    */
  virtual const IWORKPropertyMap &getPropertyMap() const = 0;
};

typedef boost::shared_ptr<KEYStyle> KEYStylePtr_t;
typedef boost::unordered_map<ID_t, KEYStylePtr_t> KEYStyleMap_t;

}

#endif // IWORKSTYLE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
