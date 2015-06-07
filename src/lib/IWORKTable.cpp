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
  m_horizontalLines = horizontalLines;
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

      IWORKStylePtr_t styleTop;
      topLine.search(c, styleTop);
      if (styleTop)
      {
        librevenge::RVNGString borderTop;
        if (styleTop->has<property::SFTStrokeProperty>())
        {
          const IWORKStroke &stroke = styleTop->get<property::SFTStrokeProperty>();
          borderTop.sprintf("%fpt", stroke.m_width);

          if (stroke.m_pattern.size() >= 2)
          {
            const double x = stroke.m_pattern[0];
            const double y = stroke.m_pattern[1];
            if (((x / y) < 0.01) || ((y / x) < 0.01))
              borderTop.append(" dotted");
            else
              borderTop.append(" dashed");
          }
          else
            borderTop.append(" solid");

          borderTop.append(" ");
          borderTop.append(makeColor(stroke.m_color));
        }

        cellProps.insert("fo:border-top", borderTop);
      }

      IWORKStylePtr_t styleBottom;
      bottomLine.search_tree(c, styleBottom);

      const IWORKGridLine_t &leftLine = m_verticalLines[c];
      IWORKStylePtr_t styleLeft;
      leftLine.search_tree(r, styleLeft);

      const IWORKGridLine_t &rightLine = m_verticalLines[c+1];
      IWORKStylePtr_t styleRight;
      rightLine.search_tree(r, styleRight);

      // if (styleTop.has<ParagraphBorderType>())
      // {
      //  librevenge::RVNGString border;
      //  border.append("solid");
      //  cellProps.insert("fo:border-top", border);
      // }

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
