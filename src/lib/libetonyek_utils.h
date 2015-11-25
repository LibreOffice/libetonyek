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

#define ETONYEK_EPSILON 1e-9
#define ETONYEK_ALMOST_ZERO(x) (std::fabs(x) < ETONYEK_EPSILON)

#define ETONYEK_NUM_ELEMENTS(array) (sizeof(array) / sizeof((array)[0]))

#if defined(__clang__) || defined(__GNUC__)
#  define ETONYEK_ATTRIBUTE_PRINTF(fmt, arg) __attribute__((__format__(__printf__, fmt, arg)))
#else
#  define ETONYEK_ATTRIBUTE_PRINTF(fmt, arg)
#endif

// debug message includes source file and line number
//#define VERBOSE_DEBUG 1

// do nothing with debug messages in a release compile
#ifdef DEBUG
namespace libetonyek
{
void debugPrint(const char *format, ...) ETONYEK_ATTRIBUTE_PRINTF(1, 2);
}
#ifdef VERBOSE_DEBUG
#define ETONYEK_DEBUG_MSG(M) libetonyek::debugPrint("%15s:%5d: ", FILE, LINE); libetonyek::debugPrint M
#define ETONYEK_DEBUG(M) M
#else
#define ETONYEK_DEBUG_MSG(M) libetonyek::debugPrint M
#define ETONYEK_DEBUG(M) M
#endif
#else
#define ETONYEK_DEBUG_MSG(M)
#define ETONYEK_DEBUG(M)
#endif

namespace libetonyek
{

struct IWORKColor;
struct IWORKStroke;

/* Constants */
const double etonyek_half_pi(1.57079632679489661923132169163975144209858469968755291048747229615390820314310449931401741267105853399107404326e+00);
const double etonyek_third_pi(1.04719755119659774615421446109316762806572313312503527365831486410260546876206966620934494178070568932738269550e+00);
const double etonyek_pi(3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651e+00);
const double etonyek_two_pi(6.28318530717958647692528676655900576839433879875021164194988918461563281257241799725606965068423413596429617303e+00);

const double etonyek_root_three(1.73205080756887729352744634150587236694280525381038062805580697945193301690880003708114618675724857567562614142e+00);
const double etonyek_root_two(1.41421356237309504880168872420969807856967187537694807317667973799073247846210703885038753432764157273501384623e+00);

// Apple Numbers timestamp starts from 01 Jan 2001 00:00:00
const unsigned ETONYEK_EPOCH_BEGIN(978307200);

struct EtonyekDummyDeleter
{
  void operator()(void *) {}
};

typedef boost::shared_ptr<librevenge::RVNGInputStream> RVNGInputStreamPtr_t;

uint8_t readU8(const RVNGInputStreamPtr_t &input, bool = false);
uint16_t readU16(const RVNGInputStreamPtr_t &input, bool bigEndian=false);
uint32_t readU32(const RVNGInputStreamPtr_t &input, bool bigEndian=false);
uint64_t readU64(const RVNGInputStreamPtr_t &input, bool bigEndian=false);

uint64_t readUVar(const RVNGInputStreamPtr_t &input);
int64_t readSVar(const RVNGInputStreamPtr_t &input);

double readDouble(const RVNGInputStreamPtr_t &input);
float readFloat(const RVNGInputStreamPtr_t &input);

unsigned long getLength(const RVNGInputStreamPtr_t &input);
unsigned long getRemainingLength(const RVNGInputStreamPtr_t &input);

/** Test two floating point numbers for equality.
  *
  * @arg[in] x first number
  * @arg[in] y second number
  * @arg[in] eps precision
  */
bool approxEqual(double x, double y, double eps = ETONYEK_EPSILON);

template<class T>
bool approxEqual(const T &left, const T &right, const double eps = ETONYEK_EPSILON)
{
  assert(left.length() == right.length());

  for (int i = 0; i != left.length(); ++i)
  {
    if (!approxEqual(left[i], right[i], eps))
      return false;
  }
  return true;
}

/** Convert a length from points to inches.
  *
  * @arg[in] d length in points
  * @returns length in inches
  */
double pt2in(double d);

/** Convert an angle from degrees to radians.
  *
  * @arg[in] value angle in degrees
  * @returns the same angle in radians
  */
double deg2rad(double value);

/** Convert an angle from radians to degrees.
  *
  * @arg[in] value angle in radians
  * @returns the same angle in degrees
  */
double rad2deg(double value);

librevenge::RVNGString makeColor(const IWORKColor &color);

librevenge::RVNGString makeBorder(const IWORKStroke &stroke);

class EndOfStreamException
{
};

class GenericException
{
};

} // namespace libetonyek

#endif // LIBETONYEK_UTILS_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
