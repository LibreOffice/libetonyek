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

#include <deque>
#include <memory>

#include <boost/optional.hpp>

#include "IWORKStyle_fwd.h"
#include "IWORKTypes.h"
#include "IWORKOutputElements.h"

namespace libetonyek
{

class IWORKLanguageManager;
class IWORKText;
class IWORKTableRecorder;

class IWORKTable
{
  struct Cell
  {
    IWORKOutputElements m_content;
    unsigned m_columnSpan;
    unsigned m_rowSpan;
    bool m_covered;
    IWORKFormulaPtr_t m_formula;
    boost::optional<unsigned> m_formulaHC;
    IWORKStylePtr_t m_style;
    IWORKCellType m_type;
    boost::optional<std::string> m_value;
    boost::optional<IWORKDateTimeData> m_dateTime;

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
  explicit IWORKTable(const IWORKTableNameMapPtr_t &tableNameMap, const IWORKLanguageManager &langManager);

  void setRecorder(const std::shared_ptr<IWORKTableRecorder> &recorder);
  const std::shared_ptr<IWORKTableRecorder> &getRecorder() const;

  void setName(std::string const &name);
  void setSize(unsigned columns, unsigned rows);
  void setHeaders(unsigned headerColumns, unsigned headerRows, unsigned footerRows);
  void setBandedRows(bool banded = true);
  void setRepeated(bool columns, bool rows);

  void setOrder(int order);
  void setStyle(const IWORKStylePtr_t &style);
  void setSizes(const IWORKColumnSizes_t &columnSizes, const IWORKRowSizes_t &rowSizes);
  void setBorders(const IWORKGridLineMap_t &verticalLines, const IWORKGridLineMap_t &horizontalLines);
  void setBorders(const IWORKGridLineMap_t &verticalLeftLines, const IWORKGridLineMap_t &verticalRightLines,
                  const IWORKGridLineMap_t &horizontalTopLines, const IWORKGridLineMap_t &horizontalBottomLines);
  void insertCell(unsigned column, unsigned row,
                  const boost::optional<std::string> &value = boost::none,
                  const std::shared_ptr<IWORKText> &text = std::shared_ptr<IWORKText>(),
                  const boost::optional<IWORKDateTimeData> &dateTime = boost::none,
                  unsigned columnSpan = 1, unsigned rowSpan = 1,
                  const IWORKFormulaPtr_t &formula = IWORKFormulaPtr_t(),
                  const boost::optional<unsigned> &formulaHC = boost::none,
                  const IWORKStylePtr_t &style = IWORKStylePtr_t(),
                  IWORKCellType type = IWORK_CELL_TYPE_TEXT);
  void insertCoveredCell(unsigned column, unsigned row);

  void draw(const librevenge::RVNGPropertyList &tableProps, IWORKOutputElements &elements, bool drawAsSimpleTable);

  void setDefaultCellStyle(CellType type, const IWORKStylePtr_t &style);
  void setDefaultLayoutStyle(CellType type, const IWORKStylePtr_t &style);
  void setDefaultParagraphStyle(CellType type, const IWORKStylePtr_t &style);

  boost::optional<int> getOrder() const;
  IWORKStylePtr_t getStyle() const;
  IWORKStylePtr_t getDefaultCellStyle(unsigned column, unsigned row) const;
  IWORKStylePtr_t getDefaultLayoutStyle(unsigned column, unsigned row) const;
  IWORKStylePtr_t getDefaultParagraphStyle(unsigned column, unsigned row) const;

private:
  IWORKStylePtr_t getDefaultStyle(unsigned column, unsigned row, const IWORKStylePtr_t *group) const;

  boost::optional<std::string> writeFormat(IWORKOutputElements &elements, const IWORKStylePtr_t &style, const IWORKCellType type, boost::optional<std::string> &rvngValueType);

private:
  const IWORKTableNameMapPtr_t m_tableNameMap;
  const IWORKLanguageManager &m_langManager;
  std::map<librevenge::RVNGString,std::string> m_formatNameMap;

  Table_t m_table;
  IWORKStylePtr_t m_style;
  boost::optional<std::string> m_name;
  boost::optional<int> m_order;
  IWORKColumnSizes_t m_columnSizes;
  IWORKRowSizes_t m_rowSizes;
  IWORKGridLineMap_t m_verticalLines;
  IWORKGridLineMap_t m_verticalRightLines; // if empty, m_verticalLines stores right/left line
  IWORKGridLineMap_t m_horizontalLines;
  IWORKGridLineMap_t m_horizontalBottomLines; // if empty, m_horizontalLines stores right/left line

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

  std::shared_ptr<IWORKTableRecorder> m_recorder;
};

}

#endif // IWORKTABLE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
