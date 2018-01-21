/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWAOBJECTINDEX_H_INCLUDED
#define IWAOBJECTINDEX_H_INCLUDED

#include <map>
#include <string>
#include <utility>

#include <boost/optional.hpp>

#include "libetonyek_utils.h"

namespace libetonyek
{

class IWAMessage;

class IWAObjectIndex
{
public:
  struct ObjectRecord
  {
    ObjectRecord();
    ObjectRecord(const RVNGInputStreamPtr_t &stream, unsigned type, long pos, unsigned long headerLen, unsigned long dataLen);

    RVNGInputStreamPtr_t m_stream;
    unsigned m_type;
    std::pair<long, long> m_headerRange;
    std::pair<long, long> m_dataRange;
  };

  typedef std::map<unsigned, std::pair<std::string, RVNGInputStreamPtr_t> > FileMap_t;
  typedef std::map<unsigned, std::pair<unsigned, ObjectRecord> > RecordMap_t;

public:
  IWAObjectIndex(const RVNGInputStreamPtr_t &fragments, const RVNGInputStreamPtr_t &package);

  void parse();

  void queryObject(unsigned id, unsigned &type, boost::optional<IWAMessage> &msg) const;
  const RVNGInputStreamPtr_t queryFile(unsigned id) const;

private:
  void scanFragment(unsigned id);
  void scanFragment(unsigned id, const RVNGInputStreamPtr_t &stream);

private:
  const RVNGInputStreamPtr_t m_fragments;
  const RVNGInputStreamPtr_t m_package;

  FileMap_t m_fragmentMap;
  mutable RecordMap_t m_fragmentObjectMap;
  mutable FileMap_t m_fileMap;
};

}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
