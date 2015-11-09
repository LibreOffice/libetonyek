/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWAReader.h"

#include <boost/make_shared.hpp>

#include "IWAMessage.h"
#include "IWORKMemoryStream.h"

namespace libetonyek
{

namespace
{

struct ParseError {};

}

namespace IWAReader
{

uint32_t UInt32::read(const RVNGInputStreamPtr_t &input, unsigned long)
{
  return uint32_t(readUVar(input));
}

uint64_t UInt64::read(const RVNGInputStreamPtr_t &input, unsigned long)
{
  return readUVar(input);
}

int64_t SInt64::read(const RVNGInputStreamPtr_t &input, unsigned long)
{
  return readSVar(input);
}

int32_t SInt32::read(const RVNGInputStreamPtr_t &input, unsigned long)
{
  return int32_t(readSVar(input));
}

bool Bool::read(const RVNGInputStreamPtr_t &input, unsigned long)
{
  return bool(readUVar(input));
}

uint64_t Fixed64::read(const RVNGInputStreamPtr_t &input, unsigned long)
{
  return readU64(input);
}

double Double::read(const RVNGInputStreamPtr_t &input, unsigned long)
{
  return readDouble(input);
}

std::string String::read(const RVNGInputStreamPtr_t &input, const unsigned long length)
{
  unsigned long readBytes(0);
  const unsigned char *const bytes = input->read(length, readBytes);
  if (readBytes < length)
    throw ParseError();
  return std::string(reinterpret_cast<const char *>(bytes), std::size_t(length));
}

const RVNGInputStreamPtr_t Bytes::read(const RVNGInputStreamPtr_t &input, const unsigned long length)
{
  unsigned long readBytes(0);
  const unsigned char *const bytes = input->read(length, readBytes);
  if (readBytes < length)
    throw ParseError();
  return boost::make_shared<IWORKMemoryStream>(bytes, std::size_t(length));
}

IWAMessage Message::read(const RVNGInputStreamPtr_t &input, const unsigned long length)
{
  return IWAMessage(input, length);
}

uint32_t Fixed32::read(const RVNGInputStreamPtr_t &input, unsigned long)
{
  return readU32(input);
}

float Float::read(const RVNGInputStreamPtr_t &input, unsigned long)
{
  return readFloat(input);
}

}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
