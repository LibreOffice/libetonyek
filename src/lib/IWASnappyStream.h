/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWASNAPPYSTREAM_H_INCLUDED
#define IWASNAPPYSTREAM_H_INCLUDED

#include <librevenge-stream/librevenge-stream.h>

#include "libetonyek_utils.h"

namespace libetonyek
{

class IWASnappyStream : public librevenge::RVNGInputStream
{
public:
  IWASnappyStream(const RVNGInputStreamPtr_t &stream);
  ~IWASnappyStream();

  bool isStructured();
  unsigned subStreamCount();
  const char *subStreamName(unsigned id);
  bool existsSubStream(const char *name);

  librevenge::RVNGInputStream *getSubStreamByName(const char *name);
  librevenge::RVNGInputStream *getSubStreamById(unsigned id);

  const unsigned char *read(unsigned long numBytes, unsigned long &numBytesRead);
  int seek(long offset, librevenge::RVNG_SEEK_TYPE seekType);
  long tell();
  bool isEnd();

private:
  RVNGInputStreamPtr_t m_stream;
};

}

#endif // IWASNAPPYSTREAM_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
