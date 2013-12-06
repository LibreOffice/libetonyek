/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYMEMORYSTREAM_H_INCLUDED
#define KEYMEMORYSTREAM_H_INCLUDED

#include <vector>

#include "libetonyek_utils.h"

namespace libetonyek
{

class KEYMemoryStream : public librevenge::RVNGInputStream
{
  // -Weffc++
  KEYMemoryStream(const KEYMemoryStream &other);
  KEYMemoryStream &operator=(const KEYMemoryStream &other);

public:
  explicit KEYMemoryStream(const RVNGInputStreamPtr_t &input);
  KEYMemoryStream(const RVNGInputStreamPtr_t &input, unsigned length);
  explicit KEYMemoryStream(std::vector<unsigned char> &data);
  KEYMemoryStream(const unsigned char *data, unsigned length);
  virtual ~KEYMemoryStream();

  virtual bool isStructured();
  virtual unsigned subStreamCount();
  virtual const char *subStreamName(unsigned id);
  virtual librevenge::RVNGInputStream *getSubStreamByName(const char *name);
  virtual librevenge::RVNGInputStream *getSubStreamById(unsigned id);

  virtual const unsigned char *read(unsigned long numBytes, unsigned long &numBytesRead);
  virtual int seek(long offset, librevenge::RVNG_SEEK_TYPE seekType);
  virtual long tell();
  virtual bool isEnd();

private:
  void assign(const unsigned char *data, unsigned length);
  void read(const RVNGInputStreamPtr_t &input, unsigned length);

private:
  const unsigned char *m_data;
  long m_length;
  long m_pos;
};

}

#endif // KEYMEMORYSTREAM_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
