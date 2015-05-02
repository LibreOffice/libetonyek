/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKPROPERTYINFO_H_INCLUDED
#define IWORKPROPERTYINFO_H_INCLUDED

#include <string>

namespace libetonyek
{

typedef std::string IWORKPropertyID_t;

template<typename Name>
struct IWORKPropertyInfo
{
  // typedef Foo ValueType;
  // static const IWORKPropertyID_t id;
};

#define IWORK_DECLARE_PROPERTY(name, type) \
namespace property { \
  struct name {}; \
} \
\
template<> \
struct IWORKPropertyInfo<property::name> \
{ \
  typedef type ValueType; \
  static const IWORKPropertyID_t id; \
}

#define IWORK_IMPLEMENT_PROPERTY(name) \
const IWORKPropertyID_t IWORKPropertyInfo<property::name>::id = #name

}

#endif // IWORKPROPERTYINFO_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
