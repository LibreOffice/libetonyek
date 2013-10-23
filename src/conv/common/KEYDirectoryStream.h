/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYDIRECTORYSTREAM_H_INCLUDED
#define KEYDIRECTORYSTREAM_H_INCLUDED

#ifndef BOOST_SYSTEM_NO_DEPRECATED
#define BOOST_SYSTEM_NO_DEPRECATED 1
#endif
#include <boost/filesystem.hpp>

#include <libwpd-stream/libwpd-stream.h>

namespace conv
{

class KEYDirectoryStream : public WPXInputStream
{
public:
  explicit KEYDirectoryStream(const boost::filesystem::path &path);
  virtual ~KEYDirectoryStream();

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

#endif //  KEYDIRECTORYSTREAM_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
