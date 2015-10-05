/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWAPARSER_H_INCLUDED
#define IWAPARSER_H_INCLUDED

#include <deque>
#include <map>
#include <string>

#include <boost/optional.hpp>
#include <boost/unordered_map.hpp>

#include "libetonyek_utils.h"

namespace libetonyek
{

class IWAMessage;
class IWORKCollector;

class IWAParser
{
  // disable copying
  IWAParser(const IWAParser &);
  IWAParser &operator=(const IWAParser &);

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
  IWAParser(const RVNGInputStreamPtr_t &fragments, const RVNGInputStreamPtr_t &package, IWORKCollector &collector);

  bool parse();

protected:
  boost::optional<IWAMessage> queryObject(unsigned id, unsigned type = 0) const;

  static boost::optional<unsigned> readRef(const IWAMessage &msg, unsigned field);
  static std::deque<unsigned> readRefs(const IWAMessage &msg, unsigned field);

private:
  virtual bool parseDocument() = 0;

private:
  void parseObjectIndex();

  void scanFragment(unsigned id);
  void scanFragment(unsigned id, const RVNGInputStreamPtr_t &stream);

private:
  const RVNGInputStreamPtr_t m_fragments;
  const RVNGInputStreamPtr_t m_package;
  IWORKCollector &m_collector;

  FileMap_t m_fragmentMap;
  mutable RecordMap_t m_fragmentObjectMap;
  FileMap_t m_fileMap;
};

}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
