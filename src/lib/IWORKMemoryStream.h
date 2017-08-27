/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKMEMORYSTREAM_H_INCLUDED
#define IWORKMEMORYSTREAM_H_INCLUDED

#include <memory>
#include <vector>

#include "libetonyek_utils.h"

namespace libetonyek
{

class IWORKMemoryStream : public librevenge::RVNGInputStream
{
  // -Weffc++
  IWORKMemoryStream(const IWORKMemoryStream &other);
  IWORKMemoryStream &operator=(const IWORKMemoryStream &other);

public:
  explicit IWORKMemoryStream(const RVNGInputStreamPtr_t &input);
  IWORKMemoryStream(const RVNGInputStreamPtr_t &input, unsigned length);
  explicit IWORKMemoryStream(const std::vector<unsigned char> &data);
  IWORKMemoryStream(const unsigned char *data, unsigned length);
  ~IWORKMemoryStream() override;

  bool isStructured() override;
  unsigned subStreamCount() override;
  const char *subStreamName(unsigned id) override;
  bool existsSubStream(const char *) override
  {
    return false;
  }
  librevenge::RVNGInputStream *getSubStreamByName(const char *name) override;
  librevenge::RVNGInputStream *getSubStreamById(unsigned id) override;

  const unsigned char *read(unsigned long numBytes, unsigned long &numBytesRead) override;
  int seek(long offset, librevenge::RVNG_SEEK_TYPE seekType) override;
  long tell() override;
  bool isEnd() override;

private:
  void assign(const unsigned char *data, unsigned length);
  void read(const RVNGInputStreamPtr_t &input, unsigned length);

private:
  std::unique_ptr<unsigned char[]> m_data;
  long m_length;
  long m_pos;
};

}

#endif // IWORKMEMORYSTREAM_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
