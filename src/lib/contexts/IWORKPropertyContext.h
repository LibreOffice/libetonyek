/* -*- Mode: Context++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKPROPERTYCONTEXT_H_INCLUDED
#define IWORKPROPERTYCONTEXT_H_INCLUDED

#include <boost/optional.hpp>

#include "IWORKPropertyContextBase.h"
#include "IWORKPropertyInfo.h"

namespace libetonyek
{

template<typename Property, typename Context, int TokenId>
class IWORKPropertyContext : public IWORKPropertyContextBase
{
public:
  IWORKPropertyContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  boost::optional<typename IWORKPropertyInfo<Property>::ValueType> m_value;
};

template<typename Property, typename Context, int TokenId>
IWORKPropertyContext<Property, Context, TokenId>::IWORKPropertyContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : IWORKPropertyContextBase(state, propMap)
  , m_value()
{
}

template<typename Property, typename Context, int TokenId>
IWORKXMLContextPtr_t IWORKPropertyContext<Property, Context, TokenId>::element(const int name)
{
  m_default = false;
  if (TokenId == name)
    return makeContext<Context>(getState(), m_value);
  return IWORKXMLContextPtr_t();
}

template<typename Property, typename Context, int TokenId>
void IWORKPropertyContext<Property, Context, TokenId>::endOfElement()
{
  if (bool(m_value))
    m_propMap.put<Property>(get(m_value));
  else if (m_default)
    m_propMap.clear<Property>();
}

}

#endif // IWORKPROPERTYCONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
