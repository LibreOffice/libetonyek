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

#include <glm/glm.hpp>

#include "libetonyek_utils.h"
#include "IWORKDocumentInterface.h"
#include "IWORKProperties.h"
#include "IWORKStyle.h"
#include "IWORKTypes.h"

using boost::numeric_cast;

namespace libetonyek
{

namespace
{

librevenge::RVNGString collectBorder(const IWORKGridLine_t &line, std::size_t index)
{
  IWORKStylePtr_t style;
  line.search_tree(index, style);
  if (style && style->has<property::SFTStrokeProperty>())
  {
    const IWORKStroke &stroke = style->get<property::SFTStrokeProperty>();
    return makeBorder(stroke);
  }
  return librevenge::RVNGString();
}

}

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
  , m_verticalLines()
  , m_horizontalLines()
{
}

void IWORKTable::setSizes(const IWORKColumnSizes_t &columnSizes, const IWORKRowSizes_t &rowSizes)
{
  m_columnSizes = columnSizes;
  m_rowSizes = rowSizes;

  // init. content table of appropriate dimensions
  m_table = Table_t(m_rowSizes.size(), Row_t(m_columnSizes.size()));
}

void IWORKTable::setBorders(const IWORKGridLineList_t &verticalLines, const IWORKGridLineList_t &horizontalLines)
{
  m_verticalLines = verticalLines;
  for (IWORKGridLineList_t::iterator it = m_verticalLines.begin(); m_verticalLines.end() != it; ++it)
    (*it).build_tree();

  m_horizontalLines = horizontalLines;
  for (IWORKGridLineList_t::iterator it = m_horizontalLines.begin(); m_horizontalLines.end() != it; ++it)
    (*it).build_tree();
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

void IWORKTable::draw(const librevenge::RVNGPropertyList &tableProps, IWORKOutputElements &elements) const
{
  librevenge::RVNGPropertyListVector columnSizes;

  for (IWORKColumnSizes_t::const_iterator it = m_columnSizes.begin(); m_columnSizes.end() != it; ++it)
  {
    librevenge::RVNGPropertyList column;
    column.insert("style:column-width", pt2in(*it));
    columnSizes.append(column);
  }

  librevenge::RVNGPropertyList allTableProps(tableProps);
  allTableProps.insert("librevenge:table-columns", columnSizes);

  elements.addOpenTable(allTableProps);
  for (std::size_t r = 0; m_table.size() != r; ++r)
  {
    const Row_t &row = m_table[r];

    librevenge::RVNGPropertyList rowProps;
    rowProps.insert("style:row-height", pt2in(m_rowSizes[r]));

    const IWORKGridLine_t &topLine = m_horizontalLines[r];
    const IWORKGridLine_t &bottomLine = m_horizontalLines[r+1];

    elements.addOpenTableRow(rowProps);
    for (std::size_t c = 0; row.size() != c; ++c)
    {
      const Cell &cell = row[c];

      librevenge::RVNGPropertyList cellProps;
      cellProps.insert("librevenge:column", numeric_cast<int>(c));
      cellProps.insert("librevenge:row", numeric_cast<int>(r));
      cellProps.insert("fo:vertical-align", "middle");

      librevenge::RVNGString border;

      border = collectBorder(topLine,c);
      if (border.len())
        cellProps.insert("fo:border-top", border);

      border = collectBorder(bottomLine,c);
      if (border.len())
        cellProps.insert("fo:border-bottom", border);

      const IWORKGridLine_t &leftLine = m_verticalLines[c];
      border = collectBorder(leftLine,r);
      if (border.len())
        cellProps.insert("fo:border-left", border);

      const IWORKGridLine_t &rightLine = m_verticalLines[c+1];
      border = collectBorder(rightLine,r);
      if (border.len())
        cellProps.insert("fo:border-right", border);

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
