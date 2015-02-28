/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYSTYLECONTEXT_H_INCLUDED
#define KEYSTYLECONTEXT_H_INCLUDED

#include <deque>

#include <boost/any.hpp>

#include "IWORKStyle.h"

namespace libetonyek
{

/** Representation of a dynamic inheritance of styles.
  *
  * A static inheritance of styles is represented by the "parent"
  * relation of styles. A dynamic inheritance is defined by the way
  * styles are applied on elements. One or more styles may be active at
  * any given moment (e.g., a character style of a text span and a
  * paragraph style of the paragraph) and a property can be inherited
  * from the enclosing style.
  *
  * The property getters in specific implementations of @c IWORKStyle
  * should take a pointer to a @c IWORKStyleContext.
  */
class IWORKStyleContext
{
  /** The internal type of style stack.
    *
    * @c std::deque is used instead of @c std::stack because @c
    * std::stack does not allow iteration through the elements.
    */
  typedef std::deque<IWORKStylePtr_t> Stack_t;

public:
  /** Construct an empty context.
    */
  IWORKStyleContext();

  /** Push a style onto the active styles stack.
    *
    * The previous top style becomes a dynamic parent @c style.
    */
  void push();

  /** Pop a style from the active styles stack.
    */
  void pop();

  void set(const IWORKStylePtr_t &style);

  /** Find the current value of property @c property.
    *
    * The search traverses the stack of active styles from top to
    * bottom. The search ends if @c property is found or if bottom is
    * reached.
    *
    * @arg[in] property the property to look for
    * @arg[in] lookInParent should the parent styles of the active
    *   styles be included in the search too?
    * @returns the found value or empty value
    *
    * @seealso IWORKPropertyMap::get
    */
  boost::any find(const std::string &property, bool lookInParent = false) const;

private:
  Stack_t m_stack;
  bool m_pending;
};

}

#endif // KEYSTYLECONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
