/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKTable.h"

#include <cassert>
#include <ctime>

#include <boost/numeric/conversion/cast.hpp>

#include <glm/glm.hpp>

#include "libetonyek_xml.h"
#include "libetonyek_utils.h"
#include "IWORKDocumentInterface.h"
#include "IWORKProperties.h"
#include "IWORKStyle.h"
#include "IWORKStyleStack.h"
#include "IWORKTableRecorder.h"
#include "IWORKText.h"
#include "IWORKTypes.h"

using boost::none;
using boost::numeric_cast;
using boost::optional;

using std::string;

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
    writeBorder(style->get<property::SFTStrokeProperty>(), name, props);
}

void writeCellFormat(librevenge::RVNGPropertyList &props, const IWORKStyleStack &style, const IWORKCellType type, const optional<string> &styleName, const boost::optional<std::string> &value)
{
  using namespace property;

  switch (type)
  {
  case IWORK_CELL_TYPE_NUMBER :
    if (style.has<SFTCellStylePropertyNumberFormat>() && value)
    {
      const IWORKNumberFormat &format = style.get<SFTCellStylePropertyNumberFormat>();

      // TODO: what's this anyway?
      if (styleName)
        props.insert("librevenge:name", get(styleName).c_str());

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
    if (style.has<SFTCellStylePropertyDateTimeFormat>() && value)
    {
      if (styleName)
        props.insert("librevenge:name", get(styleName).c_str());
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
    if (style.has<SFTCellStylePropertyDurationFormat>() && value)
    {
      // TODO: How to insert format and convert?
      // const IWORKDurationFormat &format = style.get<SFTCellStylePropertyDurationFormat>();
      // props.insert("librevenge:format",format.m_format.c_str());

      const int seconds = int_cast(get(value).c_str());
      props.insert("librevenge:value-type", "time");
      props.insert("librevenge:hours", seconds / 3600);
      props.insert("librevenge:minutes", (seconds % 3600) / 60);
      props.insert("librevenge:seconds", (seconds % 3600) % 60);

      if (styleName)
        props.insert("librevenge:name", get(styleName).c_str());
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

void writeCellStyle(librevenge::RVNGPropertyList &props, const IWORKStyleStack &style)
{
  using namespace property;

  IWORKVerticalAlignment vertAlign(IWORK_VERTICAL_ALIGNMENT_MIDDLE);
  if (style.has<VerticalAlignment>())
    vertAlign = style.get<VerticalAlignment>();
  switch (vertAlign)
  {
  case IWORK_VERTICAL_ALIGNMENT_TOP :
    props.insert("fo:vertical-align", "top");
    break;
  case IWORK_VERTICAL_ALIGNMENT_MIDDLE :
    props.insert("fo:vertical-align", "middle");
    break;
  case IWORK_VERTICAL_ALIGNMENT_BOTTOM :
    props.insert("fo:vertical-align", "bottom");
    break;
  }

  if (style.has<TopBorder>())
    writeBorder(style.get<TopBorder>(), "fo:border-top", props);
  if (style.has<BottomBorder>())
    writeBorder(style.get<BottomBorder>(), "fo:border-bottom", props);
  if (style.has<LeftBorder>())
    writeBorder(style.get<LeftBorder>(), "fo:border-left", props);
  if (style.has<RightBorder>())
    writeBorder(style.get<RightBorder>(), "fo:border-right", props);

  if (style.has<Fill>())
  {
    // TODO: add support for style:background-image to libodfgen
    double opacity=style.has<Opacity>() ? style.get<Opacity>() : 1.;
    if (const IWORKColor *const color = boost::get<IWORKColor>(&style.get<Fill>()))
    {
      props.insert("fo:background-color", makeColor(*color));
      opacity *= color->m_alpha;
    }
    else if (const IWORKGradient *const gradient = boost::get<IWORKGradient>(&style.get<Fill>()))
      props.insert("fo:background-color", makeColor(*gradient));
    if (opacity<1)
      props.insert("draw:opacity", opacity, librevenge::RVNG_PERCENT);
  }

  if (style.has<Padding>())
  {
    const IWORKPadding &padding = style.get<Padding>();
    if (padding.m_left)
      props.insert("fo:padding-left", get(padding.m_left));
    if (padding.m_right)
      props.insert("fo:padding-right", get(padding.m_right));
    if (padding.m_top)
      props.insert("fo:padding-top", get(padding.m_top));
    if (padding.m_bottom)
      props.insert("fo:padding-bottom", get(padding.m_bottom));
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

IWORKTable::IWORKTable(const IWORKTableNameMapPtr_t &tableNameMap, const IWORKLanguageManager &langManager)
  : m_tableNameMap(tableNameMap)
  , m_langManager(langManager)
  , m_table()
  , m_columnSizes()
  , m_rowSizes()
  , m_verticalLines()
  , m_horizontalLines()
  , m_rows(0)
  , m_columns(0)
  , m_headerRows(0)
  , m_footerRows(0)
  , m_headerColumns(0)
  , m_bandedRows(false)
  , m_headerRowsRepeated(false)
  , m_headerColumnsRepeated(false)
  , m_recorder()
{
}

void IWORKTable::setRecorder(const boost::shared_ptr<IWORKTableRecorder> &recorder)
{
  m_recorder = recorder;
}

const boost::shared_ptr<IWORKTableRecorder> &IWORKTable::getRecorder() const
{
  return m_recorder;
}

void IWORKTable::setSize(const unsigned columns, const unsigned rows)
{
  if (bool(m_recorder))
  {
    m_recorder->setSize(columns, rows);
    return;
  }

  m_columns = columns;
  m_rows = rows;
}

void IWORKTable::setHeaders(const unsigned headerColumns, const unsigned headerRows, const unsigned footerRows)
{
  if (bool(m_recorder))
  {
    m_recorder->setHeaders(headerColumns, headerRows, footerRows);
    return;
  }

  m_headerColumns = headerColumns;
  m_headerRows = headerRows;
  m_footerRows = footerRows;
}

void IWORKTable::setBandedRows(const bool banded)
{
  if (bool(m_recorder))
  {
    m_recorder->setBandedRows(banded);
    return;
  }

  m_bandedRows = banded;
}

void IWORKTable::setRepeated(const bool columns, const bool rows)
{
  if (bool(m_recorder))
  {
    m_recorder->setRepeated(columns, rows);
    return;
  }

  m_headerColumnsRepeated = columns;
  m_headerRowsRepeated = rows;
}

void IWORKTable::setStyle(const IWORKStylePtr_t &style)
{
  if (bool(m_recorder))
  {
    m_recorder->setStyle(style);
    return;
  }

  m_style = style;
}

void IWORKTable::setSizes(const IWORKColumnSizes_t &columnSizes, const IWORKRowSizes_t &rowSizes)
{
  if (bool(m_recorder))
  {
    m_recorder->setSizes(columnSizes, rowSizes);
    return;
  }

  m_columnSizes = columnSizes;
  m_rowSizes = rowSizes;

  // init. content table of appropriate dimensions
  m_table = Table_t(m_rowSizes.size(), Row_t(m_columnSizes.size()));
}

void IWORKTable::setBorders(const IWORKGridLineMap_t &verticalLines, const IWORKGridLineMap_t &horizontalLines)
{
  if (bool(m_recorder))
  {
    m_recorder->setBorders(verticalLines, horizontalLines);
    return;
  }

  m_verticalLines = verticalLines;
  m_horizontalLines = horizontalLines;
}

void IWORKTable::insertCell(const unsigned column, const unsigned row, const boost::optional<std::string> &value, const boost::shared_ptr<IWORKText> &text, const unsigned columnSpan, const unsigned rowSpan, const boost::optional<IWORKFormula> &formula, const IWORKStylePtr_t &style, const IWORKCellType type)
{
  if (bool(m_recorder))
  {
    m_recorder->insertCell(column, row, value, text, columnSpan, rowSpan, formula, style, type);
    return;
  }

  if ((m_rowSizes.size() <= row) || (m_columnSizes.size() <= column))
    return;

  Cell cell;
  if (bool(text))
  {
    IWORKStyleStack fStyle;
    fStyle.push(getDefaultCellStyle(column, row));
    fStyle.push(style);
    using namespace property;
    if (fStyle.has<SFTCellStylePropertyParagraphStyle>())
      text->pushBaseParagraphStyle(fStyle.get<SFTCellStylePropertyParagraphStyle>());
    else
      text->pushBaseParagraphStyle(getDefaultParagraphStyle(column,row));
    if (fStyle.has<SFTCellStylePropertyLayoutStyle>())
      text->pushBaseLayoutStyle(fStyle.get<SFTCellStylePropertyLayoutStyle>());
    else
      text->pushBaseLayoutStyle(getDefaultLayoutStyle(column,row));
    text->draw(cell.m_content);
  }
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
  if (bool(m_recorder))
  {
    m_recorder->insertCoveredCell(column, row);
    return;
  }

  if ((m_rowSizes.size() <= row) || (m_columnSizes.size() <= column))
    return;

  Cell cell;
  cell.m_covered = true;
  m_table[row][column] = cell;
}

void IWORKTable::draw(const librevenge::RVNGPropertyList &tableProps, IWORKOutputElements &elements)
{
  assert(!m_recorder);

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
    if (r < m_headerRows)
      rowProps.insert("librevenge:is-header-row", true);

    elements.addOpenTableRow(rowProps);
    for (std::size_t c = 0; row.size() != c; ++c)
    {
      const Cell &cell = row[c];
      librevenge::RVNGPropertyList cellProps;
      cellProps.insert("librevenge:column", numeric_cast<int>(c));
      cellProps.insert("librevenge:row", numeric_cast<int>(r));

      using namespace property;

      if (m_horizontalLines.find(r)!=m_horizontalLines.end())
        writeBorder(cellProps, "fo:border-top", m_horizontalLines.find(r)->second, c);
      if (m_horizontalLines.find(r+1)!=m_horizontalLines.end())
        writeBorder(cellProps, "fo:border-bottom", m_horizontalLines.find(r+1)->second, c);
      if (m_verticalLines.find(c)!=m_verticalLines.end())
        writeBorder(cellProps, "fo:border-left", m_verticalLines.find(c)->second, r);
      if (m_verticalLines.find(c+1)!=m_verticalLines.end())
        writeBorder(cellProps, "fo:border-right", m_verticalLines.find(c+1)->second, r);

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

        IWORKStyleStack style;
        style.push(getDefaultCellStyle(c, r));
        style.push(cell.m_style);
        writeCellFormat(cellProps, style, cell.m_type, cell.m_style ? cell.m_style->getIdent() : none, cell.m_value);
        writeCellStyle(cellProps, style);

        IWORKStyleStack pStyle;
        pStyle.push(getDefaultParagraphStyle(c,r));
        if (style.has<SFTCellStylePropertyParagraphStyle>())
          pStyle.push(style.get<SFTCellStylePropertyParagraphStyle>());
        IWORKText::fillCharPropList(pStyle, m_langManager, cellProps);

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

void IWORKTable::setDefaultCellStyle(const CellType type, const IWORKStylePtr_t &style)
{
  if (bool(m_recorder))
  {
    m_recorder->setDefaultCellStyle(type, style);
    return;
  }

  assert(type < ETONYEK_NUM_ELEMENTS(m_defaultCellStyles));
  m_defaultCellStyles[type] = style;
}

void IWORKTable::setDefaultLayoutStyle(const CellType type, const IWORKStylePtr_t &style)
{
  if (bool(m_recorder))
  {
    m_recorder->setDefaultLayoutStyle(type, style);
    return;
  }

  assert(type < ETONYEK_NUM_ELEMENTS(m_defaultLayoutStyles));
  m_defaultLayoutStyles[type] = style;
}

void IWORKTable::setDefaultParagraphStyle(const CellType type, const IWORKStylePtr_t &style)
{
  if (bool(m_recorder))
  {
    m_recorder->setDefaultParagraphStyle(type, style);
    return;
  }

  assert(type < ETONYEK_NUM_ELEMENTS(m_defaultParaStyles));
  m_defaultParaStyles[type] = style;
}

IWORKStylePtr_t IWORKTable::getDefaultCellStyle(unsigned column, unsigned row) const
{
  return getDefaultStyle(column, row, m_defaultCellStyles);
}

IWORKStylePtr_t IWORKTable::getDefaultLayoutStyle(unsigned column, unsigned row) const
{
  return getDefaultStyle(column, row, m_defaultLayoutStyles);
}

IWORKStylePtr_t IWORKTable::getDefaultParagraphStyle(unsigned column, unsigned row) const
{
  return getDefaultStyle(column, row, m_defaultParaStyles);
}

IWORKStylePtr_t IWORKTable::getDefaultStyle(const unsigned column, const unsigned row, const IWORKStylePtr_t *const group) const
{
  if ((row < m_headerRows) && bool(group[CELL_TYPE_ROW_HEADER]))
    return group[CELL_TYPE_ROW_HEADER];
  else if (((m_rows - row) < m_footerRows) && bool(group[CELL_TYPE_ROW_FOOTER]))
    return group[CELL_TYPE_ROW_FOOTER];
  else if ((column < m_headerColumns) && bool(group[CELL_TYPE_COLUMN_HEADER]))
    return group[CELL_TYPE_COLUMN_HEADER];
  else if (m_bandedRows && (row % 2 == 1) && bool(group[CELL_TYPE_ALTERNATE_BODY]))
    return group[CELL_TYPE_ALTERNATE_BODY];
  else
    return group[CELL_TYPE_BODY];
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
