/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "libetonyek_utils.h"
#include "IWORKTypes.h"

namespace libetonyek
{

namespace
{

void checkStream(const RVNGInputStreamPtr_t &input)
{
  if (!input || input->isEnd())
    throw EndOfStreamException();
}

}

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
  while (360 <= value)
    value -= 360;
  while (0 > value)
    value += 360;

  // convert
  return etonyek_pi / 180 * value;
}

librevenge::RVNGString makeColor(const IWORKColor &color)
{
  // TODO: alpha

  const unsigned r = color.m_red * 256 - 0.5;
  const unsigned g = color.m_green * 256 - 0.5;
  const unsigned b = color.m_blue * 256 - 0.5;

  librevenge::RVNGString str;
  str.sprintf("#%.2x%.2x%.2x", r, g, b);

  return str;
}

librevenge::RVNGString makeBorder(const IWORKStroke &stroke)
{
  librevenge::RVNGString border;

  border.sprintf("%fpt", stroke.m_width);

  // The format can represent arbitrary patterns, but we have to
  // fit them to the limited number of options ODF allows...
  if (stroke.m_pattern.size() >= 2)
  {
    const double x = stroke.m_pattern[0];
    const double y = stroke.m_pattern[1];
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
  return border;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
