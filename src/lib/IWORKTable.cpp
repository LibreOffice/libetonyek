/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKTable.h"

#include <ctime>

#include <boost/numeric/conversion/cast.hpp>

#include <glm/glm.hpp>

#include "libetonyek_xml.h"
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

void writeBorder(librevenge::RVNGPropertyList &props, const char *name, IWORKGridLine_t &line, std::size_t index)
{
  if (!line.is_tree_valid())
    line.build_tree();

  IWORKStylePtr_t style;
  line.search_tree(index, style);
  if (style && style->has<property::SFTStrokeProperty>())
  {
    const IWORKStroke &stroke = style->get<property::SFTStrokeProperty>();
    props.insert(name, makeBorder(stroke));
  }
}

void writeCellFormat(librevenge::RVNGPropertyList &props, const IWORKStylePtr_t &style, const IWORKCellType type, const boost::optional<std::string> &value)
{
  using namespace property;

  switch (type)
  {
  case IWORK_CELL_TYPE_NUMBER :
    if (style->has<SFTCellStylePropertyNumberFormat>() && value)
    {
      const IWORKNumberFormat &format = style->get<SFTCellStylePropertyNumberFormat>();

      if (style->getIdent())
        props.insert("librevenge:name", get(style->getIdent()).c_str());

      props.insert("librevenge:value", get(value).c_str());

      switch (format.m_type)
      {
      case IWORK_CELL_NUMBER_TYPE_CURRENCY :
        props.insert("librevenge:value-type", "currency");
        props.insert("number:currency-style", format.m_currencyCode.c_str());
        break;
      case IWORK_CELL_NUMBER_TYPE_PERCENTAGE :
        props.insert("librevenge:value-type", "percentage");
        props.insert("number:decimal-places", format.m_decimalPlaces);
        break;
      case IWORK_CELL_NUMBER_TYPE_SCIENTIFIC :
        props.insert("librevenge:value-type", "scientific");
        props.insert("number:decimal-places", format.m_decimalPlaces);
        break;
      case IWORK_CELL_NUMBER_TYPE_DOUBLE :
        props.insert("librevenge:value-type", "double");
        props.insert("number:decimal-places", format.m_decimalPlaces);
        break;
      }
    }
    break;
  case IWORK_CELL_TYPE_DATE_TIME :
    if (style->has<SFTCellStylePropertyDateTimeFormat>() && value)
    {
      if (style->getIdent())
        props.insert("librevenge:name", get(style->getIdent()).c_str());
      props.insert("librevenge:value-type", "date");

      const double seconds = double_cast(get(value).c_str());
      const std::time_t t = ETONYEK_EPOCH_BEGIN + seconds;
      struct tm *const time = gmtime(&t);

      props.insert("librevenge:day", time->tm_mday);
      props.insert("librevenge:month", time->tm_mon + 1);
      props.insert("librevenge:year", time->tm_year + 1900);
      props.insert("librevenge:hours", time->tm_hour);
      props.insert("librevenge:minutes", time->tm_min);
      props.insert("librevenge:seconds", time->tm_sec);

    }
    break;
  case IWORK_CELL_TYPE_DURATION :
    if (style->has<SFTCellStylePropertyDurationFormat>() && value)
    {
      // TODO: How to insert format and convert?
      // const IWORKDurationFormat &format = style->get<SFTCellStylePropertyDurationFormat>();
      // props.insert("librevenge:format",format.m_format.c_str());

      const int seconds = int_cast(get(value).c_str());
      props.insert("librevenge:value-type", "time");
      props.insert("librevenge:hours", seconds / 3600);
      props.insert("librevenge:minutes", (seconds % 3600) / 60);
      props.insert("librevenge:seconds", (seconds % 3600) % 60);

      if (style->getIdent())
        props.insert("librevenge:name", get(style->getIdent()).c_str());
    }
    break;
  case IWORK_CELL_TYPE_BOOL:
    if (value)
    {
      props.insert("librevenge:value", get(value).c_str());
      props.insert("librevenge:value-type", "bool");
    }
    break;
  case IWORK_CELL_TYPE_TEXT :
  default:
    //TODO: librevenge:name ?
    if (value)
      props.insert("librevenge:value-type", "string");
    break;
  }
}

}

IWORKTable::Cell::Cell()
  : m_content()
  , m_columnSpan(1)
  , m_rowSpan(1)
  , m_covered(false)
  , m_formula()
  , m_style()
  , m_type(IWORK_CELL_TYPE_TEXT)
  , m_value()
{
}

IWORKTable::IWORKTable()
  : m_table()
  , m_columnSizes()
  , m_rowSizes()
  , m_verticalLines()
  , m_horizontalLines()
  , m_tableNameMap()
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

void IWORKTable::setTableNameMap(const IWORKTableNameMapPtr_t &tableNameMap)
{
  m_tableNameMap = tableNameMap;
}

void IWORKTable::insertCell(const unsigned column, const unsigned row, const boost::optional<std::string> &value, const IWORKOutputElements &content, const unsigned columnSpan, const unsigned rowSpan, const boost::optional<IWORKFormula> &formula, const IWORKStylePtr_t &style, const IWORKCellType type)
{
  if ((m_rowSizes.size() <= row) || (m_columnSizes.size() <= column))
    return;

  Cell cell;
  cell.m_content = content;
  cell.m_columnSpan = columnSpan;
  cell.m_rowSpan = rowSpan;
  cell.m_formula = formula;
  cell.m_style = style;
  cell.m_type = type;
  cell.m_value = value;
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

void IWORKTable::draw(const librevenge::RVNGPropertyList &tableProps, IWORKOutputElements &elements)
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

    elements.addOpenTableRow(rowProps);
    for (std::size_t c = 0; row.size() != c; ++c)
    {
      const Cell &cell = row[c];

      librevenge::RVNGPropertyList cellProps;
      cellProps.insert("librevenge:column", numeric_cast<int>(c));
      cellProps.insert("librevenge:row", numeric_cast<int>(r));
      cellProps.insert("fo:vertical-align", "middle");

      if (r < m_horizontalLines.size())
        writeBorder(cellProps, "fo:border-top", m_horizontalLines[r], c);
      if (r+1 < m_horizontalLines.size())
        writeBorder(cellProps, "fo:border-bottom", m_horizontalLines[r+1], c);
      if (c < m_verticalLines.size())
        writeBorder(cellProps, "fo:border-left", m_verticalLines[c], r);
      if (c+1 < m_verticalLines.size())
        writeBorder(cellProps, "fo:border-right", m_verticalLines[c+1], r);

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

        if (cell.m_style)
          writeCellFormat(cellProps, cell.m_style, cell.m_type, cell.m_value);

        if (cell.m_formula)
          elements.addOpenFormulaCell(cellProps, get(cell.m_formula), m_tableNameMap);
        else
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
