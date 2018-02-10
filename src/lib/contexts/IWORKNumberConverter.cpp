/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKNumberConverter.h"

#include "libetonyek_xml.h"

namespace libetonyek
{

using boost::none;
using boost::optional;

optional<bool> IWORKNumberConverter<bool>::convert(const char *const value)
{
  return try_bool_cast(value);
}

optional<bool> IWORKNumberConverter<bool>::convert(const unsigned value)
{
  return bool(value);
}

optional<double> IWORKNumberConverter<double>::convert(const char *const value)
{
  return try_double_cast(value);
}

optional<double> IWORKNumberConverter<double>::convert(const unsigned value)
{
  return double(value);
}

optional<int> IWORKNumberConverter<int>::convert(const char *const value)
{
  return try_int_cast(value);
}

optional<int> IWORKNumberConverter<int>::convert(const unsigned value)
{
  return int(value);
}

optional<IWORKAlignment> IWORKNumberConverter<IWORKAlignment>::convert(const char *const value)
{
  const optional<int> alignment(try_int_cast(value));
  if (alignment)
    return convert(unsigned(get(alignment)));
  return none;
}

optional<IWORKAlignment> IWORKNumberConverter<IWORKAlignment>::convert(const unsigned value)
{
  switch (value)
  {
  case 0 :
    return IWORK_ALIGNMENT_LEFT;
  case 1 :
    return IWORK_ALIGNMENT_RIGHT;
  case 2 :
    return IWORK_ALIGNMENT_CENTER;
  case 3 :
    return IWORK_ALIGNMENT_JUSTIFY;
  case 4 :
    return IWORK_ALIGNMENT_AUTOMATIC;
  default:
    ETONYEK_DEBUG_MSG(("IWORKNumberConverter<IWORKAlignment>::convert: unknown value %d\n", int(value)));
    break;
  }
  return none;
}

optional<IWORKBaseline> IWORKNumberConverter<IWORKBaseline>::convert(const char *const value)
{
  const optional<int> superscript(try_int_cast(value));
  if (superscript)
    return convert(unsigned(get(superscript)));
  return none;
}

optional<IWORKBaseline> IWORKNumberConverter<IWORKBaseline>::convert(const unsigned value)
{
  switch (value)
  {
  case 0:
    return none;
  case 1 :
    return IWORK_BASELINE_SUPER;
  case 2 :
    return IWORK_BASELINE_SUB;
  default:
    ETONYEK_DEBUG_MSG(("IWORKNumberConverter<IWORKBaseline>::convert: unknown value %d\n", int(value)));
  }
  return none;
}

optional<IWORKBorderType> IWORKNumberConverter<IWORKBorderType>::convert(const char *const value)
{
  const optional<int> border(try_int_cast(value));
  if (border)
    return convert(unsigned(get(border)));
  return none;
}

optional<IWORKBorderType> IWORKNumberConverter<IWORKBorderType>::convert(const unsigned value)
{
  switch (value)
  {
  case 0:
    return none;
  case 1 :
    return IWORK_BORDER_TYPE_TOP;
  case 2 :
    return IWORK_BORDER_TYPE_BOTTOM;
  case 3 :
    return IWORK_BORDER_TYPE_TOP_AND_BOTTOM;
  case 4 :
    return IWORK_BORDER_TYPE_ALL;
  default:
    ETONYEK_DEBUG_MSG(("IWORKNumberConverter<IWORKBorderType>::convert: unknown value %d\n", int(value)));
  }
  return none;
}

optional<IWORKCapitalization> IWORKNumberConverter<IWORKCapitalization>::convert(const char *const value)
{
  const optional<int> capitalization(try_int_cast(value));
  if (capitalization)
    return convert(unsigned(get(capitalization)));
  return none;
}

optional<IWORKCapitalization> IWORKNumberConverter<IWORKCapitalization>::convert(const unsigned value)
{
  switch (value)
  {
  case 0 :
    return IWORK_CAPITALIZATION_NONE;
  case 1 :
    return IWORK_CAPITALIZATION_ALL_CAPS;
  case 2 :
    return IWORK_CAPITALIZATION_SMALL_CAPS;
  case 3 :
    return IWORK_CAPITALIZATION_TITLE;
  default:
    ETONYEK_DEBUG_MSG(("IWORKNumberConverter<IWORKCapitalization>::convert: unknown value %d\n", int(value)));
  }
  return none;
}

optional<IWORKCellNumberType> IWORKNumberConverter<IWORKCellNumberType>::convert(const char *const value)
{
  const optional<int> numberFormat(try_int_cast(value));
  if (numberFormat)
    return convert(unsigned(get(numberFormat)));
  return none;
}

optional<IWORKCellNumberType> IWORKNumberConverter<IWORKCellNumberType>::convert(const unsigned value)
{
  switch (value)
  {
  case 1 :
    return IWORK_CELL_NUMBER_TYPE_CURRENCY;
  case 2 :
    return IWORK_CELL_NUMBER_TYPE_PERCENTAGE;
  case 3 :
    return IWORK_CELL_NUMBER_TYPE_SCIENTIFIC;
  case 4 :
  case 5 :
  default :
    return IWORK_CELL_NUMBER_TYPE_DOUBLE;
  }
}

optional<IWORKStrokeType> IWORKNumberConverter<IWORKStrokeType>::convert(const char *const value)
{
  const optional<int> type(try_int_cast(value));
  if (type)
    return convert(unsigned(get(type)));
  return none;
}

optional<IWORKStrokeType> IWORKNumberConverter<IWORKStrokeType>::convert(const unsigned value)
{
  switch (value)
  {
  case 0 :
    return IWORK_STROKE_TYPE_DASHED;
  case 1 :
    return IWORK_STROKE_TYPE_SOLID;
  case 2 :
    return IWORK_STROKE_TYPE_AUTO;
  default:
    ETONYEK_DEBUG_MSG(("IWORKNumberConverter<IWORKStrokeType>::convert: unknown value %d\n", int(value)));
  }
  return none;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
