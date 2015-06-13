/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKNUMBERELEMENT_H_INCLUDED
#define IWORKNUMBERELEMENT_H_INCLUDED

#include "IWORKNumberConverter.h"
#include "IWORKToken.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

template<typename T>
class IWORKNumberElement : public IWORKXMLEmptyContextBase
{
public:
  IWORKNumberElement(IWORKXMLParserState &state, boost::optional<T> &value);

private:
  virtual void attribute(int name, const char *value);

private:
  boost::optional<T> &m_value;
};

template<typename T>
IWORKNumberElement<T>::IWORKNumberElement(IWORKXMLParserState &state, boost::optional<T> &value)
  : IWORKXMLEmptyContextBase(state)
  , m_value(value)
{
}

template<typename T>
void IWORKNumberElement<T>::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SFA | IWORKToken::number :
    m_value = IWORKNumberConverter<T>::convert(value);
    break;
  }
}

}

#endif // IWORKNUMBERELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
