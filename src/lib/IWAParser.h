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
#include "IWAMessage.h"

namespace libetonyek
{

class IWORKCollector;
struct IWORKPosition;
struct IWORKSize;

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
  class ObjectMessage
  {
  public:
    ObjectMessage(IWAParser &parser, unsigned id, unsigned type = 0);
    ~ObjectMessage();

    operator bool() const;
    const IWAMessage &get() const;

    unsigned getType() const;

    friend const IWAMessage &get(const ObjectMessage &msg)
    {
      return msg.get();
    }

  private:
    IWAParser &m_parser;
    boost::optional<IWAMessage> m_message;
    const unsigned m_id;
    unsigned m_type;
  };
  friend class ObjectMessage;

protected:
  static boost::optional<unsigned> readRef(const IWAMessage &msg, unsigned field);
  static std::deque<unsigned> readRefs(const IWAMessage &msg, unsigned field);
  static boost::optional<IWORKPosition> readPosition(const IWAMessage &msg, unsigned field);
  static boost::optional<IWORKSize> readSize(const IWAMessage &msg, unsigned field);

  bool dispatchShape(unsigned id);
  bool parseText(unsigned id);

  bool parseDrawableShape(const IWAMessage &msg);

private:
  virtual bool parseDocument() = 0;

private:
  void queryObject(unsigned id, unsigned &type, boost::optional<IWAMessage> &msg) const;

  void parseObjectIndex();

  void scanFragment(unsigned id);
  void scanFragment(unsigned id, const RVNGInputStreamPtr_t &stream);

  bool parseGroup(const IWAMessage &msg);
  bool parseShapePlacement(const IWAMessage &msg);

private:
  const RVNGInputStreamPtr_t m_fragments;
  const RVNGInputStreamPtr_t m_package;
  IWORKCollector &m_collector;

  FileMap_t m_fragmentMap;
  mutable RecordMap_t m_fragmentObjectMap;
  FileMap_t m_fileMap;

  std::deque<unsigned> m_visited;
};

}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
