/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <boost/numeric/conversion/cast.hpp>

#include <librevenge/librevenge.h>

#include "libetonyek_utils.h"
#include "IWORKTypes.h"
#include "KEYOutput.h"
#include "KEYTable.h"
#include "KEYTypes.h"

using boost::numeric_cast;

namespace libetonyek
{

KEYTable::Cell::Cell()
  : content()
  , columnSpan(1)
  , rowSpan(1)
  , covered(false)
{
}

KEYTable::KEYTable()
  : m_table()
  , m_columnSizes()
  , m_rowSizes()
  , m_geometry()
{
}

void KEYTable::setSizes(const ColumnSizes_t &columnSizes, const RowSizes_t &rowSizes)
{
  m_columnSizes = columnSizes;
  m_rowSizes = rowSizes;

  // init. content table of appropriate dimensions
  m_table = Table_t(m_rowSizes.size(), Row_t(m_columnSizes.size()));
}

void KEYTable::insertCell(const unsigned column, const unsigned row, const KEYObjectPtr_t &content, const unsigned columnSpan, const unsigned rowSpan)
{
  if ((m_rowSizes.size() <= row) || (m_columnSizes.size() <= column))
    return;

  Cell cell;
  cell.content = content;
  cell.columnSpan = columnSpan;
  cell.rowSpan = rowSpan;
  m_table[row][column] = cell;
}

void KEYTable::insertCoveredCell(const unsigned column, const unsigned row)
{
  if ((m_rowSizes.size() <= row) || (m_columnSizes.size() <= column))
    return;

  Cell cell;
  cell.covered = true;
  m_table[row][column] = cell;
}

void KEYTable::setGeometry(const IWORKGeometryPtr_t &geometry)
{
  m_geometry = geometry;
}

void KEYTable::draw(const KEYOutput &output) const
{
  librevenge::RVNGPresentationInterface *const painter = output.getPainter();

  librevenge::RVNGPropertyList tableProps;
  tableProps.insert("table:align", "center");

  if (m_geometry)
  {
    double x = m_geometry->position.x;
    double y = m_geometry->position.y;
    double w = m_geometry->naturalSize.width;
    double h = m_geometry->naturalSize.height;

    const IWORKTransformation tr = output.getTransformation();
    tr(x, y);
    tr(w, h, true);

    tableProps.insert("svg:x", pt2in(x));
    tableProps.insert("svg:y", pt2in(y));
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

  painter->startTableObject(tableProps);
  for (std::size_t r = 0; m_table.size() != r; ++r)
  {
    const Row_t &row = m_table[r];

    librevenge::RVNGPropertyList rowProps;
    rowProps.insert("style:row-height", pt2in(m_rowSizes[r]));

    painter->openTableRow(rowProps);
    for (std::size_t c = 0; row.size() != c; ++c)
    {
      const Cell &cell = row[c];

      librevenge::RVNGPropertyList cellProps;
      cellProps.insert("librevenge:column", numeric_cast<int>(c));
      cellProps.insert("librevenge:row", numeric_cast<int>(r));
      cellProps.insert("fo:vertical-align", "middle");

      if (cell.covered)
      {
        painter->insertCoveredTableCell(cellProps);
      }
      else
      {
        if (1 < cell.columnSpan)
          cellProps.insert("table:number-columns-spanned", numeric_cast<int>(cell.columnSpan));
        if (1 < cell.rowSpan)
          cellProps.insert("table:number-rows-spanned", numeric_cast<int>(cell.rowSpan));

        painter->openTableCell(cellProps);
        if (bool(cell.content))
          cell.content->draw(output);
        painter->closeTableCell();
      }
    }
    painter->closeTableRow();
  }
  painter->endTableObject();
}

namespace
{

class TableObject : public KEYObject
{
public:
  explicit TableObject(const KEYTable &table);

  virtual void draw(const KEYOutput &output);

private:
  const KEYTable m_table;
};

TableObject::TableObject(const KEYTable &table)
  : m_table(table)
{
}

void TableObject::draw(const KEYOutput &output)
{
  m_table.draw(output);
}

}

KEYObjectPtr_t makeObject(const KEYTable &table)
{
  const KEYObjectPtr_t object(new TableObject(table));
  return object;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
