/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYZLIBSTREAM_H_INCLUDED
#define KEYZLIBSTREAM_H_INCLUDED

#include "libetonyek_utils.h"

namespace libetonyek
{

class KEYZlibStream : public librevenge::RVNGInputStream
{
public:
  KEYZlibStream(const RVNGInputStreamPtr_t &stream);
  ~KEYZlibStream();

  bool isStructured();
  unsigned subStreamCount();
  const char *subStreamName(unsigned id);
  bool existsSubStream(const char *)
  {
    return false;
  }
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

#endif // KEYZLIBSTREAM_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
