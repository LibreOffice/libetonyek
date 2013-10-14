/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNDIRECTORYSTREAM_H_INCLUDED
#define KNDIRECTORYSTREAM_H_INCLUDED

#include <boost/filesystem.hpp>

#include <libwpd-stream/libwpd-stream.h>

namespace conv
{

class KNDirectoryStream : public WPXInputStream
{
public:
  explicit KNDirectoryStream(const boost::filesystem::path &path);
  virtual ~KNDirectoryStream();

  virtual bool isOLEStream();
  virtual WPXInputStream *getDocumentOLEStream(const char *name);

  virtual const unsigned char *read(unsigned long numBytes, unsigned long &numBytesRead);
  virtual int seek(long offset, WPX_SEEK_TYPE seekType);
  virtual long tell();
  virtual bool atEOS();

private:
  boost::filesystem::path m_path;
};

}

#endif //  KNDIRECTORYSTREAM_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
