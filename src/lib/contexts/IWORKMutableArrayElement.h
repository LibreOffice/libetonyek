/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKMUTABLEARRAYELEMENT_H_INCLUDED
#define IWORKMUTABLEARRAYELEMENT_H_INCLUDED

#include "IWORKContainerContext.h"

namespace libetonyek
{

template<typename Type, class NestedParser, template<typename T, class C> class Collector, unsigned Id, unsigned RefId = 0>
class IWORKMutableArrayElement : public IWORKContainerContext<Type, NestedParser, Collector, Id, RefId>
{
  typedef IWORKContainerContext<Type, NestedParser, Collector, Id, RefId> Super_t;
  typedef boost::unordered_map<ID_t, Type> ContextDict_t;
  typedef boost::unordered_map<ID_t, std::deque<Type> > ArrayDict_t;

public:
  IWORKMutableArrayElement(IWORKXMLParserState &state, ArrayDict_t &arrayDict, std::deque<Type> &elements)
    : Super_t(state, elements)
    , m_arrayDict(arrayDict)
    , m_elements(elements)
  {
  }

  IWORKMutableArrayElement(IWORKXMLParserState &state, ArrayDict_t &arrayDict, ContextDict_t &dict, std::deque<Type> &elements)
    : Super_t(state, dict, elements)
    , m_arrayDict(arrayDict)
    , m_elements(elements)
  {
  }

private:
  virtual IWORKXMLContextPtr_t element(const int name)
  {
    return Super_t::element(name);
  }

  virtual void endOfElement()
  {
    Super_t::endOfElement();
    if (this->getId())
      m_arrayDict[get(this->getId())] = m_elements;
  }

private:
  ArrayDict_t &m_arrayDict;
  std::deque<Type> &m_elements;
};

}

#endif // IWORKMUTABLEARRAYELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
