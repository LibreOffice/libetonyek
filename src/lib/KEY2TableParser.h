/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY2TABLEPARSER_H_INCLUDED
#define KEY2TABLEPARSER_H_INCLUDED

#include "IWORKXMLReader.h"
#include "KEY2ParserUtils.h"
#include "KEYTable.h"

namespace libetonyek
{

class KEYCollector;
class KEY2Parser;

class KEY2TableParser : public KEY2ParserUtils
{
public:
  explicit KEY2TableParser(KEY2Parser &parser);

  void parse(const IWORKXMLReader &reader);

private:
  void parseTabularModel(const IWORKXMLReader &reader);
  void parseGrid(const IWORKXMLReader &reader);
  void parseColumns(const IWORKXMLReader &reader);
  void parseGridColumn(const IWORKXMLReader &reader);
  void parseRows(const IWORKXMLReader &reader);
  void parseGridRow(const IWORKXMLReader &reader);

  void parseDatasource(const IWORKXMLReader &reader);
  void parseD(const IWORKXMLReader &reader);
  void parseDu(const IWORKXMLReader &reader);
  void parseF(const IWORKXMLReader &reader);
  void parseG(const IWORKXMLReader &reader);
  void parseN(const IWORKXMLReader &reader);
  void parseS(const IWORKXMLReader &reader);
  void parseT(const IWORKXMLReader &reader);
  void parseCt(const IWORKXMLReader &reader);
  void parseSo(const IWORKXMLReader &reader);

  void parseCommonCellAttribute(const IWORKXMLReader::AttributeIterator &attr);

  void emitCell(bool covered = false);

  KEYCollector *getCollector();

private:
  KEY2Parser &m_parser;
  KEYTable::ColumnSizes_t m_columnSizes;
  KEYTable::RowSizes_t m_rowSizes;

  unsigned m_column;
  unsigned m_row;

  boost::optional<unsigned> m_columnSpan;
  boost::optional<unsigned> m_rowSpan;
  boost::optional<unsigned> m_cellMove;
  boost::optional<std::string> m_content;
};

}

#endif //  KEY2TABLEPARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
