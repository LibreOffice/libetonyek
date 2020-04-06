/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKDIRECTCOLLECTOR_H_INCLUDED
#define IWORKDIRECTCOLLECTOR_H_INCLUDED

#include <deque>

#include "IWORKXMLContext.h"

namespace libetonyek
{

template<typename Type, class Container = std::deque<Type> >
class IWORKDirectCollector
{
public:
  explicit IWORKDirectCollector(Container &collection)
    : m_collection(collection)
  {
  }

  template<class Context, class State>
  IWORKXMLContextPtr_t makeContext(State &state) const
  {
    return std::make_shared<Context>(state, m_collection);
  }

  bool pending() const
  {
    return false;
  }

  void push()
  {
  }

private:
  Container &m_collection;
};

}

#endif // IWORKDIRECTCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
