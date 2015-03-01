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

#include <librevenge/librevenge.h>

#include "libetonyek_utils.h"
#include "IWORKTransformation.h"
#include "IWORKTypes.h"

using boost::numeric_cast;

namespace libetonyek
{

IWORKTable::Cell::Cell()
  : content()
  , columnSpan(1)
  , rowSpan(1)
  , covered(false)
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

void IWORKTable::insertCell(const unsigned column, const unsigned row, const IWORKObjectPtr_t &content, const unsigned columnSpan, const unsigned rowSpan)
{
  if ((m_rowSizes.size() <= row) || (m_columnSizes.size() <= column))
    return;

  Cell cell;
  cell.content = content;
  cell.columnSpan = columnSpan;
  cell.rowSpan = rowSpan;
  m_table[row][column] = cell;
}

void IWORKTable::insertCoveredCell(const unsigned column, const unsigned row)
{
  if ((m_rowSizes.size() <= row) || (m_columnSizes.size() <= column))
    return;

  Cell cell;
  cell.covered = true;
  m_table[row][column] = cell;
}

void IWORKTable::setGeometry(const IWORKGeometryPtr_t &geometry)
{
  m_geometry = geometry;
}

void IWORKTable::draw(librevenge::RVNGPresentationInterface *const painter, const IWORKTransformation &trafo) const
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
    double w = m_geometry->naturalSize.width;
    double h = m_geometry->naturalSize.height;

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
          cell.content->draw(painter);
        painter->closeTableCell();
      }
    }
    painter->closeTableRow();
  }
  painter->endTableObject();
}

namespace
{

class TableObject : public IWORKObject
{
public:
  TableObject(const IWORKTable &table, const IWORKTransformation &trafo);

  virtual void draw(librevenge::RVNGPresentationInterface *painter);

private:
  const IWORKTable m_table;
  const IWORKTransformation m_trafo;
};

TableObject::TableObject(const IWORKTable &table, const IWORKTransformation &trafo)
  : m_table(table)
  , m_trafo(trafo)
{
}

void TableObject::draw(librevenge::RVNGPresentationInterface *const painter)
{
  m_table.draw(painter, m_trafo);
}

}

IWORKObjectPtr_t makeObject(const IWORKTable &table, const IWORKTransformation &trafo)
{
  const IWORKObjectPtr_t object(new TableObject(table, trafo));
  return object;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */