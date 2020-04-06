/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKTableRecorder.h"

#include <deque>

#include <boost/variant.hpp>

#include "IWORKOutputElements.h"
#include "IWORKText.h"
#include "IWORKTextRecorder.h"

namespace libetonyek
{

using std::shared_ptr;

namespace
{

struct SetComment
{
  SetComment(const unsigned column, const unsigned row, IWORKOutputElements const &text)
    : m_column(column)
    , m_row(row)
    , m_text(text)
  {
  }

  const unsigned m_column;
  const unsigned m_row;
  const IWORKOutputElements m_text;
};

struct SetSize
{
  SetSize(const unsigned columns, const unsigned rows)
    : m_columns(columns)
    , m_rows(rows)
  {
  }

  const unsigned m_columns;
  const unsigned m_rows;
};

struct SetHeaders
{
  SetHeaders(const unsigned headerColumns, const unsigned headerRows, const unsigned footerRows)
    : m_headerColumns(headerColumns)
    , m_headerRows(headerRows)
    , m_footerRows(footerRows)
  {
  }

  const unsigned m_headerColumns;
  const unsigned m_headerRows;
  const unsigned m_footerRows;
};

struct SetBandedRows
{
  explicit SetBandedRows(const bool banded)
    : m_banded(banded)
  {
  }

  const bool m_banded;
};

struct SetRepeated
{
  SetRepeated(const bool columns, const bool rows)
    : m_columns(columns)
    , m_rows(rows)
  {
  }

  const bool m_columns;
  const bool m_rows;
};

struct SetOrder
{
  explicit SetOrder(const int order)
    : m_order(order)
  {
  }

  const int m_order;
};

struct SetStyle
{
  explicit SetStyle(const IWORKStylePtr_t &style)
    : m_style(style)
  {
  }

  const IWORKStylePtr_t m_style;
};

struct SetSizes
{
  SetSizes(const IWORKColumnSizes_t &columnSizes, const IWORKRowSizes_t &rowSizes)
    : m_columnSizes(columnSizes)
    , m_rowSizes(rowSizes)
  {
  }

  const IWORKColumnSizes_t m_columnSizes;
  const IWORKRowSizes_t m_rowSizes;
};

struct SetBorders
{
  SetBorders(const IWORKGridLineMap_t &verticalLines, const IWORKGridLineMap_t &horizontalLines)
    : m_verticalLines(verticalLines)
    , m_verticalRightLines()
    , m_horizontalLines(horizontalLines)
    , m_horizontalBottomLines()
  {
  }
  SetBorders(const IWORKGridLineMap_t &verticalLeftLines, const IWORKGridLineMap_t &verticalRightLines,
             const IWORKGridLineMap_t &horizontalTopLines, const IWORKGridLineMap_t &horizontalBottomLines)
    : m_verticalLines(verticalLeftLines)
    , m_verticalRightLines(verticalRightLines)
    , m_horizontalLines(horizontalTopLines)
    , m_horizontalBottomLines(horizontalBottomLines)
  {
  }

  const IWORKGridLineMap_t m_verticalLines;
  const IWORKGridLineMap_t m_verticalRightLines;
  const IWORKGridLineMap_t m_horizontalLines;
  const IWORKGridLineMap_t m_horizontalBottomLines;
};

struct InsertCell
{
  InsertCell(const unsigned column, const unsigned row, const boost::optional<std::string> &value, const std::shared_ptr<IWORKText> &content, const boost::optional<IWORKDateTimeData> &dateTime, const unsigned columnSpan, const unsigned rowSpan, const IWORKFormulaPtr_t &formula, const boost::optional<unsigned> &formulaHC, const IWORKStylePtr_t &style, IWORKCellType type)
    : m_column(column)
    , m_row(row)
    , m_value(value)
    , m_content(content)
    , m_dateTime(dateTime)
    , m_columnSpan(columnSpan)
    , m_rowSpan(rowSpan)
    , m_formula(formula)
    , m_formulaHC(formulaHC)
    , m_style(style)
    , m_type(type)
  {
  }

  const unsigned m_column;
  const unsigned m_row;
  const boost::optional<std::string> m_value;
  const std::shared_ptr<IWORKText> m_content;
  const boost::optional<IWORKDateTimeData> m_dateTime;
  const unsigned m_columnSpan;
  const unsigned m_rowSpan;
  const IWORKFormulaPtr_t m_formula;
  boost::optional<unsigned> m_formulaHC;
  const IWORKStylePtr_t m_style;
  const IWORKCellType m_type;
};

struct InsertCoveredCell
{
  InsertCoveredCell(const unsigned column, const unsigned row)
    : m_column(column)
    , m_row(row)
  {
  }

  const unsigned m_column;
  const unsigned m_row;
};

struct SetDefaultCellStyle
{
  SetDefaultCellStyle(IWORKTable::CellType type, const IWORKStylePtr_t &style)
    : m_type(type)
    , m_style(style)
  {
  }

  const IWORKTable::CellType m_type;
  const IWORKStylePtr_t m_style;
};

struct SetDefaultLayoutStyle
{
  SetDefaultLayoutStyle(IWORKTable::CellType type, const IWORKStylePtr_t &style)
    : m_type(type)
    , m_style(style)
  {
  }

  const IWORKTable::CellType m_type;
  const IWORKStylePtr_t m_style;
};

struct SetDefaultParagraphStyle
{
  SetDefaultParagraphStyle(IWORKTable::CellType type, const IWORKStylePtr_t &style)
    : m_type(type)
    , m_style(style)
  {
  }

  const IWORKTable::CellType m_type;
  const IWORKStylePtr_t m_style;
};

typedef boost::variant
< SetComment
, SetSize
, SetHeaders
, SetBandedRows
, SetRepeated
, SetOrder
, SetStyle
, SetSizes
, SetBorders
, InsertCell
, InsertCoveredCell
, SetDefaultCellStyle
, SetDefaultLayoutStyle
, SetDefaultParagraphStyle
>
Element_t;

}

namespace
{

struct Sender : public boost::static_visitor<void>
{
  explicit Sender(IWORKTable &table)
    : m_table(table)
  {
  }

  void operator()(const SetComment &value) const
  {
    m_table.setComment(value.m_column, value.m_row, value.m_text);
  }

  void operator()(const SetSize &value) const
  {
    m_table.setSize(value.m_columns, value.m_rows);
  }

  void operator()(const SetHeaders &value) const
  {
    m_table.setHeaders(value.m_headerColumns, value.m_headerRows, value.m_footerRows);
  }

  void operator()(const SetBandedRows &value) const
  {
    m_table.setBandedRows(value.m_banded);
  }

  void operator()(const SetRepeated &value) const
  {
    m_table.setRepeated(value.m_columns, value.m_rows);
  }

  void operator()(const SetStyle &value) const
  {
    m_table.setStyle(value.m_style);
  }

  void operator()(const SetOrder &value) const
  {
    m_table.setOrder(value.m_order);
  }

  void operator()(const SetSizes &value) const
  {
    m_table.setSizes(value.m_columnSizes, value.m_rowSizes);
  }

  void operator()(const SetBorders &value) const
  {
    m_table.setBorders(value.m_verticalLines,value.m_verticalRightLines,
                       value.m_horizontalLines, value.m_horizontalBottomLines);
  }

  void operator()(const InsertCell &value) const
  {
    const shared_ptr<IWORKTextRecorder> recorder(value.m_content->getRecorder());
    value.m_content->setRecorder(shared_ptr<IWORKTextRecorder>());
    if (bool(recorder))
      recorder->replay(*value.m_content);
    m_table.insertCell(value.m_column, value.m_row, value.m_value, value.m_content, value.m_dateTime, value.m_columnSpan, value.m_rowSpan, value.m_formula, value.m_formulaHC, value.m_style, value.m_type);
  }

  void operator()(const InsertCoveredCell &value) const
  {
    m_table.insertCoveredCell(value.m_column, value.m_row);
  }

  void operator()(const SetDefaultCellStyle &value) const
  {
    m_table.setDefaultCellStyle(value.m_type, value.m_style);
  }

  void operator()(const SetDefaultLayoutStyle &value) const
  {
    m_table.setDefaultLayoutStyle(value.m_type, value.m_style);
  }

  void operator()(const SetDefaultParagraphStyle &value) const
  {
    m_table.setDefaultParagraphStyle(value.m_type, value.m_style);
  }

private:
  IWORKTable &m_table;
};

}

struct IWORKTableRecorder::Impl
{
  Impl();

  std::deque<Element_t> m_elements;
};

IWORKTableRecorder::Impl::Impl()
  : m_elements()
{
}

IWORKTableRecorder::IWORKTableRecorder()
  : m_impl(new Impl())
{
}

void IWORKTableRecorder::replay(IWORKTable &table) const
{
  Sender sender(table);
  for (std::deque<Element_t>::const_iterator it = m_impl->m_elements.begin(); it != m_impl->m_elements.end(); ++it)
    boost::apply_visitor(sender, *it);
}

void IWORKTableRecorder::setComment(unsigned column, unsigned row, IWORKOutputElements const &text)
{
  m_impl->m_elements.push_back(SetComment(column, row, text));
}

void IWORKTableRecorder::setSize(unsigned columns, unsigned rows)
{
  m_impl->m_elements.push_back(SetSize(columns, rows));
}

void IWORKTableRecorder::setHeaders(unsigned headerColumns, unsigned headerRows, const unsigned footerRows)
{
  m_impl->m_elements.push_back(SetHeaders(headerColumns, headerRows, footerRows));
}

void IWORKTableRecorder::setBandedRows(bool banded)
{
  m_impl->m_elements.push_back(SetBandedRows(banded));
}

void IWORKTableRecorder::setRepeated(bool columns, bool rows)
{
  m_impl->m_elements.push_back(SetRepeated(columns, rows));
}

void IWORKTableRecorder::setOrder(int order)
{
  m_impl->m_elements.push_back(SetOrder(order));
}

void IWORKTableRecorder::setStyle(const IWORKStylePtr_t &style)
{
  m_impl->m_elements.push_back(SetStyle(style));
}

void IWORKTableRecorder::setSizes(const IWORKColumnSizes_t &columnSizes, const IWORKRowSizes_t &rowSizes)
{
  m_impl->m_elements.push_back(SetSizes(columnSizes, rowSizes));
}

void IWORKTableRecorder::setBorders(const IWORKGridLineMap_t &verticalLines, const IWORKGridLineMap_t &horizontalLines)
{
  m_impl->m_elements.push_back(SetBorders(verticalLines, horizontalLines));
}

void IWORKTableRecorder::setBorders(const IWORKGridLineMap_t &verticalLeftLines, const IWORKGridLineMap_t &verticalRightLines,
                                    const IWORKGridLineMap_t &horizontalTopLines, const IWORKGridLineMap_t &horizontalBottomLines)
{
  m_impl->m_elements.push_back(SetBorders(verticalLeftLines, verticalRightLines,
                                          horizontalTopLines, horizontalBottomLines));
}

void IWORKTableRecorder::insertCell(const unsigned column, const unsigned row, const boost::optional<std::string> &value, const std::shared_ptr<IWORKText> &content, const boost::optional<IWORKDateTimeData> &dateTime, const unsigned columnSpan, const unsigned rowSpan, const IWORKFormulaPtr_t &formula, const boost::optional<unsigned> &formulaHC, const IWORKStylePtr_t &style, const IWORKCellType type)
{
  m_impl->m_elements.push_back(InsertCell(column, row, value, content, dateTime, columnSpan, rowSpan, formula, formulaHC, style, type));
}

void IWORKTableRecorder::insertCoveredCell(const unsigned column, const unsigned row)
{
  m_impl->m_elements.push_back(InsertCoveredCell(column, row));
}

void IWORKTableRecorder::setDefaultCellStyle(const IWORKTable::CellType type, const IWORKStylePtr_t &style)
{
  m_impl->m_elements.push_back(SetDefaultCellStyle(type, style));
}

void IWORKTableRecorder::setDefaultLayoutStyle(const IWORKTable::CellType type, const IWORKStylePtr_t &style)
{
  m_impl->m_elements.push_back(SetDefaultLayoutStyle(type, style));
}

void IWORKTableRecorder::setDefaultParagraphStyle(const IWORKTable::CellType type, const IWORKStylePtr_t &style)
{
  m_impl->m_elements.push_back(SetDefaultParagraphStyle(type, style));
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
