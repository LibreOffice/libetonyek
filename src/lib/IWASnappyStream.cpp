/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWASnappyStream.h"

#include <cassert>
#include <limits>
#include <utility>
#include <vector>

#include <boost/make_shared.hpp>

#include "IWORKMemoryStream.h"

using std::vector;

namespace libetonyek
{

namespace
{

struct CompressionException
{
};

struct Data
{
  explicit Data(vector<unsigned char> &data);

  vector<unsigned char> &m_data; //! Uncompressed data.
  size_t m_blockStart; //! A position in m_data where data from the current block start.
};

Data::Data(vector<unsigned char> &data)
  : m_data(data)
  , m_blockStart(m_data.size())
{
}

unsigned long readVarlen(const RVNGInputStreamPtr_t &input)
{
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
    throw CompressionException();

  unsigned long value = 0;

  for (vector<unsigned char>::const_reverse_iterator it = bytes.rbegin(); it != bytes.rend(); ++it)
  {
    if (std::numeric_limits<unsigned long>::max() - value < *it) // overflow
      throw CompressionException();
    value += *it;
  }

  return value;
}

void appendRef(Data &data, const unsigned offset, const unsigned length)
{
  if (length > data.m_data.size() - data.m_blockStart) // we don't have enough uncompressed data in the current block
    throw CompressionException();

  data.m_data.resize(data.m_data.size() + length);
  const vector<unsigned char>::iterator end = data.m_data.end();
  vector<unsigned char>::iterator dest = data.m_data.end() - length;
  vector<unsigned char>::iterator src = dest - offset;

  if (offset >= length)
  {
    std::copy(src, src + length, dest);
  }
  else // the run is inserted repeatedly
  {
    while (end - dest >= offset) // as long as the whole run fits
    {
      std::copy(src, src + offset, dest);
      dest += offset;
    }
    if (dest != end) // copy the remainder
      std::copy(src, src + (end - dest), dest);
  }
}

bool uncompressBlock(const RVNGInputStreamPtr_t &input, const unsigned long length, vector<unsigned char> &uncompressed)
{
  Data data(uncompressed);

  const unsigned long uncompressedLength = readVarlen(input);
  const size_t maxSize = size_t((std::min)(2 * length, uncompressedLength)); // don't want unbounded allocation
  size_t newSize = data.m_data.size() + maxSize;
  data.m_data.reserve(newSize);

  while (!input->isEnd())
  {
    const unsigned char c = readU8(input);
    switch (c & 0x3)
    {
    case 0: // a run of literals
    {
      unsigned runLength = 0;
      if ((c & 0xf0) == 0xf0)
      {
        const unsigned count = (c >> 2) & 0x3;
        if (count >= 4)
        {
          ETONYEK_DEBUG_MSG(("uncompressBlock: Invalid number of count bytes: %u\n", count));
        }
        for (unsigned shift = 8; shift <= 8 * count; shift += 8)
        {
          const unsigned b = readU8(input);
          runLength += b << shift;
        }
      }
      else
      {
        runLength = (c >> 2) + 1;
      }
      unsigned long bytesRead = 0;
      const unsigned char *const bytes = input->read(runLength, bytesRead);
      if (bytesRead != runLength)
        return false;
      data.m_data.insert(data.m_data.end(), bytes, bytes + runLength);
      break;
    }
    case 1: // near ref
    {
      const unsigned runLength = ((c >> 2) & 0x7) + 4;
      const unsigned high = c >> 5;
      const unsigned low = readU8(input);
      const unsigned offset = (high << 8) | low;
      appendRef(data, offset, runLength);
      break;
    }
    case 2: // far ref
    {
      const unsigned runLength = (c >> 2) + 1;
      const unsigned low = readU8(input);
      const unsigned high = readU8(input);
      const unsigned offset = (high << 8) | low;
      appendRef(data, offset, runLength);
      break;
    }
    case 3: // unknown
      ETONYEK_DEBUG_MSG(("uncompressBlock: Found an unexpected mark value 3\n"));
      return false;
    default:
      assert(0);
    }
  }

  return true;
}

RVNGInputStreamPtr_t uncompress(const RVNGInputStreamPtr_t &input)
{
  vector<unsigned char> data;

  while (!input->isEnd())
  {
    const unsigned long blockLength = readVarlen(input);
    if (!uncompressBlock(input, (std::min)(blockLength, getRemainingLength(input)), data))
      throw CompressionException();
  }

  return boost::make_shared<IWORKMemoryStream>(data);
}

}

IWASnappyStream::IWASnappyStream(const RVNGInputStreamPtr_t &stream)
  : m_stream()
{
  if (0 != stream->seek(0, librevenge::RVNG_SEEK_SET))
    throw EndOfStreamException();

  m_stream = uncompress(stream);
}

IWASnappyStream::~IWASnappyStream()
{
}

bool IWASnappyStream::isStructured()
{
  return false;
}

unsigned IWASnappyStream::subStreamCount()
{
  return 0;
}

const char *IWASnappyStream::subStreamName(unsigned)
{
  return 0;
}

librevenge::RVNGInputStream *IWASnappyStream::getSubStreamByName(const char *)
{
  return 0;
}

librevenge::RVNGInputStream *IWASnappyStream::getSubStreamById(unsigned)
{
  return 0;
}

const unsigned char *IWASnappyStream::read(const unsigned long numBytes, unsigned long &numBytesRead)
{
  return m_stream->read(numBytes, numBytesRead);
}

int IWASnappyStream::seek(long offset, const librevenge::RVNG_SEEK_TYPE seekType)
{
  return m_stream->seek(offset, seekType);
}

long IWASnappyStream::tell()
{
  return m_stream->tell();
}

bool IWASnappyStream::isEnd()
{
  return m_stream->isEnd();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
