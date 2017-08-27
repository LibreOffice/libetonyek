/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKZLIBSTREAM_H_INCLUDED
#define IWORKZLIBSTREAM_H_INCLUDED

#include "libetonyek_utils.h"

namespace libetonyek
{

class IWORKZlibStream : public librevenge::RVNGInputStream
{
public:
  IWORKZlibStream(const RVNGInputStreamPtr_t &stream);
  ~IWORKZlibStream() override;

  bool isStructured() override;
  unsigned subStreamCount() override;
  const char *subStreamName(unsigned id) override;
  bool existsSubStream(const char *) override
  {
    return false;
  }
  librevenge::RVNGInputStream *getSubStreamByName(const char *name) override;
  librevenge::RVNGInputStream *getSubStreamById(unsigned id) override;

  const unsigned char *read(unsigned long numBytes, unsigned long &numBytesRead) override;
  int seek(long offset, librevenge::RVNG_SEEK_TYPE seekType) override;
  long tell() override;
  bool isEnd() override;

private:
  RVNGInputStreamPtr_t m_stream;
};

}

#endif // IWORKZLIBSTREAM_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
