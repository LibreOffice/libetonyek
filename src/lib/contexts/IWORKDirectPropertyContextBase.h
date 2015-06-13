/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKDIRECTPROPERTYCONTEXTBASE_H_INCLUDED
#define IWORKDIRECTPROPERTYCONTEXTBASE_H_INCLUDED

#include "IWORKPropertyContextBase.h"
#include "IWORKPropertyInfo.h"

namespace libetonyek
{

template<class ContextT, class PropertyT>
class IWORKDirectPropertyContextBase : public IWORKPropertyContextBase
{
public:
  IWORKDirectPropertyContextBase(IWORKXMLParserState &state, IWORKPropertyMap &propMap, int propId);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  const int m_propId;
  typename IWORKPropertyInfo<PropertyT>::ValueType m_value;
};

template<class ContextT, class PropertyT>
IWORKDirectPropertyContextBase<ContextT, PropertyT>::IWORKDirectPropertyContextBase(IWORKXMLParserState &state, IWORKPropertyMap &propMap, const int propId)
  : IWORKPropertyContextBase(state, propMap)
  , m_propId(propId)
  , m_value()
{
}

template<class ContextT, class PropertyT>
IWORKXMLContextPtr_t IWORKDirectPropertyContextBase<ContextT, PropertyT>::element(const int name)
{
  m_default = false;

  if (m_propId == name)
    return makeContext<ContextT>(getState(), m_value);

  return IWORKXMLContextPtr_t();
}

template<class ContextT, class PropertyT>
void IWORKDirectPropertyContextBase<ContextT, PropertyT>::endOfElement()
{
  if (bool(m_value))
    m_propMap.put<PropertyT>(m_value);
  else if (m_default)
    m_propMap.clear<PropertyT>();
}

}

#endif // IWORKDIRECTPROPERTYCONTEXTBASE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
