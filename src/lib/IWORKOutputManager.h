/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKOUTPUTMANAGER_H_INCLUDED
#define IWORKOUTPUTMANAGER_H_INCLUDED

#include <deque>
#include <stack>

#include "IWORKOutputElements.h"
#include "IWORKTypes.h"

namespace libetonyek
{

/** A manager for output elements.
  *
  * It keeps a stack of currently opened output elements and a list of saved
  * output elements.
  *
  * There is always at least one output element on the stack.
  */
class IWORKOutputManager
{
  // disable copying
  IWORKOutputManager(const IWORKOutputManager &);
  IWORKOutputManager &operator=(const IWORKOutputManager &);

  typedef std::deque<IWORKOutputElements> OutputList_t;
  typedef std::stack<IWORKOutputElements> OutputStack_t;

public:
  IWORKOutputManager();
  ~IWORKOutputManager();

  /** Push a new output element onto the stack.
    */
  void push();

  /** Pop a output element from the stack
    */
  void pop();

  /** Save the current output element.
    *
    * It remains on the stack.
    *
    * @return The ID of the saved output element.
    */
  IWORKOutputID_t save();

  /** Get output element with ID @c id.
    *
    * @arg[in] id ID of the output element to get.
    */
  IWORKOutputElements &get(IWORKOutputID_t id);
  /** Get output element with ID @c id.
    *
    * @arg[in] id ID of the output element to get.
    */
  const IWORKOutputElements &get(IWORKOutputID_t id) const;

  /** Get the current output element.
    */
  IWORKOutputElements &getCurrent();
  /** Get the current output element.
    */
  const IWORKOutputElements &getCurrent() const;

private:
  OutputStack_t m_active;
  OutputList_t m_saved;
};

}

#endif // IWORKOUTPUTMANAGER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
