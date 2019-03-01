/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <deque>

#include "IWORKFormatElement.h"

#include "IWORKContainerContext.h"
#include "IWORKDictionary.h"
#include "IWORKNumberConverter.h"
#include "IWORKProperties.h"
#include "IWORKRefContext.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

#include "libetonyek_utils.h"
#include "libetonyek_xml.h"

namespace libetonyek
{

using boost::optional;

IWORKDateTimeFormatElement::IWORKDateTimeFormatElement(IWORKXMLParserState &state, optional<IWORKDateTimeFormat> &value)
  : IWORKXMLEmptyContextBase(state)
  , m_value(value)
{
  m_value=IWORKDateTimeFormat();
}

void IWORKDateTimeFormatElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::fmt:
    get(m_value).m_format = value;
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::ID :
    IWORKXMLEmptyContextBase::attribute(name, value);
    break;
  default:
    ETONYEK_DEBUG_MSG(("DateTimeFormatElement::attribute[IWORKPropertyMapElement.cpp]: find unknown attribute\n"));
  }
}

void IWORKDateTimeFormatElement::endOfElement()
{
  if (getId())
    getState().getDictionary().m_dateTimeFormats.insert(IWORKDateTimeFormatMap_t::value_type(get(getId()),get(m_value)));
}

/////////////////////////////
IWORKDurationFormatElement::IWORKDurationFormatElement(IWORKXMLParserState &state, optional<IWORKDurationFormat> &value)
  : IWORKXMLEmptyContextBase(state)
  , m_value(value)
{
  m_value=IWORKDurationFormat();
}

void IWORKDurationFormatElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::fmt:
    get(m_value).m_format = value;
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::ID :
    IWORKXMLEmptyContextBase::attribute(name, value);
    break;
  default:
    ETONYEK_DEBUG_MSG(("DurationFormatElement::attribute[IWORKPropertyMapElement.cpp]: find unknown attribute\n"));
  }
}

void IWORKDurationFormatElement::endOfElement()
{
  if (getId())
    getState().getDictionary().m_durationFormats.insert(IWORKDurationFormatMap_t::value_type(get(getId()),get(m_value)));
}

/////////////////////////////
IWORKNumberFormatElement::IWORKNumberFormatElement(IWORKXMLParserState &state, optional<IWORKNumberFormat> &value)
  : IWORKXMLEmptyContextBase(state)
  , m_value(value)
{
  m_value=IWORKNumberFormat();
}

void IWORKNumberFormatElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::format_base :
    get(m_value).m_base = int_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::format_base_places :
    get(m_value).m_basePlaces = int_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::format_base_use_minus_sign :
    get(m_value).m_baseUseMinusSign = bool_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::format_currency_code :
    get(m_value).m_currencyCode = value;
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::format_decimal_places :
    get(m_value).m_decimalPlaces = int_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::format_fraction_accuracy :
    get(m_value).m_fractionAccuracy = int_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::format_negative_style :
    get(m_value).m_negativeStyle = int_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::format_show_thousands_separator :
    get(m_value).m_thousandsSeparator = bool_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::format_string :
    get(m_value).m_string = value;
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::format_type :
    get(m_value).m_type = get(IWORKNumberConverter<IWORKCellNumberType>::convert(value));
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::format_use_accounting_style :
    get(m_value).m_accountingStyle = bool_cast(value);
    break;
  case IWORKToken::NS_URI_SFA | IWORKToken::ID :
    IWORKXMLEmptyContextBase::attribute(name, value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::custom :
  case IWORKToken::NS_URI_SF | IWORKToken::format_name :
    break;
  default:
    ETONYEK_DEBUG_MSG(("IWORKNumberFormatElement::attribute[IWORKFormatElement.cpp]: find unknown attribute\n"));
  }
}

void IWORKNumberFormatElement::endOfElement()
{
  if (getId())
    getState().getDictionary().m_numberFormats.insert(IWORKNumberFormatMap_t::value_type(get(getId()),get(m_value)));
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
