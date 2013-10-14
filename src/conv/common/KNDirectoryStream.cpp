/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KNDirectoryStream.h"

namespace conv
{

namespace
{

struct NotADirectoryException
{
};

struct InvalidSubPathException
{
};

}

KNDirectoryStream::KNDirectoryStream(const boost::filesystem::path &path)
  : m_path(path)
{
  if (!is_directory(path))
    throw NotADirectoryException();
}

KNDirectoryStream::~KNDirectoryStream()
{
}

bool KNDirectoryStream::isOLEStream()
{
  return true;
}

WPXInputStream *KNDirectoryStream::getDocumentOLEStream(const char *const name)
{
  using boost::filesystem::path;

  const path filePath = m_path / name;

  if (!is_regular_file(filePath))
    throw InvalidSubPathException();

  return new WPXFileStream(filePath.c_str());
}

const unsigned char *KNDirectoryStream::read(const unsigned long, unsigned long &numBytesRead)
{
  numBytesRead = 0;
  return 0;
}

int KNDirectoryStream::seek(const long, const WPX_SEEK_TYPE)
{
  return -1;
}

long KNDirectoryStream::tell()
{
  return 0;
}

bool KNDirectoryStream::atEOS()
{
  return true;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
