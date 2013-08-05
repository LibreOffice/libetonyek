/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "libkeynote_utils.h"

namespace libkeynote
{

uint8_t readU8(WPXInputStream *input)
{
  if (!input || input->atEOS())
  {
    KN_DEBUG_MSG(("Throwing EndOfStreamException\n"));
    throw EndOfStreamException();
  }
  unsigned long numBytesRead;
  uint8_t const *p = input->read(sizeof(uint8_t), numBytesRead);

  if (p && numBytesRead == sizeof(uint8_t))
    return *(uint8_t const *)(p);
  KN_DEBUG_MSG(("Throwing EndOfStreamException\n"));
  throw EndOfStreamException();
}

int8_t readS8(WPXInputStream *input)
{
  return (int8_t)readU8(input);
}

uint16_t readU16(WPXInputStream *input)
{
  if (!input || input->atEOS())
  {
    KN_DEBUG_MSG(("Throwing EndOfStreamException\n"));
    throw EndOfStreamException();
  }
  unsigned long numBytesRead;
  uint8_t const *p = input->read(sizeof(uint16_t), numBytesRead);

  if (p && numBytesRead == sizeof(uint16_t))
    return (uint16_t)p[1]|((uint16_t)p[0]<<8);

  KN_DEBUG_MSG(("Throwing EndOfStreamException\n"));
  throw EndOfStreamException();
}

int16_t readS16(WPXInputStream *input)
{
  return (int16_t)readU16(input);
}

uint32_t readU32(WPXInputStream *input)
{
  if (!input || input->atEOS())
  {
    KN_DEBUG_MSG(("Throwing EndOfStreamException\n"));
    throw EndOfStreamException();
  }
  unsigned long numBytesRead;
  uint8_t const *p = input->read(sizeof(uint32_t), numBytesRead);

  if (p && numBytesRead == sizeof(uint32_t))
    return (uint32_t)p[3]|((uint32_t)p[2]<<8)|((uint32_t)p[1]<<16)|((uint32_t)p[0]<<24);

  KN_DEBUG_MSG(("Throwing EndOfStreamException\n"));
  throw EndOfStreamException();
}

int32_t readS32(WPXInputStream *input)
{
  return (int32_t)readU32(input);
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
