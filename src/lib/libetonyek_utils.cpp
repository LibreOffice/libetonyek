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
    bytes.push_back((unsigned char)(c & ~0x80));
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

  const auto r = unsigned(color.m_red * 256 - 0.5);
  const auto g = unsigned(color.m_green * 256 - 0.5);
  const auto b = unsigned(color.m_blue * 256 - 0.5);

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
  const auto red = unsigned((middle*gradient.m_stops.front().m_color.m_red+
                             (1-middle)*gradient.m_stops.back().m_color.m_red)* 256 - 0.5);
  const auto green = unsigned((middle*gradient.m_stops.front().m_color.m_green+
                               (1-middle)*gradient.m_stops.back().m_color.m_green)* 256 - 0.5);
  const auto blue = unsigned((middle*gradient.m_stops.front().m_color.m_blue+
                              (1-middle)*gradient.m_stops.back().m_color.m_blue)* 256 - 0.5);

  librevenge::RVNGString str;
  str.sprintf("#%.2x%.2x%.2x", red, green, blue);

  return str;
}

void writeBorder(const IWORKStroke &stroke, const char *const name, librevenge::RVNGPropertyList &props)
{
  if (stroke.m_pattern.m_type == IWORK_STROKE_TYPE_NONE)
    return;

  librevenge::RVNGString border;

  border.sprintf("%fpt", stroke.m_width);

  // The format can represent arbitrary patterns, but we have to
  // fit them to the limited number of options ODF allows...
  if ((stroke.m_pattern.m_type == IWORK_STROKE_TYPE_DASHED) && (stroke.m_pattern.m_values.size() >= 2))
  {
    const double x = stroke.m_pattern.m_values[0];
    const double y = stroke.m_pattern.m_values[1];
    // checkme: maybe we can use <sf:stroke ... sf:cap|butt ...>
    if (x>0 && y>0 && (((x / y) < 0.01) || ((y / x) < 0.01))) // arbitrarily picked constant
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

namespace
{

const unsigned char SIGNATURE_BMP[] = { 'B', 'M' };
const unsigned char SIGNATURE_GIF87[] = { 'G', 'I', 'F', '8', '7', 'a' };
const unsigned char SIGNATURE_GIF89[] = { 'G', 'I', 'F', '8', '9', 'a' };
const unsigned char SIGNATURE_JPEG[] = { 0xff, 0xd8 };
const unsigned char SIGNATURE_JPEG2000[] = { 0, 0, 0, 0xc, 'j', 'P', ' ', ' ' };
const unsigned char SIGNATURE_PDF[] = { '%', 'P', 'D', 'F' };
const unsigned char SIGNATURE_PNG[] = { 0x89, 'P', 'N', 'G', 0x0d, 0x0a, 0x1a, 0x0a };
const unsigned char SIGNATURE_QUICKTIME[] = { 'm', 'o', 'o', 'v' };
const unsigned char SIGNATURE_TIFF_1[] = { 0x49, 0x49, 0x2a, 0x00 };
const unsigned char SIGNATURE_TIFF_2[] = { 0x4d, 0x4d, 0x00, 0x2a };
}

std::string detectMimetype(const RVNGInputStreamPtr_t &stream)
try
{
  stream->seek(0, librevenge::RVNG_SEEK_SET);

  unsigned long numBytesRead = 0;
  const unsigned char *const sig = stream->read(8, numBytesRead);

  if (!sig || 8 != numBytesRead)
    // looks like the binary is broken anyway: just bail out
    return std::string();

  if (0 == std::memcmp(sig, SIGNATURE_GIF87, ETONYEK_NUM_ELEMENTS(SIGNATURE_GIF87)) ||
      0 == std::memcmp(sig, SIGNATURE_GIF89, ETONYEK_NUM_ELEMENTS(SIGNATURE_GIF89)))
    return std::string("image/gif");

  if (0 == std::memcmp(sig, SIGNATURE_PNG, ETONYEK_NUM_ELEMENTS(SIGNATURE_PNG)))
    return std::string("image/png");

  if (0 == std::memcmp(sig, SIGNATURE_PDF, ETONYEK_NUM_ELEMENTS(SIGNATURE_PDF)))
    return std::string("application/pdf");

  if ((0 == std::memcmp(sig, SIGNATURE_TIFF_1, ETONYEK_NUM_ELEMENTS(SIGNATURE_TIFF_1)))
      || (0 == std::memcmp(sig, SIGNATURE_TIFF_2, ETONYEK_NUM_ELEMENTS(SIGNATURE_TIFF_2))))
    return std::string("image/tiff");

  if (0 == std::memcmp(sig + 4, SIGNATURE_QUICKTIME, ETONYEK_NUM_ELEMENTS(SIGNATURE_QUICKTIME)))
    return std::string("video/quicktime");

  if (0 == std::memcmp(sig, SIGNATURE_JPEG, ETONYEK_NUM_ELEMENTS(SIGNATURE_JPEG)))
    return std::string("image/jpeg");

  if (0 == std::memcmp(sig, SIGNATURE_JPEG2000, ETONYEK_NUM_ELEMENTS(SIGNATURE_JPEG2000)))
    return std::string("image/jpx");

  if (0 == std::memcmp(sig, SIGNATURE_BMP, ETONYEK_NUM_ELEMENTS(SIGNATURE_BMP)))
    return std::string("image/bmp");

  // FIXME: add code to detect apple pict file, ie. MathType can generate some
  static bool first=true;
  if (first)
  {
    ETONYEK_DEBUG_MSG(("detectMimetype[libetonyek_util.cpp]: can not detect some stream types\n"));
    first=false;
  }
  return std::string();
}
catch (...)
{
  return std::string();
}


bool detectImageDimension(const RVNGInputStreamPtr_t &stream, double &width, double &height)
try
{
  width=height=0;
  stream->seek(0, librevenge::RVNG_SEEK_SET);

  unsigned long numBytesRead = 0;
  const unsigned char *const sig = stream->read(8, numBytesRead);
  if (!sig && numBytesRead!=8)
    return false;
  if (0 == std::memcmp(sig, SIGNATURE_GIF87, ETONYEK_NUM_ELEMENTS(SIGNATURE_GIF87)) ||
      0 == std::memcmp(sig, SIGNATURE_GIF89, ETONYEK_NUM_ELEMENTS(SIGNATURE_GIF89)))
  {
    stream->seek(6, librevenge::RVNG_SEEK_SET);
    width=double(readU16(stream, false));
    height=double(readU16(stream, false));
  }
  else if (0 == std::memcmp(sig, SIGNATURE_PNG, ETONYEK_NUM_ELEMENTS(SIGNATURE_PNG)))
  {
    stream->seek(8, librevenge::RVNG_SEEK_SET);
    unsigned len=readU32(stream, true);
    unsigned type=readU32(stream, true);
    if (type!=0x49484452 || len<8) // IHDR
      return false;
    width=double(readU32(stream, true));
    height=double(readU32(stream, true));
    if (stream->seek(len-8+4, librevenge::RVNG_SEEK_CUR)!=0)
      return false;
    while (!stream->isEnd())
    {
      len=readU32(stream, true);
      type=readU32(stream, true);
      if (type==0x49454e44)   // IEND
      {
        // unsure we need to divide by something like 1.7... to be more compatible with LibreOffice
        width /= 1.7;
        height /= 1.7;
        break;
      }
      if (type!=0x70485973)   // pHYs
      {
        if (stream->seek(len+4, librevenge::RVNG_SEEK_CUR)!=0)
          return false;
        continue;
      }
      if (len>=9)
      {
        unsigned xPixels=readU32(stream, true);
        unsigned yPixels=readU32(stream, true);
        unsigned unit=readU8(stream);
        if (unit==1)   // unit is meter 1 inch=0.0254 meters.
        {
          const double factor[]= {std::floor(0.0254*xPixels+0.5)/72, std::floor(0.0254*yPixels+0.5)/72};
          if (factor[0]<=0 || factor[1]<=0)
            return false;
          width/=factor[0];
          height/=factor[1];
          break;
        }
        else
          return false;
      }
      break;
    }
  }
  else if (0 == std::memcmp(sig, SIGNATURE_JPEG, ETONYEK_NUM_ELEMENTS(SIGNATURE_JPEG)))
  {
    stream->seek(0, librevenge::RVNG_SEEK_SET);
    unsigned size=2, type=0;
    double factor[]= {1,1};
    while (true)
    {
      unsigned nextPos=unsigned(stream->tell())+size;
      if (type==0xc0 || type==0xc2)
      {
        stream->seek(1, librevenge::RVNG_SEEK_CUR);
        height=double(readU16(stream, true))/factor[1];
        width=double(readU16(stream, true))/factor[0];
        break;
      }
      else if (type==0xd9)
        break;
      else if (type==0xe0)
      {
        if (readU32(stream, true)==0x4a464946)   // JFIG
        {
          stream->seek(3, librevenge::RVNG_SEEK_CUR);
          unsigned unit=readU8(stream);
          unsigned xPixels=readU16(stream, true);
          unsigned yPixels=readU16(stream, true);
          if (unit==0)
          {
            // unsure, but this seems more compatible with LibreOffice
            factor[0]=1.44*double(xPixels)/72;
            factor[1]=1.44*double(yPixels)/72;
          }
          else if (unit==1)   // by inch
          {
            factor[0]=double(xPixels)/72;
            factor[1]=double(yPixels)/72;
          }
          else if (unit==2)   // by cm
          {
            factor[0]=std::floor(2.54*xPixels+0.5)/72;
            factor[1]=std::floor(2.54*yPixels+0.5)/72;
          }
          if (factor[0]<=0 || factor[1]<=0)
            return false;
        }
      }
      if (stream->seek(nextPos, librevenge::RVNG_SEEK_SET)!=0)
        return false;
      if (stream->isEnd())
        break;
      type=readU8(stream, true);
      while (type==0xff)
        type=readU8(stream, true);
      if (type>=0xd0 && type<=0xd8)
        size=0;
      else
      {
        size=readU16(stream, true);
        if (size<2)
          return false;
        size-=2;
      }
    }
  }
#if 0
  else if (0 == std::memcmp(sig, SIGNATURE_JPEG2000, ETONYEK_NUM_ELEMENTS(SIGNATURE_JPEG2000)))
  {
    // fixme: this format can store images with different resolutions
    stream->seek(48, librevenge::RVNG_SEEK_SET);
    height=double(readU32(stream, true));
    width=double(readU32(stream, true));
  }
#endif
#if 0
  else if ((0 == std::memcmp(sig, SIGNATURE_TIFF_1, ETONYEK_NUM_ELEMENTS(SIGNATURE_TIFF_1))) ||
           (0 == std::memcmp(sig, SIGNATURE_TIFF_2, ETONYEK_NUM_ELEMENTS(SIGNATURE_TIFF_2))))
  {
    // fixme: the code seems ok, but sometimes, LibreOffice seems to
    // compute the image's dimensions differently :-~ In fact, it does
    // not display some TIFF correctly, so as this code is used for
    // retrieving cropping, the display of the picture can become
    // weirder
    bool bigEndian=std::memcmp(sig, SIGNATURE_TIFF_2, ETONYEK_NUM_ELEMENTS(SIGNATURE_TIFF_2))==0;
    stream->seek(4, librevenge::RVNG_SEEK_SET);
    if (stream->seek(readU32(stream, bigEndian), librevenge::RVNG_SEEK_SET)!=0 || stream->isEnd())
      return false;
    unsigned N=readU16(stream, bigEndian);
    unsigned unit=2;
    double factor[]= {0, 0};
    for (unsigned n=0; n<N; ++n)
    {
      unsigned tag=readU16(stream, bigEndian);
      unsigned type=readU16(stream, bigEndian);
      if (stream->seek(4, librevenge::RVNG_SEEK_CUR)!=0)
        return false;
      unsigned nextPos=unsigned(stream->tell())+4;
      if (tag==256 || tag==257)
      {
        double &wh=tag==256 ? width : height;
        if (type==3)
          wh=double(readU16(stream, bigEndian));
        else if (type==4)
          wh=double(readU32(stream, bigEndian));
        else
          return false;
      }
      if (tag==296 && type==3)
        unit=readU16(stream, bigEndian);
      if ((tag==282 || tag==283) && type==5)
      {
        if (stream->seek(readU32(stream, bigEndian), librevenge::RVNG_SEEK_SET)!=0)
          return false;
        unsigned numer=readU32(stream, bigEndian);
        unsigned denom=readU32(stream, bigEndian);
        if (!denom)
          return false;
        factor[tag==282 ? 0 : 1]=double(numer)/double(denom);
      }
      if (stream->seek(nextPos, librevenge::RVNG_SEEK_SET)!=0)
        return false;
    }
    if (unit==2)   // inch
    {
      factor[0]/=72;
      factor[1]/=72;
    }
    else if (unit==3)   // cm
    {
      factor[0]=std::floor(2.54*factor[0]+0.5)/72;
      factor[1]=std::floor(2.54*factor[1]+0.5)/72;
    }
    if (factor[0]>0 && factor[1]>0)
    {
      width /= factor[0];
      height /= factor[1];
    }
  }
#endif
  else if (0 == std::memcmp(sig, SIGNATURE_BMP, ETONYEK_NUM_ELEMENTS(SIGNATURE_BMP)))
  {
    stream->seek(14, librevenge::RVNG_SEEK_SET);
    unsigned headerSz=readU32(stream, false);
    if (headerSz==12)
    {
      width=double(readU16(stream, false));
      height=double(readU16(stream, false));
    }
    else
    {
      width=double(readU32(stream, false));
      height=double(std::abs<int32_t>(int32_t(readU32(stream, false))));
      if (headerSz>=46)
      {
        stream->seek(38, librevenge::RVNG_SEEK_SET);
        // resolution unit meters
        double factor[2];
        factor[0]=std::floor(0.0254*readU32(stream, false)+0.5)/72;
        factor[1]=std::floor(0.0254*readU32(stream, false)+0.5)/72;
        if (factor[0]<=0 || factor[1]<=0)
          return false;
        width/=factor[0];
        height/=factor[1];
      }
    }
  }
  else
    return false;
  if (width<10 || width>3000 || height<10 || height>3000)
    return false;
  return true;
}
catch (...)
{
  return false;
}


}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
