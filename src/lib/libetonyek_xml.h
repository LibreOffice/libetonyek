/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LIBETONYEK_XML_H_INCLUDED
#define LIBETONYEK_XML_H_INCLUDED

#include <string>

#include <boost/optional.hpp>

#include "libetonyek_utils.h"

extern "C" int readFromStream(void *context, char *buffer, int len);

extern "C" int closeStream(void * /* context */);

namespace libetonyek
{

/** Convert string value to bool.
  *
  * @arg value the string
  * @returns the boolean value of the string
  */
bool bool_cast(const char *value);
boost::optional<bool> try_bool_cast(const char *value);

double double_cast(const char *value);
boost::optional<double> try_double_cast(const char *value);

int int_cast(const char *value);
boost::optional<int> try_int_cast(const char *value);

const char *char_cast(const char *c);
const char *char_cast(const signed char *c);
const char *char_cast(const unsigned char *c);

}

#endif // LIBETONYEK_XML_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
