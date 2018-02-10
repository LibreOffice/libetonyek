/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKCONTAINERCONTEXT_H_INCLUDED
#define IWORKCONTAINERCONTEXT_H_INCLUDED

#include <cassert>
#include <deque>

#include <boost/optional.hpp>

#include "IWORKTypes_fwd.h"
#include "IWORKRefContext.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

template<typename Type, class NestedParser, template<typename T, class C> class Collector, unsigned Id, unsigned RefId = 0>
class IWORKContainerContext : public IWORKXMLElementContextBase
{
  typedef std::unordered_map<ID_t, Type> Dict_t;

public:
  IWORKContainerContext(IWORKXMLParserState &state, std::deque<Type> &elements)
    : IWORKXMLElementContextBase(state)
    , m_dict(nullptr)
    , m_elements(elements)
    , m_collector(elements)
  {
  }

  IWORKContainerContext(IWORKXMLParserState &state, Dict_t &dict, std::deque<Type> &elements)
    : IWORKXMLElementContextBase(state)
    , m_dict(&dict)
    , m_elements(elements)
    , m_collector(elements)
  {
  }

protected:
  IWORKXMLContextPtr_t element(const int name) override
  {
    if (m_ref && m_dict)
      handleRef();
    else if (m_collector.pending())
      m_collector.push();

    if (name == Id)
      return m_collector.template makeContext<NestedParser>(getState());
    else if ((RefId != 0) && (name == RefId))
      return makeContext<IWORKRefContext>(getState(), m_ref);
    return IWORKXMLContextPtr_t();
  }

  void endOfElement() override
  {
    if (m_ref && m_dict)
      handleRef();
    else if (m_collector.pending())
      m_collector.push();
  }

private:
  void handleRef()
  {
    assert(m_dict);

    const typename Dict_t::const_iterator it = m_dict->find(get(m_ref));
    if (it == m_dict->end())
    {
      ETONYEK_DEBUG_MSG(("IWORKContainerContext::handleRef: unknown ref \"%s\"\n", get(m_ref).c_str()));
      m_elements.push_back(Type());
    }
    else
      m_elements.push_back(it->second);
    m_ref.reset();
  }

private:
  Dict_t *const m_dict;
  boost::optional<ID_t> m_ref;
  std::deque<Type> &m_elements;
  Collector<Type, std::deque<Type> > m_collector;
};

}

#endif // IWORKCONTAINERCONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
