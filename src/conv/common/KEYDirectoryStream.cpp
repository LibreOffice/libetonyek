/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEYDirectoryStream.h"

namespace conv
{

namespace
{

struct NotADirectoryException
{
};

}

KEYDirectoryStream::KEYDirectoryStream(const boost::filesystem::path &path)
  : m_path(path)
{
  if (!is_directory(path))
    throw NotADirectoryException();
}

KEYDirectoryStream::~KEYDirectoryStream()
{
}

bool KEYDirectoryStream::isStructured()
{
  return true;
}
unsigned KEYDirectoryStream::subStreamCount()
{
  // TODO: implement me
  return 1;
}

const char *KEYDirectoryStream::subStreamName(unsigned id)
{
  // TODO: implement me
  (void) id;
  return 0;
}

librevenge::RVNGInputStream *KEYDirectoryStream::getSubStreamByName(const char *const name)
{
  using boost::filesystem::path;

  const path filePath = m_path / name;

  return is_regular_file(filePath) ? new librevenge::RVNGFileStream(filePath.string().c_str()) : 0;
}

librevenge::RVNGInputStream *KEYDirectoryStream::getSubStreamById(unsigned id)
{
  // TODO: implement me
  (void) id;
  return 0;
}

const unsigned char *KEYDirectoryStream::read(const unsigned long, unsigned long &numBytesRead)
{
  numBytesRead = 0;
  return 0;
}

int KEYDirectoryStream::seek(const long, const librevenge::RVNG_SEEK_TYPE)
{
  return -1;
}

long KEYDirectoryStream::tell()
{
  return 0;
}

bool KEYDirectoryStream::isEnd()
{
  return true;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
