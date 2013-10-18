/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYMEMORYSTREAM_H_INCLUDED
#define KEYMEMORYSTREAM_H_INCLUDED

#include <vector>

#include "libkeynote_utils.h"

namespace libkeynote
{

class KEYMemoryStream : public WPXInputStream
{
  // -Weffc++
  KEYMemoryStream(const KEYMemoryStream &other);
  KEYMemoryStream &operator=(const KEYMemoryStream &other);

public:
  explicit KEYMemoryStream(const WPXInputStreamPtr_t &input);
  KEYMemoryStream(const WPXInputStreamPtr_t &input, unsigned length);
  explicit KEYMemoryStream(std::vector<unsigned char> &data);
  KEYMemoryStream(const unsigned char *data, unsigned length);
  virtual ~KEYMemoryStream();

  virtual bool isOLEStream();
  virtual WPXInputStream *getDocumentOLEStream(const char *name);

  virtual const unsigned char *read(unsigned long numBytes, unsigned long &numBytesRead);
  virtual int seek(long offset, WPX_SEEK_TYPE seekType);
  virtual long tell();
  virtual bool atEOS();

private:
  void assign(const unsigned char *data, unsigned length);
  void read(const WPXInputStreamPtr_t &input, unsigned length);

private:
  const unsigned char *m_data;
  long m_length;
  long m_pos;
};

}

#endif // KEYMEMORYSTREAM_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
