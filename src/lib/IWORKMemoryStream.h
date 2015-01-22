/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKMEMORYSTREAM_H_INCLUDED
#define IWORKMEMORYSTREAM_H_INCLUDED

#include <vector>

#include "libetonyek_utils.h"

namespace libetonyek
{

class IWORKMemoryStream : public librevenge::RVNGInputStream
{
  // -Weffc++
  IWORKMemoryStream(const IWORKMemoryStream &other);
  IWORKMemoryStream &operator=(const IWORKMemoryStream &other);

public:
  explicit IWORKMemoryStream(const RVNGInputStreamPtr_t &input);
  IWORKMemoryStream(const RVNGInputStreamPtr_t &input, unsigned length);
  explicit IWORKMemoryStream(std::vector<unsigned char> &data);
  IWORKMemoryStream(const unsigned char *data, unsigned length);
  ~IWORKMemoryStream();

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
  void assign(const unsigned char *data, unsigned length);
  void read(const RVNGInputStreamPtr_t &input, unsigned length);

private:
  const unsigned char *m_data;
  long m_length;
  long m_pos;
};

}

#endif // IWORKMEMORYSTREAM_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
