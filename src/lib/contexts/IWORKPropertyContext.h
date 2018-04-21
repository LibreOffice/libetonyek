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
#include "IWORKToken.h"

namespace libetonyek
{

template<typename Property, typename Context, int TokenId, int TokenId2=0>
class IWORKPropertyContext : public IWORKPropertyContextBase
{
public:
  IWORKPropertyContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  boost::optional<typename IWORKPropertyInfo<Property>::ValueType> m_value;
};

template<typename Property, typename Context, int TokenId, int TokenId2>
IWORKPropertyContext<Property, Context, TokenId, TokenId2>::IWORKPropertyContext(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : IWORKPropertyContextBase(state, propMap)
  , m_value()
{
}

template<typename Property, typename Context, int TokenId, int TokenId2>
IWORKXMLContextPtr_t IWORKPropertyContext<Property, Context, TokenId, TokenId2>::element(const int name)
{
  m_default = false;
  if (TokenId == name || (TokenId2 != 0 && TokenId2 == name))
    return std::make_shared<Context>(getState(), m_value);
  else if (name != (IWORKToken::NS_URI_SF | IWORKToken::null))
  {
    ETONYEK_DEBUG_MSG(("IWORKPropertyContext<...>::element: found unexpected element %d\n", name));
  }
  return IWORKXMLContextPtr_t();
}

template<typename Property, typename Context, int TokenId, int TokenId2>
void IWORKPropertyContext<Property, Context, TokenId, TokenId2>::endOfElement()
{
  if (bool(m_value))
    m_propMap.put<Property>(get(m_value));
  else if (m_default)
    m_propMap.clear<Property>();
}

}

#endif // IWORKPROPERTYCONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
