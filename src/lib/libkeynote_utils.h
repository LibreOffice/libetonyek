/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LIBKEYNOTE_UTILS_H_INCLUDED
#define LIBKEYNOTE_UTILS_H_INCLUDED

#include <stdio.h>
#include <string>
#include <libwpd/libwpd.h>
#include <libwpd-stream/libwpd-stream.h>

#ifdef _MSC_VER

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned uint32_t;
typedef unsigned int64 uint64_t;
typedef signed char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef int64 int64_t;

#else

#ifdef HAVE_CONFIG_H

#include <config.h>

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#else

// assume that the headers are there inside LibreOffice build when no HAVE_CONFIG_H is defined
#include <stdint.h>
#include <inttypes.h>

#endif

#endif

// debug message includes source file and line number
//#define VERBOSE_DEBUG 1

// do nothing with debug messages in a release compile
#ifdef DEBUG
#ifdef VERBOSE_DEBUG
#define KN_DEBUG_MSG(M) printf("%15s:%5d: ", FILE, LINE); printf M
#define KN_DEBUG(M) M
#else
#define KN_DEBUG_MSG(M) printf M
#define KN_DEBUG(M) M
#endif
#else
#define KN_DEBUG_MSG(M)
#define KN_DEBUG(M)
#endif

namespace libkeynote
{

uint8_t readU8(WPXInputStream *input, bool = false);
uint16_t readU16(WPXInputStream *input, bool bigEndian=false);
uint32_t readU32(WPXInputStream *input, bool bigEndian=false);
uint64_t readU64(WPXInputStream *input, bool bigEndian=false);

class EndOfStreamException
{
};

class GenericException
{
};

} // namespace libkeynote

#endif // LIBKEYNOTE_UTILS_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
