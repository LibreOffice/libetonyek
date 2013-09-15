/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNZIPSTREAM_H_INCLUDED
#define KNZIPSTREAM_H_INCLUDED

#include <vector>

#include "libkeynote_utils.h"

namespace libkeynote
{

struct KNZipStreamImpl;

class KNZipStream : public WPXInputStream
{
public:
  KNZipStream(const WPXInputStreamPtr_t &input);
  ~KNZipStream();

  bool isOLEStream();
  WPXInputStream *getDocumentOLEStream(const char *);

  const unsigned char *read(unsigned long numBytes, unsigned long &numBytesRead);
  int seek(long offset, WPX_SEEK_TYPE seekType);
  long tell();
  bool atEOS();

private:
  KNZipStream(const KNZipStream &);
  KNZipStream &operator=(const KNZipStream &);
  KNZipStreamImpl *m_pImpl;
};

} // namespace libkeynote

#endif // KNZIPSTREAM_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
