/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY1STRINGCONVERTER_H_INCLUDED
#define KEY1STRINGCONVERTER_H_INCLUDED

#include <boost/optional.hpp>

#include <glm/glm.hpp>

#include "IWORKTypes.h"

namespace libetonyek
{
template<typename T>
struct KEY1StringConverter
{
  static boost::optional<T> convert(const char *value);
};

#define KEY1_DECLARE_STRING_CONVERTER(type) \
template<> \
struct KEY1StringConverter<type> \
{ \
static boost::optional<type> convert (const char *value); \
}

KEY1_DECLARE_STRING_CONVERTER(glm::dmat3);
KEY1_DECLARE_STRING_CONVERTER(IWORKColor);
KEY1_DECLARE_STRING_CONVERTER(IWORKPosition);
KEY1_DECLARE_STRING_CONVERTER(IWORKSize);
}

#endif // KEY1STRINGCONVERTER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
