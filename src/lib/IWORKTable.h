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

    Cell();
  };

  typedef std::deque<Cell> Row_t;
  typedef std::deque<Row_t> Table_t;

public:
  IWORKTable();

  void setSizes(const IWORKColumnSizes_t &columnSizes, const IWORKRowSizes_t &rowSizes);
  void setBorders(const IWORKGridLineList_t &verticalLines, const IWORKGridLineList_t &horizontalLines);
  void setTableNameMap(const IWORKTableNameMapPtr_t &tableNameMap);
  void insertCell(unsigned column, unsigned row, const IWORKOutputElements &content = IWORKOutputElements(), unsigned columnSpan = 1, unsigned rowSpan = 1, const boost::optional<IWORKFormula> &formula = boost::none, const IWORKStylePtr_t &style = IWORKStylePtr_t(), IWORKCellType type = IWORK_CELL_TYPE_TEXT);
  void insertCoveredCell(unsigned column, unsigned row);
  void insertObject(const IWORKOutputElements &elements);

  void draw(const librevenge::RVNGPropertyList &tableProps, IWORKOutputElements &elements);

private:
  Table_t m_table;
  IWORKColumnSizes_t m_columnSizes;
  IWORKRowSizes_t m_rowSizes;
  IWORKGridLineList_t m_verticalLines;
  IWORKGridLineList_t m_horizontalLines;
  IWORKTableNameMapPtr_t m_tableNameMap;
  IWORKOutputElements m_mediaObjects;
};

}

#endif // IWORKTABLE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
