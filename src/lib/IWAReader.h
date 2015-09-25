/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWAREADER_H_INCLUDED
#define IWAREADER_H_INCLUDED

#include <string>

#include "libetonyek_utils.h"

namespace libetonyek
{

class IWAMessage;

namespace IWAReader
{

struct UInt32
{
  static uint32_t read(const RVNGInputStreamPtr_t &input, unsigned long length);
};

struct UInt64
{
  static uint64_t read(const RVNGInputStreamPtr_t &input, unsigned long length);
};

struct SInt32
{
  static int32_t read(const RVNGInputStreamPtr_t &input, unsigned long length);
};

struct SInt64
{
  static int64_t read(const RVNGInputStreamPtr_t &input, unsigned long length);
};

struct Bool
{
  static bool read(const RVNGInputStreamPtr_t &input, unsigned long length);
};

struct Fixed64
{
  static uint64_t read(const RVNGInputStreamPtr_t &input, unsigned long length);
};

struct Double
{
  static double read(const RVNGInputStreamPtr_t &input, unsigned long length);
};

struct String
{
  static std::string read(const RVNGInputStreamPtr_t &input, unsigned long length);
};

struct Message
{
  static IWAMessage read(const RVNGInputStreamPtr_t &input, unsigned long length);
};

struct Fixed32
{
  static uint32_t read(const RVNGInputStreamPtr_t &input, unsigned long length);
};

struct Float
{
  static float read(const RVNGInputStreamPtr_t &input, unsigned long length);
};

}

}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
