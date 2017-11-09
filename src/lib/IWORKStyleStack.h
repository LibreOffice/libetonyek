/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKSTYLESTACK_H_INCLUDED
#define IWORKSTYLESTACK_H_INCLUDED

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
  * should take a pointer to a @c IWORKStyleStack.
  */
class IWORKStyleStack
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
  IWORKStyleStack();

  ~IWORKStyleStack();

  /** Push a style onto the active styles stack.
    *
    * The previous top style becomes a dynamic parent @c style.
    */
  void push();

  /** Push a style onto the active styles stack.
    *
    * The previous top style becomes a dynamic parent @c style.
    */
  void push(const IWORKStylePtr_t &style);

  /** Pop a style from the active styles stack.
    */
  void pop();

  void set(const IWORKStylePtr_t &style);

  template<class Property>
  bool has(const bool lookInParent = true) const
  {
    for (auto it = m_stack.begin(); m_stack.end() != it; ++it)
    {
      if (*it)
      {
        if ((*it)->getPropertyMap().has<Property>(lookInParent))
          return true;
        else if ((*it)->getPropertyMap().clears<Property>(lookInParent))
          break;
      }
    }
    return false;
  }

  template<class Property>
  const typename IWORKPropertyInfo<Property>::ValueType &get(const bool lookInParent = true) const
  {
    for (auto it = m_stack.begin(); m_stack.end() != it; ++it)
    {
      if (*it)
      {
        if ((*it)->getPropertyMap().has<Property>(lookInParent))
          return (*it)->getPropertyMap().get<Property>(lookInParent);
        else if ((*it)->getPropertyMap().clears<Property>(lookInParent))
          break;
      }
    }
    throw IWORKPropertyMap::NotFoundException();
  }

private:
  Stack_t m_stack;
};

}

#endif // IWORKSTYLESTACK_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
