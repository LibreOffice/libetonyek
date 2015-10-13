/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKNUMBERCONVERTER_H_INCLUDED
#define IWORKNUMBERCONVERTER_H_INCLUDED

#include <boost/optional.hpp>

#include "IWORKEnum.h"

namespace libetonyek
{

#define IWORK_DECLARE_NUMBER_CONVERTER(type) \
template<> \
struct IWORKNumberConverter<type> \
{ \
static boost::optional<type> convert (const char *value); \
static boost::optional<type> convert (unsigned value); \
}

template<typename T>
struct IWORKNumberConverter
{
  static boost::optional<T> convert(const char *value);
  static boost::optional<T> convert(unsigned value);
};

IWORK_DECLARE_NUMBER_CONVERTER(bool);
IWORK_DECLARE_NUMBER_CONVERTER(double);
IWORK_DECLARE_NUMBER_CONVERTER(int);
IWORK_DECLARE_NUMBER_CONVERTER(IWORKAlignment);
IWORK_DECLARE_NUMBER_CONVERTER(IWORKBaseline);
IWORK_DECLARE_NUMBER_CONVERTER(IWORKBorderType);
IWORK_DECLARE_NUMBER_CONVERTER(IWORKCapitalization);
IWORK_DECLARE_NUMBER_CONVERTER(IWORKCellNumberType);

}

#endif // IWORKNUMBERCONVERTER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
