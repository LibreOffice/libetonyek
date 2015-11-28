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

#include <deque>

#include "IWORKXMLContextBase.h"

namespace libetonyek
{

template<typename Type, class NestedParser, unsigned Id>
class IWORKContainerContext : public IWORKXMLElementContextBase
{
public:
  IWORKContainerContext(IWORKXMLParserState &state, std::deque<Type> &elements)
    : IWORKXMLElementContextBase(state)
    , m_elements(elements)
  {
  }

private:
  virtual IWORKXMLContextPtr_t element(const int name)
  {
    if (name == Id)
      return makeContext<NestedParser>(getState(), m_elements);
    return IWORKXMLContextPtr_t();
  }

private:
  std::deque<Type> &m_elements;
};

}

#endif // IWORKCONTAINERCONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
