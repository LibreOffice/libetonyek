/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "libetonyek_utils.h"

#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <limits>
#include <stdexcept>

#include "IWORKTypes.h"

namespace libetonyek
{

using std::numeric_limits;
using std::vector;
using std::range_error;

namespace
{

void checkStream(const RVNGInputStreamPtr_t &input)
{
  if (!input || input->isEnd())
    throw EndOfStreamException();
}

}

#ifdef DEBUG
void debugPrint(const char *const format, ...)
{
  va_list args;
  va_start(args, format);
  std::vfprintf(stderr, format, args);
  va_end(args);
}
#endif

uint8_t readU8(const RVNGInputStreamPtr_t &input, bool /* bigEndian */)
{
  checkStream(input);

  unsigned long numBytesRead;
  uint8_t const *p = input->read(sizeof(uint8_t), numBytesRead);

  if (p && numBytesRead == sizeof(uint8_t))
    return *(uint8_t const *)(p);
  throw EndOfStreamException();
}

uint16_t readU16(const RVNGInputStreamPtr_t &input, bool bigEndian)
{
  checkStream(input);

  unsigned long numBytesRead;
  uint8_t const *p = input->read(sizeof(uint16_t), numBytesRead);

  if (p && numBytesRead == sizeof(uint16_t))
  {
    if (bigEndian)
      return static_cast<uint16_t>((uint16_t)p[1]|((uint16_t)p[0]<<8));
    return static_cast<uint16_t>((uint16_t)p[0]|((uint16_t)p[1]<<8));
  }
  throw EndOfStreamException();
}

uint32_t readU32(const RVNGInputStreamPtr_t &input, bool bigEndian)
{
  checkStream(input);

  unsigned long numBytesRead;
  uint8_t const *p = input->read(sizeof(uint32_t), numBytesRead);

  if (p && numBytesRead == sizeof(uint32_t))
  {
    if (bigEndian)
      return (uint32_t)p[3]|((uint32_t)p[2]<<8)|((uint32_t)p[1]<<16)|((uint32_t)p[0]<<24);
    return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);
  }
  throw EndOfStreamException();
}

uint64_t readU64(const RVNGInputStreamPtr_t &input, bool bigEndian)
{
  checkStream(input);

  unsigned long numBytesRead;
  uint8_t const *p = input->read(sizeof(uint64_t), numBytesRead);

  if (p && numBytesRead == sizeof(uint64_t))
  {
    if (bigEndian)
      return (uint64_t)p[7]|((uint64_t)p[6]<<8)|((uint64_t)p[5]<<16)|((uint64_t)p[4]<<24)|((uint64_t)p[3]<<32)|((uint64_t)p[2]<<40)|((uint64_t)p[1]<<48)|((uint64_t)p[0]<<56);
    return (uint64_t)p[0]|((uint64_t)p[1]<<8)|((uint64_t)p[2]<<16)|((uint64_t)p[3]<<24)|((uint64_t)p[4]<<32)|((uint64_t)p[5]<<40)|((uint64_t)p[6]<<48)|((uint64_t)p[7]<<56);
  }
  throw EndOfStreamException();
}

uint64_t readUVar(const RVNGInputStreamPtr_t &input)
{
  if (!input || input->isEnd())
    throw EndOfStreamException();

  vector<unsigned char> bytes;
  bytes.reserve(8);

  bool cont = true;
  while (!input->isEnd() && cont)
  {
    const unsigned char c = readU8(input);
    bytes.push_back(c & ~0x80);
    cont = c & 0x80;
  }

  if (cont && input->isEnd())
    throw EndOfStreamException();

  uint64_t value = 0;

  for (vector<unsigned char>::const_reverse_iterator it = bytes.rbegin(); it != bytes.rend(); ++it)
  {
    if (numeric_limits<uint64_t>::max() >> 7 < value) // overflow
      throw range_error("Number too big");
    if (numeric_limits<uint64_t>::max() - (value << 7) < *it) // overflow
      throw range_error("Number too big");
    value = (value << 7) + *it;
  }

  return value;
}

int64_t readSVar(const RVNGInputStreamPtr_t &input)
{
  const uint64_t encoded = readUVar(input);
  const unsigned mod = encoded % 2;

  const uint64_t val = (encoded / 2 + mod);

  // sanity check
  if (!mod && (val > uint64_t(numeric_limits<int64_t>::max())))
    throw range_error("Number too big");
  assert(-numeric_limits<int64_t>::max() == numeric_limits<int64_t>::min() + 1);
  if (mod && (val > 0) && (val - 1 > uint64_t(std::abs(numeric_limits<int64_t>::min() + 1))))
    throw range_error("Number too small");

  // special handling, as the abs. value of minimal int64_t number doesn't fit into int64_t
  if (mod && (val > 0) && (val - 1 == uint64_t(std::abs(numeric_limits<int64_t>::min() + 1))))
    return numeric_limits<int64_t>::min();

  return mod ? -int64_t(val) : int64_t(val);
}

double readDouble(const RVNGInputStreamPtr_t &input)
{
  union
  {
    uint64_t u;
    double d;
  } convert;
  convert.u = readU64(input);
  return convert.d;
}

float readFloat(const RVNGInputStreamPtr_t &input)
{
  union
  {
    uint32_t u;
    float f;
  } convert;
  convert.u = readU32(input);
  return convert.f;
}

unsigned long getLength(const RVNGInputStreamPtr_t &input)
{
  if (!input)
    throw EndOfStreamException();

  const long orig = input->tell();
  long end = 0;

  if (input->seek(0, librevenge::RVNG_SEEK_END) == 0)
  {
    end = input->tell();
  }
  else
  {
    // RVNG_SEEK_END does not work. Use the harder way.
    if (input->seek(0, librevenge::RVNG_SEEK_SET) != 0)
      throw EndOfStreamException();
    while (!input->isEnd())
    {
      readU8(input);
      ++end;
    }
  }
  assert(end >= 0);

  if (input->seek(orig, librevenge::RVNG_SEEK_SET) != 0)
    throw EndOfStreamException();

  return static_cast<unsigned long>(end);
}

unsigned long getRemainingLength(const RVNGInputStreamPtr_t &input)
{
  return getLength(input) - static_cast<unsigned long>(input->tell());
}

bool approxEqual(const double x, const double y, const double eps)
{
  return std::fabs(x - y) < eps;
}

double pt2in(const double d)
{
  return d / 72;
}

double deg2rad(double value)
{
  // normalize range
  value = std::fmod(value, 360);
  if (value < 0)
    value += 360;
  return etonyek_pi / 180 * value;
}

double rad2deg(double value)
{
  // normalize range
  while (etonyek_two_pi <= value)
    value -= etonyek_two_pi;
  while (0 > value)
    value += etonyek_two_pi;

  // convert
  return value / etonyek_pi * 180;
}

librevenge::RVNGString makeColor(const IWORKColor &color)
{
  // TODO: alpha

  const unsigned r = unsigned(color.m_red * 256 - 0.5);
  const unsigned g = unsigned(color.m_green * 256 - 0.5);
  const unsigned b = unsigned(color.m_blue * 256 - 0.5);

  librevenge::RVNGString str;
  str.sprintf("#%.2x%.2x%.2x", r, g, b);

  return str;
}

librevenge::RVNGString makeColor(const IWORKGradient &gradient)
{
  if (gradient.m_stops.empty())
  {
    ETONYEK_DEBUG_MSG(("makeColor[gradient]: can not find gradient stops\n"));
    return "#ffffff";
  }
  const double middle=(gradient.m_stops.front().m_fraction+gradient.m_stops.back().m_fraction)/2;
  const unsigned red = unsigned((middle*gradient.m_stops.front().m_color.m_red+
                                 (1-middle)*gradient.m_stops.back().m_color.m_red)* 256 - 0.5);
  const unsigned green = unsigned((middle*gradient.m_stops.front().m_color.m_green+
                                   (1-middle)*gradient.m_stops.back().m_color.m_green)* 256 - 0.5);
  const unsigned blue = unsigned((middle*gradient.m_stops.front().m_color.m_blue+
                                  (1-middle)*gradient.m_stops.back().m_color.m_blue)* 256 - 0.5);

  librevenge::RVNGString str;
  str.sprintf("#%.2x%.2x%.2x", red, green, blue);

  return str;
}

void writeBorder(const IWORKStroke &stroke, const char *const name, librevenge::RVNGPropertyList &props)
{
  if (stroke.m_type == IWORK_STROKE_TYPE_NONE)
    return;

  librevenge::RVNGString border;

  border.sprintf("%fpt", stroke.m_width);

  // The format can represent arbitrary patterns, but we have to
  // fit them to the limited number of options ODF allows...
  if ((stroke.m_type == IWORK_STROKE_TYPE_DASHED) && (stroke.m_pattern.size() >= 2))
  {
    const double x = stroke.m_pattern[0];
    const double y = stroke.m_pattern[1];
    // checkme: maybe we can use <sf:stroke ... sf:cap|butt ...>
    if (((x / y) < 0.01) || ((y / x) < 0.01)) // arbitrarily picked constant
      border.append(" dotted");
    else
      border.append(" dashed");
  }
  else
  {
    border.append(" solid");
  }

  border.append(" ");
  border.append(makeColor(stroke.m_color));

  props.insert(name, border);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
