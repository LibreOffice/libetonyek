/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKTABLE_H_INCLUDED
#define IWORKTABLE_H_INCLUDED

#include <boost/optional.hpp>

#include <deque>

#include "IWORKStyle_fwd.h"
#include "IWORKTypes.h"
#include "IWORKOutputElements.h"

namespace libetonyek
{

class IWORKTable
{
  struct Cell
  {
    IWORKOutputElements m_content;
    unsigned m_columnSpan;
    unsigned m_rowSpan;
    bool m_covered;
    boost::optional<IWORKFormula> m_formula;
    IWORKStylePtr_t m_style;
    IWORKCellType m_type;
    boost::optional<std::string> m_value;

    Cell();
  };

  typedef std::deque<Cell> Row_t;
  typedef std::deque<Row_t> Table_t;

public:
  enum CellType
  {
    CELL_TYPE_BODY,
    CELL_TYPE_ALTERNATE_BODY,
    CELL_TYPE_COLUMN_HEADER,
    CELL_TYPE_ROW_HEADER,
    CELL_TYPE_ROW_FOOTER
  };

public:
  explicit IWORKTable(const IWORKTableNameMapPtr_t &tableNameMap);

  void setSize(unsigned columns, unsigned rows);
  void setHeaders(unsigned headerColumns, unsigned headerRows, unsigned footerRows);
  void setBandedRows(bool banded = true);
  void setRepeated(bool columns, bool rows);

  void setStyle(const IWORKStylePtr_t &style);
  void setSizes(const IWORKColumnSizes_t &columnSizes, const IWORKRowSizes_t &rowSizes);
  void setBorders(const IWORKGridLineList_t &verticalLines, const IWORKGridLineList_t &horizontalLines);
  void insertCell(unsigned column, unsigned row, const boost::optional<std::string> &value = boost::none, const IWORKOutputElements &content = IWORKOutputElements(), unsigned columnSpan = 1, unsigned rowSpan = 1, const boost::optional<IWORKFormula> &formula = boost::none, const IWORKStylePtr_t &style = IWORKStylePtr_t(), IWORKCellType type = IWORK_CELL_TYPE_TEXT);
  void insertCoveredCell(unsigned column, unsigned row);

  void draw(const librevenge::RVNGPropertyList &tableProps, IWORKOutputElements &elements);

  void setDefaultCellStyle(CellType type, const IWORKStylePtr_t &style);
  void setDefaultLayoutStyle(CellType type, const IWORKStylePtr_t &style);
  void setDefaultParagraphStyle(CellType type, const IWORKStylePtr_t &style);

  IWORKStylePtr_t getDefaultCellStyle(unsigned column, unsigned row) const;
  IWORKStylePtr_t getDefaultLayoutStyle(unsigned column, unsigned row) const;
  IWORKStylePtr_t getDefaultParagraphStyle(unsigned column, unsigned row) const;

private:
  IWORKStylePtr_t getDefaultStyle(unsigned column, unsigned row, const IWORKStylePtr_t *group) const;

private:
  const IWORKTableNameMapPtr_t m_tableNameMap;

  Table_t m_table;
  IWORKStylePtr_t m_style;
  IWORKColumnSizes_t m_columnSizes;
  IWORKRowSizes_t m_rowSizes;
  IWORKGridLineList_t m_verticalLines;
  IWORKGridLineList_t m_horizontalLines;

  unsigned m_rows;
  unsigned m_columns;
  unsigned m_headerRows;
  unsigned m_footerRows;
  unsigned m_headerColumns;
  bool m_bandedRows;
  bool m_headerRowsRepeated;
  bool m_headerColumnsRepeated;

  IWORKStylePtr_t m_defaultCellStyles[5];
  IWORKStylePtr_t m_defaultLayoutStyles[5];
  IWORKStylePtr_t m_defaultParaStyles[5];
};

}

#endif // IWORKTABLE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
