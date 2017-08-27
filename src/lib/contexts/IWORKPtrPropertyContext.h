/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKPTRPROPERTYCONTEXT_H_INCLUDED
#define IWORKPTRPROPERTYCONTEXT_H_INCLUDED

#include "IWORKPropertyContextBase.h"
#include "IWORKPropertyInfo.h"

namespace libetonyek
{

template<typename Property, typename Context, int TokenId>
class IWORKPtrPropertyContext : public IWORKPropertyContextBase
{
public:
  IWORKPtrPropertyContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  typename IWORKPropertyInfo<Property>::ValueType m_value;
};

template<typename Property, typename Context, int TokenId>
IWORKPtrPropertyContext<Property, Context, TokenId>::IWORKPtrPropertyContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : IWORKPropertyContextBase(state, propMap)
  , m_value()
{
}

template<typename Property, typename Context, int TokenId>
IWORKXMLContextPtr_t IWORKPtrPropertyContext<Property, Context, TokenId>::element(const int name)
{
  m_default = false;
  if (TokenId == name)
    return makeContext<Context>(getState(), m_value);
  return IWORKXMLContextPtr_t();
}

template<typename Property, typename Context, int TokenId>
void IWORKPtrPropertyContext<Property, Context, TokenId>::endOfElement()
{
  if (bool(m_value))
    m_propMap.put<Property>(m_value);
  else if (m_default)
    m_propMap.clear<Property>();
}

}

#endif // IWORKPTRPROPERTYCONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
