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
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

#include <boost/optional.hpp>
#include <boost/variant.hpp>

#include <mdds/flat_segment_tree.hpp>

#include "libetonyek_utils.h"
#include "IWAMessage.h"
#include "IWAObjectIndex.h"
#include "IWORKLanguageManager.h"
#include "IWORKOutputElements.h"
#include "IWORKStyle_fwd.h"
#include "IWORKTypes.h"

namespace libetonyek
{

class IWORKCollector;
class IWORKPropertyMap;
class IWORKTable;
class IWORKText;

class IWAParser
{
  // disable copying
  IWAParser(const IWAParser &);
  IWAParser &operator=(const IWAParser &);
public:
  IWAParser(const RVNGInputStreamPtr_t &fragments, const RVNGInputStreamPtr_t &package, IWORKCollector &collector);
  virtual ~IWAParser()
  {
  }

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
  typedef std::function<void(unsigned, IWORKStylePtr_t &)> StyleParseFun_t;

protected:
  static boost::optional<unsigned> readRef(const IWAMessage &msg, unsigned field);
  static std::deque<unsigned> readRefs(const IWAMessage &msg, unsigned field);
  static boost::optional<IWORKPosition> readPosition(const IWAMessage &msg, unsigned field);
  static boost::optional<IWORKSize> readSize(const IWAMessage &msg, unsigned field);
  static boost::optional<IWORKColor> readColor(const IWAMessage &msg, unsigned field);
  static void readStroke(const IWAMessage &msg, IWORKStroke &stroke);
  bool readFill(const IWAMessage &msg, IWORKFill &fill);
  static void readGradient(const IWAMessage &msg, IWORKGradient &gradient);
  static void readShadow(const IWAMessage &msg, IWORKShadow &shadow);
  static void readPadding(const IWAMessage &msg, IWORKPadding &padding);

  bool dispatchShape(unsigned id);
  bool parseText(unsigned id);
  void parseComment(unsigned id);

  bool parseShapePlacement(const IWAMessage &msg, IWORKGeometryPtr_t &geometry);

  const IWORKStylePtr_t queryCharacterStyle(unsigned id) const;
  const IWORKStylePtr_t queryParagraphStyle(unsigned id) const;
  const IWORKStylePtr_t querySectionStyle(unsigned id) const;

  const IWORKStylePtr_t queryGraphicStyle(unsigned id) const;
  const IWORKStylePtr_t queryCellStyle(unsigned id) const;
  const IWORKStylePtr_t queryTableStyle(unsigned id) const;
  const IWORKStylePtr_t queryListStyle(unsigned id) const;

  const IWORKStylePtr_t queryStyle(unsigned id, StyleMap_t &styleMap, StyleParseFun_t parse) const;

protected:
  IWORKLanguageManager m_langManager;
  std::shared_ptr<IWORKText> m_currentText;

private:
  typedef std::map<unsigned, boost::variant<std::string, unsigned> > DataList_t;

  struct PageMaster
  {
    PageMaster();
    IWORKStylePtr_t m_style;
    bool m_headerFootersSameAsPrevious;
  };

  struct TableHeader
  {
    TableHeader(unsigned count);

    mdds::flat_segment_tree<unsigned, float> m_sizes;
    mdds::flat_segment_tree<unsigned, bool> m_hidden;
  };

  struct TableInfo
  {
    TableInfo(const std::shared_ptr<IWORKTable> &table, unsigned columns, unsigned rows);

    std::shared_ptr<IWORKTable> m_table;

    const unsigned m_columns;
    const unsigned m_rows;

    IWORKStylePtr_t m_style;

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

  void parseCharacterStyle(unsigned id, IWORKStylePtr_t &style);
  void parseParagraphStyle(unsigned id, IWORKStylePtr_t &style);
  void parseSectionStyle(unsigned id, IWORKStylePtr_t &style);

  void parseGraphicStyle(unsigned id, IWORKStylePtr_t &style);
  void parseCellStyle(unsigned id, IWORKStylePtr_t &style);
  void parseTableStyle(unsigned id, IWORKStylePtr_t &style);
  void parseListStyle(unsigned id, IWORKStylePtr_t &style);

  void parseHeaderAndFooter(unsigned id, IWORKPageMaster &hf);
  void parsePageMaster(unsigned id, PageMaster &pageMaster);

  void parseTabularModel(unsigned id);
  void parseDataList(unsigned id, DataList_t &dataList);
  void parseTile(unsigned id);
  void parseHeaders(unsigned id, TableHeader &header);

  void parseLink(unsigned id, std::string &url);

  bool parseDrawableShape(const IWAMessage &msg);
  bool parseGroup(const IWAMessage &msg);
  bool parseShapePlacement(const IWAMessage &msg);
  void parseCharacterProperties(const IWAMessage &msg, IWORKPropertyMap &props);
  void parseColumnsProperties(const IWAMessage &msg, IWORKPropertyMap &props);
  bool parseImage(const IWAMessage &msg);
  bool parseTabularInfo(const IWAMessage &msg);

private:
  IWORKCollector &m_collector;

  IWAObjectIndex m_index;

  std::deque<unsigned> m_visited;

  mutable StyleMap_t m_charStyles;
  mutable StyleMap_t m_paraStyles;
  mutable StyleMap_t m_sectionStyles;

  mutable StyleMap_t m_graphicStyles;
  mutable StyleMap_t m_cellStyles;
  mutable StyleMap_t m_tableStyles;
  mutable StyleMap_t m_listStyles;

  IWORKTableNameMapPtr_t m_tableNameMap;
  std::shared_ptr<TableInfo> m_currentTable;
};

}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
