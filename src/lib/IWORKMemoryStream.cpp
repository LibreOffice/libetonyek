/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKMemoryStream.h"

#include <algorithm>
#include <cassert>

#include "libetonyek_utils.h"

namespace libetonyek
{

IWORKMemoryStream::IWORKMemoryStream(const RVNGInputStreamPtr_t &input)
  : m_data()
  , m_length(0)
  , m_pos(0)
{
  const unsigned long begin = input->tell();
  if (input->seek(0, librevenge::RVNG_SEEK_END))
  {
    while (!input->isEnd())
      readU8(input);
  }
  const unsigned long end = input->tell();
  input->seek(begin, librevenge::RVNG_SEEK_SET);

  read(input, static_cast<unsigned>(end - begin));
}

IWORKMemoryStream::IWORKMemoryStream(const RVNGInputStreamPtr_t &input, const unsigned length)
  : m_data()
  , m_length(0)
  , m_pos(0)
{
  read(input, length);
}

IWORKMemoryStream::IWORKMemoryStream(const std::vector<unsigned char> &data)
  : m_data()
  , m_length(data.size())
  , m_pos(0)
{
  if (data.empty())
    throw GenericException();

  assign(&data[0], data.size());
}

IWORKMemoryStream::IWORKMemoryStream(const unsigned char *const data, const unsigned length)
  : m_data(nullptr)
  , m_length(length)
  , m_pos(0)
{
  if (0 == length)
    throw GenericException();

  assign(data, length);
}

IWORKMemoryStream::~IWORKMemoryStream()
{
}

bool IWORKMemoryStream::isStructured()
{
  return false;
}

unsigned IWORKMemoryStream::subStreamCount()
{
  return 0;
}

const char *IWORKMemoryStream::subStreamName(unsigned)
{
  return nullptr;
}

librevenge::RVNGInputStream *IWORKMemoryStream::getSubStreamByName(const char *)
{
  return nullptr;
}

librevenge::RVNGInputStream *IWORKMemoryStream::getSubStreamById(unsigned)
{
  return nullptr;
}

const unsigned char *IWORKMemoryStream::read(unsigned long numBytes, unsigned long &numBytesRead) try
{
  numBytesRead = 0;

  if (0 == numBytes)
    return nullptr;

  if ((m_pos + numBytes) >= static_cast<unsigned long>(m_length))
    numBytes = static_cast<unsigned long>(m_length - m_pos);

  const long oldPos = m_pos;
  m_pos += numBytes;

  numBytesRead = numBytes;
  return m_data.get() + oldPos;
}
catch (...)
{
  return nullptr;
}

int IWORKMemoryStream::seek(const long offset, librevenge::RVNG_SEEK_TYPE seekType) try
{
  long pos = 0;
  switch (seekType)
  {
  case librevenge::RVNG_SEEK_SET :
    pos = offset;
    break;
  case librevenge::RVNG_SEEK_CUR :
    pos = offset + m_pos;
    break;
  case librevenge::RVNG_SEEK_END :
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

long IWORKMemoryStream::tell()
{
  return m_pos;
}

bool IWORKMemoryStream::isEnd()
{
  return m_length == m_pos;
}

void IWORKMemoryStream::assign(const unsigned char *const data, const unsigned length)
{
  assert(0 != length);

  m_data.reset(new unsigned char[length]);
  std::copy(data, data + length, m_data.get());
}

void IWORKMemoryStream::read(const RVNGInputStreamPtr_t &input, const unsigned length)
{
  if (0 == length)
    return;
  if (!bool(input))
    throw EndOfStreamException();

  unsigned long readBytes = 0;
  const unsigned char *const data = input->read(length, readBytes);
  if (length != readBytes)
    throw EndOfStreamException();

  m_length = length;
  assign(data, length);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
