/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKNUMERICPROPERTYBASE_H_INCLUDED
#define IWORKNUMERICPROPERTYBASE_H_INCLUDED

#include "IWORKNumberElement.h"
#include "IWORKPropertyContext.h"

namespace libetonyek
{

template<typename ValueT, class PropertyT>
class IWORKNumericPropertyBase : public IWORKPropertyContext<PropertyT, IWORKNumberElement<ValueT>, IWORKToken::NS_URI_SF | IWORKToken::number>
{
  typedef IWORKPropertyContext<PropertyT, IWORKNumberElement<ValueT>, IWORKToken::NS_URI_SF | IWORKToken::number> Parent_t;

public:
  IWORKNumericPropertyBase(IWORKXMLParserState &state, IWORKPropertyMap &propMap);
};

template<typename ValueT, class PropertyT>
IWORKNumericPropertyBase<ValueT, PropertyT>::IWORKNumericPropertyBase(IWORKXMLParserState &state, IWORKPropertyMap &propMap)
  : Parent_t(state, propMap)
{
}

}

#endif // IWORKNUMERICPROPERTYBASE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
