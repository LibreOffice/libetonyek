/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <boost/lexical_cast.hpp>

#include "libetonyek_xml.h"
#include "IWORKToken.h"
#include "IWORKTypes.h"
#include "IWORKXMLReader.h"
#include "KEY2ParserUtils.h"
#include "KEY2Token.h"
#include "KEYTypes.h"

using boost::lexical_cast;

namespace libetonyek
{

bool KEY2ParserUtils::bool_cast(const char *value)
{
  IWORKTokenizer tok;
  switch (tok(value))
  {
  case IWORKToken::_1 :
  case IWORKToken::true_ :
    return true;
  case IWORKToken::_0 :
  case IWORKToken::false_ :
  default :
    return false;
  }

  return false;
}

double KEY2ParserUtils::double_cast(const char *value)
{
  return lexical_cast<double, const char *>(value);
}

int KEY2ParserUtils::int_cast(const char *value)
{
  return lexical_cast<int, const char *>(value);
}

double KEY2ParserUtils::deg2rad(double value)
{
  // normalize range
  while (360 <= value)
    value -= 360;
  while (0 > value)
    value += 360;

  // convert
  return etonyek_pi / 180 * value;
}

KEY2ParserUtils::KEY2ParserUtils()
{
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
