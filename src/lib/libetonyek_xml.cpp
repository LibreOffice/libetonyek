/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "libetonyek_xml.h"

#include <cassert>

#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include <boost/optional.hpp>

#include "libetonyek_utils.h"
#include "IWORKToken.h"
#include "IWORKTokenizer.h"

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
  const IWORKTokenizer &tok(IWORKToken::getTokenizer());
  switch (tok.getId(value))
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

const char *char_cast(const char *const c)
{
  return c;
}

const char *char_cast(const signed char *const c)
{
  return reinterpret_cast<const char *>(c);
}

const char *char_cast(const unsigned char *const c)
{
  return reinterpret_cast<const char *>(c);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
