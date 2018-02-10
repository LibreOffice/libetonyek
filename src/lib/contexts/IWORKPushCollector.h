/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKPUSHCOLLECTOR_H_INCLUDED
#define IWORKPUSHCOLLECTOR_H_INCLUDED

#include <cassert>
#include <deque>

#include <boost/optional.hpp>

#include "IWORKXMLContext.h"

namespace libetonyek
{

template<typename Type, class Container = std::deque<Type> >
class IWORKPushCollector
{
public:
  IWORKPushCollector(Container &collection)
    : m_collection(collection)
    , m_value()
  {
  }

  template<class Context, class State>
  IWORKXMLContextPtr_t makeContext(State &state)
  {
    return libetonyek::makeContext<Context>(state, m_value);
  }

  bool pending() const
  {
    return bool(m_value);
  }

  void push()
  {
    assert(m_value);
    m_collection.push_back(get(m_value));
    m_value.reset();
  }

private:
  Container &m_collection;
  boost::optional<Type> m_value;
};

}

#endif // IWORKPUSHCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
