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

optional<double> IWORKNumberConverter<double>::convert(const char *const value)
{
  return try_double_cast(value);
}

optional<int> IWORKNumberConverter<int>::convert(const char *const value)
{
  return try_int_cast(value);
}

optional<IWORKAlignment> IWORKNumberConverter<IWORKAlignment>::convert(const char *const value)
{
  const optional<int> alignment(try_int_cast(value));
  if (alignment)
  {
    switch (get(alignment))
    {
    case 0 :
      return IWORK_ALIGNMENT_LEFT;
    case 1 :
      return IWORK_ALIGNMENT_RIGHT;
    case 2 :
      return IWORK_ALIGNMENT_CENTER;
    case 3 :
      return IWORK_ALIGNMENT_JUSTIFY;
    case 4 : // TODO: what is this?
      break;
    }
  }

  return none;
}

optional<IWORKBaseline> IWORKNumberConverter<IWORKBaseline>::convert(const char *const value)
{
  const optional<int> superscript(try_int_cast(value));
  if (superscript)
  {
    switch (get(superscript))
    {
    case 1 :
      return IWORK_BASELINE_SUPER;
    case 2 :
      return IWORK_BASELINE_SUB;
    }
  }

  return none;
}

optional<IWORKBorderType> IWORKNumberConverter<IWORKBorderType>::convert(const char *const value)
{
  switch (int_cast(value))
  {
  case 1 :
    return IWORK_BORDER_TYPE_TOP;
  case 2 :
    return IWORK_BORDER_TYPE_BOTTOM;
  case 3 :
    return IWORK_BORDER_TYPE_TOP_AND_BOTTOM;
  case 4 :
    return IWORK_BORDER_TYPE_ALL;
  }
  return optional<IWORKBorderType>();
}

optional<IWORKCapitalization> IWORKNumberConverter<IWORKCapitalization>::convert(const char *const value)
{
  const optional<int> capitalization(try_int_cast(value));
  if (capitalization)
  {
    switch (get(capitalization))
    {
    case 0 :
      return IWORK_CAPITALIZATION_NONE;
    case 1 :
      return IWORK_CAPITALIZATION_ALL_CAPS;
    case 2 :
      return IWORK_CAPITALIZATION_SMALL_CAPS;
    case 3 :
      return IWORK_CAPITALIZATION_TITLE;
    }
  }

  return none;
}

optional<IWORKCellNumberType> IWORKNumberConverter<IWORKCellNumberType>::convert(const char *const value)
{
  const optional<int> numberFormat(try_int_cast(value));
  if (numberFormat)
  {
    switch (get(numberFormat))
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

  return none;
}


}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
