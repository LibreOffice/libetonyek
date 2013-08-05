/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Major Contributor(s):
 * Copyright (C) 2012 Fridrich Strba <fridrich.strba@bluewin.ch>
 * Copyright (C) 2011 Eilidh McAdam <tibbylickle@gmail.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPLv2+"), or
 * the GNU Lesser General Public License Version 2 or later (the "LGPLv2+"),
 * in which case the provisions of the GPLv2+ or the LGPLv2+ are applicable
 * instead of those above.
 */

#ifndef KNZIPSTREAM_H_INCLUDED
#define KNZIPSTREAM_H_INCLUDED

#include <vector>

#include <libwpd-stream/libwpd-stream.h>

namespace libkeynote
{

struct KNZipStreamImpl;

class KNZipStream : public WPXInputStream
{
public:
  KNZipStream(WPXInputStream *input);
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
