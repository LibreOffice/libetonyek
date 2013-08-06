/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <vector>

#include <zlib.h>

#include "libkeynote_utils.h"
#include "KNMemoryStream.h"
#include "KNZlibStream.h"

using std::vector;

namespace libkeynote
{

namespace
{

class ZlibStreamException
{
};

KNMemoryStream *getInflatedStream(WPXInputStream *const input)
{
  if (0x78 != readU8(input)) // not a zlib stream
    throw ZlibStreamException();

  const bool uncompressed = Z_NO_COMPRESSION == readU8(input);

  unsigned long begin = input->tell();
  input->seek(0, WPX_SEEK_END);
  unsigned long end = input->tell();
  unsigned long compressedSize = end - begin;
  input->seek(begin, WPX_SEEK_SET);

  unsigned long numBytesRead = 0;
  unsigned char *compressedData = const_cast<unsigned char *>(input->read(compressedSize, numBytesRead));

  if (uncompressed)
  {
    if (numBytesRead != compressedSize)
      throw ZlibStreamException();
    return new KNMemoryStream(compressedData, static_cast<unsigned>(compressedSize));
  }
  else
  {
    int ret;
    z_stream strm;

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit2(&strm,-MAX_WBITS);
    if (ret != Z_OK)
      throw ZlibStreamException();

    strm.avail_in = (unsigned)numBytesRead;
    strm.next_in = (Bytef *)compressedData;
    strm.total_out = 0;

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

    return new KNMemoryStream(&data[0], strm.total_out);
  }
}

}

KNZlibStream::KNZlibStream(WPXInputStream *const stream)
  : m_stream()
{
  if (0 != stream->seek(0, WPX_SEEK_SET))
    throw EndOfStreamException();

  m_stream.reset(getInflatedStream(stream));
}

KNZlibStream::~KNZlibStream()
{
}

bool KNZlibStream::isOLEStream()
{
  return false;
}

WPXInputStream *KNZlibStream::getDocumentOLEStream(const char *)
{
  return 0;
}

const unsigned char *KNZlibStream::read(const unsigned long numBytes, unsigned long &numBytesRead)
{
  return m_stream->read(numBytes, numBytesRead);
}

int KNZlibStream::seek(long offset, const WPX_SEEK_TYPE seekType)
{
  return m_stream->seek(offset, seekType);
}

long KNZlibStream::tell()
{
  return m_stream->tell();
}

bool KNZlibStream::atEOS()
{
  return m_stream->atEOS();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
