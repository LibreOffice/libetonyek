/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <algorithm>

#include "libetonyek_utils.h"
#include "KEYMemoryStream.h"

namespace libetonyek
{

KEYMemoryStream::KEYMemoryStream(const WPXInputStreamPtr_t &input)
  : m_data(0)
  , m_length(0)
  , m_pos(0)
{
  const unsigned long begin = input->tell();
  if (input->seek(0, WPX_SEEK_END))
  {
    while (!input->atEOS())
      readU8(input);
  }
  const unsigned long end = input->tell();
  input->seek(begin, WPX_SEEK_SET);

  read(input, static_cast<unsigned>(end - begin));
}

KEYMemoryStream::KEYMemoryStream(const WPXInputStreamPtr_t &input, const unsigned length)
  : m_data(0)
  , m_length(0)
  , m_pos(0)
{
  read(input, length);
}

KEYMemoryStream::KEYMemoryStream(std::vector<unsigned char> &data)
  : m_data(0)
  , m_length(data.size())
  , m_pos(0)
{
  if (data.empty())
    throw GenericException();

  assign(&data[0], data.size());
}

KEYMemoryStream::KEYMemoryStream(const unsigned char *const data, const unsigned length)
  : m_data(0)
  , m_length(length)
  , m_pos(0)
{
  assign(data, length);
}

KEYMemoryStream::~KEYMemoryStream()
{
  delete[] m_data;
}

bool KEYMemoryStream::isOLEStream()
{
  return false;
}

WPXInputStream *KEYMemoryStream::getDocumentOLEStream(const char *)
{
  return 0;
}

const unsigned char *KEYMemoryStream::read(unsigned long numBytes, unsigned long &numBytesRead) try
{
  numBytesRead = 0;

  if (0 == numBytes)
    return 0;

  if ((m_pos + numBytes) >= static_cast<unsigned long>(m_length))
    numBytes = static_cast<unsigned long>(m_length - m_pos);

  const long oldPos = m_pos;
  m_pos += numBytes;

  numBytesRead = numBytes;
  return m_data + oldPos;
}
catch (...)
{
  return 0;
}

int KEYMemoryStream::seek(const long offset, WPX_SEEK_TYPE seekType) try
{
  long pos = 0;
  switch (seekType)
  {
  case WPX_SEEK_SET :
    pos = offset;
    break;
  case WPX_SEEK_CUR :
    pos = offset + m_pos;
    break;
  case WPX_SEEK_END :
    pos = offset + m_length;
    break;
  default :
    return -1;
  }

  if ((pos < 0) || (pos > m_length))
    return 1;

  m_pos = pos;
  return 0;
}
catch (...)
{
  return -1;
}

long KEYMemoryStream::tell()
{
  return m_pos;
}

bool KEYMemoryStream::atEOS()
{
  return m_length == m_pos;
}

void KEYMemoryStream::assign(const unsigned char *const data, const unsigned length)
{
  unsigned char *buffer = new unsigned char[length];
  std::copy(data, data + length, buffer);
  m_data = buffer;
}

void KEYMemoryStream::read(const WPXInputStreamPtr_t &input, const unsigned length)
{
  unsigned long readBytes = 0;
  const unsigned char *const data = bool(input) ? input->read(length, readBytes) : 0;
  if (length != readBytes)
    throw EndOfStreamException();

  m_length = length;
  assign(data, length);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
