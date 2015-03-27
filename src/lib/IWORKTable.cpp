/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKTable.h"

#include <boost/numeric/conversion/cast.hpp>

#include "libetonyek_utils.h"
#include "IWORKDocumentInterface.h"
#include "IWORKTransformation.h"
#include "IWORKTypes.h"

using boost::numeric_cast;

namespace libetonyek
{

IWORKTable::Cell::Cell()
  : m_content()
  , m_columnSpan(1)
  , m_rowSpan(1)
  , m_covered(false)
{
}

IWORKTable::IWORKTable()
  : m_table()
  , m_columnSizes()
  , m_rowSizes()
  , m_geometry()
{
}

void IWORKTable::setSizes(const ColumnSizes_t &columnSizes, const RowSizes_t &rowSizes)
{
  m_columnSizes = columnSizes;
  m_rowSizes = rowSizes;

  // init. content table of appropriate dimensions
  m_table = Table_t(m_rowSizes.size(), Row_t(m_columnSizes.size()));
}

void IWORKTable::insertCell(const unsigned column, const unsigned row, const IWORKOutputElements &content, const unsigned columnSpan, const unsigned rowSpan)
{
  if ((m_rowSizes.size() <= row) || (m_columnSizes.size() <= column))
    return;

  Cell cell;
  cell.m_content = content;
  cell.m_columnSpan = columnSpan;
  cell.m_rowSpan = rowSpan;
  m_table[row][column] = cell;
}

void IWORKTable::insertCoveredCell(const unsigned column, const unsigned row)
{
  if ((m_rowSizes.size() <= row) || (m_columnSizes.size() <= column))
    return;

  Cell cell;
  cell.m_covered = true;
  m_table[row][column] = cell;
}

void IWORKTable::setGeometry(const IWORKGeometryPtr_t &geometry)
{
  m_geometry = geometry;
}

void IWORKTable::draw(const IWORKTransformation &trafo, IWORKOutputElements &elements) const
{
  librevenge::RVNGPropertyList tableProps;
  tableProps.insert("table:align", "center");

  double x = 0;
  double y = 0;
  trafo(x, y);
  tableProps.insert("svg:x", pt2in(x));
  tableProps.insert("svg:y", pt2in(y));

  if (m_geometry)
  {
    double w = m_geometry->m_naturalSize.m_width;
    double h = m_geometry->m_naturalSize.m_height;

    trafo(w, h, true);

    tableProps.insert("svg:width", pt2in(w));
    tableProps.insert("svg:height", pt2in(h));
  }

  librevenge::RVNGPropertyListVector columnSizes;

  for (ColumnSizes_t::const_iterator it = m_columnSizes.begin(); m_columnSizes.end() != it; ++it)
  {
    librevenge::RVNGPropertyList column;
    column.insert("style:column-width", pt2in(*it));
    columnSizes.append(column);
  }
  tableProps.insert("librevenge:table-columns", columnSizes);

  elements.addOpenTable(tableProps);
  for (std::size_t r = 0; m_table.size() != r; ++r)
  {
    const Row_t &row = m_table[r];

    librevenge::RVNGPropertyList rowProps;
    rowProps.insert("style:row-height", pt2in(m_rowSizes[r]));

    elements.addOpenTableRow(rowProps);
    for (std::size_t c = 0; row.size() != c; ++c)
    {
      const Cell &cell = row[c];

      librevenge::RVNGPropertyList cellProps;
      cellProps.insert("librevenge:column", numeric_cast<int>(c));
      cellProps.insert("librevenge:row", numeric_cast<int>(r));
      cellProps.insert("fo:vertical-align", "middle");

      if (cell.m_covered)
      {
        elements.addInsertCoveredTableCell(cellProps);
      }
      else
      {
        if (1 < cell.m_columnSpan)
          cellProps.insert("table:number-columns-spanned", numeric_cast<int>(cell.m_columnSpan));
        if (1 < cell.m_rowSpan)
          cellProps.insert("table:number-rows-spanned", numeric_cast<int>(cell.m_rowSpan));

        elements.addOpenTableCell(cellProps);

        if (!cell.m_content.empty())
          elements.append(cell.m_content);

        elements.addCloseTableCell();
      }
    }
    elements.addCloseTableRow();
  }
  elements.addCloseTable();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
