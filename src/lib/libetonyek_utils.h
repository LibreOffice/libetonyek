/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LIBETONYEK_UTILS_H_INCLUDED
#define LIBETONYEK_UTILS_H_INCLUDED

#include <cmath>
#include <stdio.h>
#include <string>

#include <boost/shared_ptr.hpp>

#include <librevenge/librevenge.h>
#include <librevenge-stream/librevenge-stream.h>

#ifdef _MSC_VER

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned uint32_t;
typedef unsigned __int64 uint64_t;
typedef signed char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef __int64 int64_t;

#else

#ifdef HAVE_CONFIG_H

#include <config.h>

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#else

// assume that the headers are there inside LibreOffice build when no HAVE_CONFIG_H is defined
#include <stdint.h>
#include <inttypes.h>

#endif

#endif

#define KEY_EPSILON 1e-9
#define KEY_ALMOST_ZERO(x) (std::fabs(x) < KEY_EPSILON)

#define KEY_NUM_ELEMENTS(array) (sizeof(array) / sizeof((array)[0]))

// debug message includes source file and line number
//#define VERBOSE_DEBUG 1

#undef DEBUG
// do nothing with debug messages in a release compile
#ifdef DEBUG
#ifdef VERBOSE_DEBUG
#define KEY_DEBUG_MSG(M) printf("%15s:%5d: ", FILE, LINE); printf M
#define KEY_DEBUG(M) M
#else
#define KEY_DEBUG_MSG(M) printf M
#define KEY_DEBUG(M) M
#endif
#else
#define KEY_DEBUG_MSG(M)
#define KEY_DEBUG(M)
#endif

namespace libetonyek
{

struct KEYDummyDeleter
{
  void operator()(void *) {}
};

typedef boost::shared_ptr<librevenge::RVNGInputStream> RVNGInputStreamPtr_t;

uint8_t readU8(const RVNGInputStreamPtr_t &input, bool = false);
uint16_t readU16(const RVNGInputStreamPtr_t &input, bool bigEndian=false);
uint32_t readU32(const RVNGInputStreamPtr_t &input, bool bigEndian=false);
uint64_t readU64(const RVNGInputStreamPtr_t &input, bool bigEndian=false);

/** Test two floating point numbers for equality.
  *
  * @arg[in] x first number
  * @arg[in] y second number
  * @arg[in] eps precision
  */
bool approxEqual(double x, double y, double eps = KEY_EPSILON);

/** Convert a length from points to inches.
  *
  * @arg[in] d length in points
  * @returns length in inches
  */
double pt2in(double d);

class EndOfStreamException
{
};

class GenericException
{
};

} // namespace libetonyek

#endif // LIBETONYEK_UTILS_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
