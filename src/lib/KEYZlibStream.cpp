/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <vector>

#include <zlib.h>

#include "libetonyek_utils.h"
#include "KEYMemoryStream.h"
#include "KEYZlibStream.h"

using std::vector;

namespace libetonyek
{

namespace
{

class ZlibStreamException
{
};

RVNGInputStreamPtr_t getInflatedStream(const RVNGInputStreamPtr_t &input)
{
  unsigned long offset = 2;

  const unsigned char sig1 = readU8(input);
  if (0x78 != sig1) // not a zlib stream
  {
    offset = 3;
    const unsigned char sig2 = readU8(input);
    if ((0x1f != sig1) || (0x8b != sig2))
      throw ZlibStreamException();
  }

  const bool uncompressed = Z_NO_COMPRESSION == readU8(input);
  if (uncompressed)
    offset = 0;

  unsigned long begin = input->tell();
  input->seek(0, librevenge::RVNG_SEEK_END);
  unsigned long end = input->tell();
  unsigned long compressedSize = end - begin + offset;
  input->seek(begin - offset, librevenge::RVNG_SEEK_SET);

  unsigned long numBytesRead = 0;
  unsigned char *compressedData = const_cast<unsigned char *>(input->read(compressedSize, numBytesRead));

  if (uncompressed)
  {
    if (numBytesRead != compressedSize)
      throw ZlibStreamException();
    return RVNGInputStreamPtr_t(new KEYMemoryStream(compressedData, static_cast<unsigned>(compressedSize)));
  }
  else
  {
    int ret;
    z_stream strm;

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = (unsigned)numBytesRead;
    strm.next_in = (Bytef *)compressedData;
    strm.total_out = 0;

    ret = inflateInit2(&strm, 16 + MAX_WBITS);
    if (ret != Z_OK)
      throw ZlibStreamException();

    vector<unsigned char> data(2 * compressedSize);

    while (true)
    {
      strm.next_out = reinterpret_cast<Bytef *>(&data[strm.total_out]);
      strm.avail_out = data.size() - strm.total_out;
      ret = inflate(&strm, Z_SYNC_FLUSH);

      if (Z_STREAM_END == ret)
        break;
      if ((Z_OK == ret) && (0 == strm.avail_in) && (0 < strm.avail_out)) // end of stream too
        break;
      if (Z_OK != ret)
      {
        (void)inflateEnd(&strm);
        throw ZlibStreamException();
      }

      data.resize(data.size() + compressedSize);
    }

    (void)inflateEnd(&strm);

    return RVNGInputStreamPtr_t(new KEYMemoryStream(&data[0], strm.total_out));
  }
}

}

KEYZlibStream::KEYZlibStream(const RVNGInputStreamPtr_t &stream)
  : m_stream()
{
  if (0 != stream->seek(0, librevenge::RVNG_SEEK_SET))
    throw EndOfStreamException();

  m_stream = getInflatedStream(stream);
}

KEYZlibStream::~KEYZlibStream()
{
}

bool KEYZlibStream::isStructured()
{
  return false;
}

unsigned KEYZlibStream::subStreamCount()
{
  return 0;
}

const char *KEYZlibStream::subStreamName(unsigned)
{
  return 0;
}

librevenge::RVNGInputStream *KEYZlibStream::getSubStreamByName(const char *)
{
  return 0;
}

librevenge::RVNGInputStream *KEYZlibStream::getSubStreamById(unsigned)
{
  return 0;
}

const unsigned char *KEYZlibStream::read(const unsigned long numBytes, unsigned long &numBytesRead)
{
  return m_stream->read(numBytes, numBytesRead);
}

int KEYZlibStream::seek(long offset, const librevenge::RVNG_SEEK_TYPE seekType)
{
  return m_stream->seek(offset, seekType);
}

long KEYZlibStream::tell()
{
  return m_stream->tell();
}

bool KEYZlibStream::isEnd()
{
  return m_stream->isEnd();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
