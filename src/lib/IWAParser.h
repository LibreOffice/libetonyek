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
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <boost/variant.hpp>

#include <mdds/flat_segment_tree.hpp>

#include "libetonyek_utils.h"
#include "IWAMessage.h"
#include "IWORKStyle_fwd.h"
#include "IWORKTypes.h"

namespace libetonyek
{

class IWORKCollector;
class IWORKPropertyMap;
class IWORKText;

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

  typedef std::map<unsigned, IWORKStylePtr_t> StyleMap_t;
  typedef void (IWAParser::*StyleParseFun_t)(unsigned, IWORKStylePtr_t &);

protected:
  static boost::optional<unsigned> readRef(const IWAMessage &msg, unsigned field);
  static std::deque<unsigned> readRefs(const IWAMessage &msg, unsigned field);
  static boost::optional<IWORKPosition> readPosition(const IWAMessage &msg, unsigned field);
  static boost::optional<IWORKSize> readSize(const IWAMessage &msg, unsigned field);
  static boost::optional<IWORKColor> readColor(const IWAMessage &msg, unsigned field);
  static void readStroke(const IWAMessage &msg, IWORKStroke &stroke);
  void readFill(const IWAMessage &msg, IWORKFill &fill);
  static void readGradient(const IWAMessage &msg, IWORKGradient &gradient);
  static void readShadow(const IWAMessage &msg, IWORKShadow &shadow);

  bool dispatchShape(unsigned id);
  bool parseText(unsigned id);
  void parseComment(unsigned id);

  bool parseShapePlacement(const IWAMessage &msg, IWORKGeometryPtr_t &geometry);

  const IWORKStylePtr_t queryCharacterStyle(unsigned id) const;
  const IWORKStylePtr_t queryParagraphStyle(unsigned id) const;
  const IWORKStylePtr_t queryGraphicStyle(unsigned id) const;
  const IWORKStylePtr_t queryCellStyle(unsigned id) const;

  const IWORKStylePtr_t queryStyle(unsigned id, StyleMap_t &styleMap, StyleParseFun_t parse) const;

protected:
  boost::shared_ptr<IWORKText> m_currentText;

private:
  typedef std::map<unsigned, boost::variant<std::string, unsigned> > DataList_t;

  struct TableHeader
  {
    TableHeader(unsigned count);

    mdds::flat_segment_tree<unsigned, float> m_sizes;
    mdds::flat_segment_tree<unsigned, bool> m_hidden;
  };

  struct TableInfo
  {
    TableInfo(unsigned columns, unsigned rows);

    const unsigned m_columns;
    const unsigned m_rows;

    TableHeader m_columnHeader;
    TableHeader m_rowHeader;

    DataList_t m_simpleTextList;
    DataList_t m_cellStyleList;
    DataList_t m_formattedTextList;
    DataList_t m_commentList;
  };

private:
  virtual bool parseDocument() = 0;

private:
  void queryObject(unsigned id, unsigned &type, boost::optional<IWAMessage> &msg) const;
  const RVNGInputStreamPtr_t queryFile(unsigned id) const;

  void parseObjectIndex();

  void scanFragment(unsigned id);
  void scanFragment(unsigned id, const RVNGInputStreamPtr_t &stream);

  void parseCharacterStyle(unsigned id, IWORKStylePtr_t &style);
  void parseParagraphStyle(unsigned id, IWORKStylePtr_t &style);
  void parseGraphicStyle(unsigned id, IWORKStylePtr_t &style);
  void parseCellStyle(unsigned id, IWORKStylePtr_t &style);

  void parseTabularModel(unsigned id);
  void parseDataList(unsigned id, DataList_t &dataList);
  void parseTile(unsigned id);
  void parseHeaders(unsigned id, TableHeader &header);

  bool parseDrawableShape(const IWAMessage &msg);
  bool parseGroup(const IWAMessage &msg);
  bool parseShapePlacement(const IWAMessage &msg);
  void parseCharacterProperties(const IWAMessage &msg, IWORKPropertyMap &props);
  bool parseImage(const IWAMessage &msg);
  bool parseTabularInfo(const IWAMessage &msg);

private:
  const RVNGInputStreamPtr_t m_fragments;
  const RVNGInputStreamPtr_t m_package;
  IWORKCollector &m_collector;

  FileMap_t m_fragmentMap;
  mutable RecordMap_t m_fragmentObjectMap;
  mutable FileMap_t m_fileMap;

  std::deque<unsigned> m_visited;

  mutable StyleMap_t m_charStyles;
  mutable StyleMap_t m_paraStyles;
  mutable StyleMap_t m_graphicStyles;
  mutable StyleMap_t m_cellStyles;

  boost::shared_ptr<TableInfo> m_currentTable;
};

}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
