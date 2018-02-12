/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKVALUECONTEXT_H_INCLUDED
#define IWORKVALUECONTEXT_H_INCLUDED

#include <boost/optional.hpp>

#include "IWORKXMLContextBase.h"

namespace libetonyek
{

template<typename Type, class NestedParser, unsigned Id, unsigned Id2=0>
class IWORKValueContext : public IWORKXMLElementContextBase
{
public:
  IWORKValueContext(IWORKXMLParserState &state, boost::optional<Type> &value)
    : IWORKXMLElementContextBase(state)
    , m_value()
    , m_optional(value)
    , m_direct(nullptr)
    , m_isSet(nullptr)
  {
  }

  IWORKValueContext(IWORKXMLParserState &state, Type &value, bool *isSet = nullptr)
    : IWORKXMLElementContextBase(state)
    , m_value()
    , m_optional(m_value)
    , m_direct(&value)
    , m_isSet(isSet)
  {
  }

protected:
  void set(const Type &value)
  {
    m_value = value;
  }

  IWORKXMLContextPtr_t element(const int name) override
  {
    if (name == Id || (Id2 && name == Id2))
      return makeContext<NestedParser>(getState(), m_value);
    ETONYEK_DEBUG_MSG(("IWORKXMLContextPtr_t::element: found unexpected element %d\n", name));
    return IWORKXMLContextPtr_t();
  }

  void endOfElement() override
  {
    if (m_value)
    {
      if (m_direct)
      {
        *m_direct = get(m_value);
        if (m_isSet)
          *m_isSet |= true;
      }
      else
      {
        m_optional = m_value;
      }
    }
  }

private:
  IWORKValueContext(const IWORKValueContext &);
  IWORKValueContext &operator=(const IWORKValueContext &);

  boost::optional<Type> m_value;
  boost::optional<Type> &m_optional;
  Type *const m_direct;
  bool *const m_isSet;
};

}

#endif // IWORKVALUECONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
