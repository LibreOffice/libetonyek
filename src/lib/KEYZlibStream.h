/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYZLIBSTREAM_H_INCLUDED
#define KEYZLIBSTREAM_H_INCLUDED

#include "libkeynote_utils.h"

namespace libkeynote
{

class KEYZlibStream : public WPXInputStream
{
public:
  KEYZlibStream(const WPXInputStreamPtr_t &stream);
  virtual ~KEYZlibStream();

  virtual bool isOLEStream();
  virtual WPXInputStream *getDocumentOLEStream(const char *name);

  virtual const unsigned char *read(unsigned long numBytes, unsigned long &numBytesRead);
  virtual int seek(long offset, WPX_SEEK_TYPE seekType);
  virtual long tell();
  virtual bool atEOS();

private:
  WPXInputStreamPtr_t m_stream;
};

}

#endif // KEYZLIBSTREAM_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
