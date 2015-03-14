/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "libetonyek_xml.h"

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

#include "IWORKToken.h"

using boost::lexical_cast;
using boost::optional;

using std::string;

extern "C" int readFromStream(void *context, char *buffer, int len)
{
  try
  {
    librevenge::RVNGInputStream *const input = reinterpret_cast<librevenge::RVNGInputStream *>(context);

    unsigned long bytesRead = 0;
    const unsigned char *const bytes = input->read(len, bytesRead);

    std::memcpy(buffer, bytes, static_cast<int>(bytesRead));
    return static_cast<int>(bytesRead);
  }
  catch (...)
  {
  }

  return -1;
}

extern "C" int closeStream(void * /* context */)
{
  return 0;
}

namespace libetonyek
{

bool bool_cast(const char *value)
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

double double_cast(const char *value)
{
  return lexical_cast<double, const char *>(value);
}

int int_cast(const char *value)
{
  return lexical_cast<int, const char *>(value);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
