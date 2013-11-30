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

#include "KEY2ParserUtils.h"
#include "KEYTable.h"
#include "KEYXMLReader.h"

namespace libetonyek
{

class KEYCollector;
class KEY2Parser;

class KEY2TableParser : public KEY2ParserUtils
{
public:
  explicit KEY2TableParser(KEY2Parser &parser);

  void parse(const KEYXMLReader &reader);

private:
  void parseTabularModel(const KEYXMLReader &reader);
  void parseGrid(const KEYXMLReader &reader);
  void parseColumns(const KEYXMLReader &reader);
  void parseGridColumn(const KEYXMLReader &reader);
  void parseRows(const KEYXMLReader &reader);
  void parseGridRow(const KEYXMLReader &reader);

  void parseDatasource(const KEYXMLReader &reader);
  void parseD(const KEYXMLReader &reader);
  void parseDu(const KEYXMLReader &reader);
  void parseF(const KEYXMLReader &reader);
  void parseG(const KEYXMLReader &reader);
  void parseN(const KEYXMLReader &reader);
  void parseS(const KEYXMLReader &reader);
  void parseT(const KEYXMLReader &reader);
  void parseCt(const KEYXMLReader &reader);
  void parseSo(const KEYXMLReader &reader);

  void parseCommonCellAttribute(const KEYXMLReader::AttributeIterator &attr);

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
