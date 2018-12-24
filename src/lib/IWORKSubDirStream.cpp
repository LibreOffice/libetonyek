/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKSubDirStream.h"

#include <cassert>

namespace libetonyek
{

namespace
{

std::string getDelimited(const std::string &path)
{
  if (path.empty() || path.back() == '/')
    return path;
  else
    return path + '/';
}

}

IWORKSubDirStream::IWORKSubDirStream(const RVNGInputStreamPtr_t &parent, const std::string &dir)
  : m_parent(parent)
  , m_dir(getDelimited(dir))
{
  assert(bool(m_parent));
  assert(m_parent->isStructured());
}

IWORKSubDirStream::~IWORKSubDirStream()
{
}

bool IWORKSubDirStream::isStructured()
{
  return true;
}

unsigned IWORKSubDirStream::subStreamCount()
{
  // TODO: implement me
  return 0;
}

const char *IWORKSubDirStream::subStreamName(unsigned id)
{
  // TODO: implement me
  (void) id;
  return nullptr;
}

bool IWORKSubDirStream::existsSubStream(const char *name)
{
  const std::string path = m_dir + name;
  return m_parent->existsSubStream(path.c_str());
}

librevenge::RVNGInputStream *IWORKSubDirStream::getSubStreamByName(const char *name)
{
  const std::string path = m_dir + name;
  return m_parent->getSubStreamByName(path.c_str());
}

librevenge::RVNGInputStream *IWORKSubDirStream::getSubStreamById(unsigned id)
{
  // TODO: implement me
  (void) id;
  return nullptr;
}

const unsigned char *IWORKSubDirStream::read(const unsigned long, unsigned long &)
{
  return nullptr;
}

int IWORKSubDirStream::seek(long, const librevenge::RVNG_SEEK_TYPE)
{
  return 0;
}

long IWORKSubDirStream::tell()
{
  return 0;
}

bool IWORKSubDirStream::isEnd()
{
  return true;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
