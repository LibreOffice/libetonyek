/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWASnappyStream.h"

#include <algorithm>
#include <cassert>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

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

void appendRef(Data &data, const unsigned offset, const unsigned length)
{
  if (offset == 0)
    throw CompressionException();
  if (offset > data.m_data.size() - data.m_blockStart) // we don't have enough uncompressed data in the current block
    throw CompressionException();

  data.m_data.resize(data.m_data.size() + length);
  const vector<unsigned char>::iterator end = data.m_data.end();
  auto dest = data.m_data.end() - vector<unsigned char>::difference_type(length);
  auto src = dest - vector<unsigned char>::difference_type(offset);

  if (offset >= length)
  {
    std::copy(src, src + vector<unsigned char>::difference_type(length), dest);
  }
  else // the run is inserted repeatedly
  {
    while (size_t(end - dest) >= offset) // as long as the whole run fits
    {
      std::copy(src, src + vector<unsigned char>::difference_type(offset), dest);
      dest += vector<unsigned char>::difference_type(offset);
    }
    if (dest != end) // copy the remainder
      std::copy(src, src + (end - dest), dest);
  }
}

bool uncompressBlock(const RVNGInputStreamPtr_t &input, const unsigned long length, vector<unsigned char> &uncompressed)
{
  Data data(uncompressed);

  const long blockEnd = input->tell() + long(length);
  const auto uncompressedLength = (unsigned long) readUVar(input);
  const size_t maxSize = size_t((std::min)(2 * length, uncompressedLength)); // don't want unbounded allocation
  size_t newSize = data.m_data.size() + maxSize;
  data.m_data.reserve(newSize);

  while (!input->isEnd() && (input->tell() < blockEnd))
  {
    const unsigned char c = readU8(input);
    switch (c & 0x3)
    {
    case 0 : // a run of literals
    {
      unsigned runLength = 0;
      if ((c & 0xf0) == 0xf0)
      {
        const unsigned count = ((c >> 2) & 0x3) + 1;
        assert(count > 0);
        assert(count <= 4);
        runLength = unsigned(readU8(input)) + 1;
        for (unsigned shift = 8; shift < 8 * count; shift += 8)
        {
          const unsigned b = readU8(input);
          runLength += b << shift;
        }
      }
      else
      {
        runLength = (c >> 2) + 1;
      }
      assert(runLength > 0);
      unsigned long bytesRead = 0;
      const unsigned char *const bytes = input->read(runLength, bytesRead);
      if (bytesRead != runLength)
        return false;
      data.m_data.insert(data.m_data.end(), bytes, bytes + runLength);
      break;
    }
    case 1 : // near ref
    {
      const unsigned runLength = ((c >> 2) & 0x7) + 4;
      const unsigned high = c >> 5;
      const unsigned low = readU8(input);
      const unsigned offset = (high << 8) | low;
      appendRef(data, offset, runLength);
      break;
    }
    case 2 : // far ref
    {
      const unsigned runLength = (c >> 2) + 1;
      const unsigned low = readU8(input);
      const unsigned high = readU8(input);
      const unsigned offset = (high << 8) | low;
      appendRef(data, offset, runLength);
      break;
    }
    case 3 : // unknown
      ETONYEK_DEBUG_MSG(("uncompressBlock: Found an unexpected mark value 3\n"));
      return false;
    default :
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
    readU8(input);
    unsigned long blockLength = readU16(input);
    // rare, but the blockLength can be greater than 65536, ie. I find 06 00 01 in one file
    blockLength+=65536*readU8(input);
    if (!uncompressBlock(input, (std::min)(blockLength, getRemainingLength(input)), data))
      throw CompressionException();
  }

  return std::make_shared<IWORKMemoryStream>(data);
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

RVNGInputStreamPtr_t IWASnappyStream::uncompressBlock(const RVNGInputStreamPtr_t &block)
{
  vector<unsigned char> data;
  libetonyek::uncompressBlock(block, getLength(block), data);
  return std::make_shared<IWORKMemoryStream>(data);
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
  return nullptr;
}

bool IWASnappyStream::existsSubStream(const char *)
{
  return false;
}

librevenge::RVNGInputStream *IWASnappyStream::getSubStreamByName(const char *)
{
  return nullptr;
}

librevenge::RVNGInputStream *IWASnappyStream::getSubStreamById(unsigned)
{
  return nullptr;
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
