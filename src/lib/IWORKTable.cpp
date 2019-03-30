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
#include <iomanip>
#include <sstream>

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

void parseDateTimeFormat(std::string const &format, librevenge::RVNGPropertyList &props, boost::optional<std::string> &rvngValueType)
{
  if (format.empty())
  {
    ETONYEK_DEBUG_MSG(("parseDateTimeFormat[IWORKTable.cpp]: called without format string\n"));
    return;
  }
  std::string text("");
  librevenge::RVNGPropertyListVector propVect;
  size_t len=format.size();
  bool hasDate=false, findSomeToken=false, inText=false;
  for (size_t c=0; c < len;)
  {
    size_t numCh=1;
    char ch=format[c++];
    if (inText)
    {
      if (ch=='\'')
        inText=false;
      else
        text+=ch;
      continue;
    }
    if (ch=='\'')
    {
      inText=true;
      continue;
    }
    while (numCh<4 && c < len && format[c]==ch)
    {
      ++numCh;
      ++c;
    }
    librevenge::RVNGPropertyList list;
    switch (ch)
    {
    case 'a':
      list.insert("librevenge:value-type", "am-pm");
      c-=(numCh-1);
      break;
    case 'd':
      hasDate=true;
      list.insert("librevenge:value-type", "day");
      if (numCh>1) list.insert("number:style", "long");
      if (numCh>2) c-=(numCh-2);
      break;
    case 'h':
    case 'k':
      list.insert("librevenge:value-type", "hours");
      if (numCh>2) c-=(numCh-2);
      break;
    case 'm':
      list.insert("librevenge:value-type", "minutes");
      list.insert("number:style", "long");
      if (numCh>2) c-=(numCh-2);
      break;
    case 's':
      list.insert("librevenge:value-type", "seconds");
      list.insert("number:style", "long");
      if (numCh>2) c-=(numCh-2);
      break;
    case 'y':
      hasDate=true;
      list.insert("librevenge:value-type", "year");
      if (numCh>2) list.insert("number:style", "long");
      break;
    case 'E':
      hasDate=true;
      list.insert("librevenge:value-type", "day-of-week");
      if (numCh>3) list.insert("number:style", "long");
      break;
    case 'H':
    case 'K':
      list.insert("librevenge:value-type", "hours");
      list.insert("number:style", "long");
      if (numCh>2) c-=(numCh-2);
      break;
    case 'M':
      hasDate=true;
      list.insert("librevenge:value-type", "month");
      if ((numCh%2)==0) list.insert("number:style", "long");
      if (numCh>2) list.insert("number:textual", true);
      break;
    // G: Era designator?
    default:
      break;
    }
    if (!list.empty())
    {
      if (!text.empty())
      {
        librevenge::RVNGPropertyList tList;
        tList.insert("librevenge:value-type", "text");
        tList.insert("librevenge:text", text.c_str());
        propVect.append(tList);
      }
      text.clear();
      propVect.append(list);
      findSomeToken=true;
    }
    else
    {
      for (size_t n=0; n<numCh; ++n)
        text += ch;
    }
  }
  if (!findSomeToken || inText)
  {
    ETONYEK_DEBUG_MSG(("parseDateTimeFormat[IWORKTable.cpp]: something went bad when parsing %s\n", format.c_str()));
    return;
  }
  if (!text.empty())
  {
    librevenge::RVNGPropertyList list;
    list.insert("librevenge:value-type", "text");
    list.insert("librevenge:text", text.c_str());
    propVect.append(list);
  }
  rvngValueType = hasDate ? "date" : "time";
  props.insert("librevenge:value-type", get(rvngValueType).c_str());
  props.insert("number:automatic-order", "true");
  if (propVect.count())
    props.insert("librevenge:format", propVect);
}

void writeBorder(librevenge::RVNGPropertyList &props, const char *name, IWORKGridLine_t &line, unsigned index)
{
  if (!line.is_tree_valid())
    line.build_tree();

  IWORKStylePtr_t style;
  line.search_tree(index, style);
  if (style && style->has<property::SFTStrokeProperty>())
    writeBorder(style->get<property::SFTStrokeProperty>(), name, props);
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
    props.insert("style:vertical-align", "top");
    break;
  case IWORK_VERTICAL_ALIGNMENT_MIDDLE :
    props.insert("style:vertical-align", "middle");
    break;
  case IWORK_VERTICAL_ALIGNMENT_BOTTOM :
    props.insert("style:vertical-align", "bottom");
    break;
  default:
    ETONYEK_DEBUG_MSG(("writeCellStyle[IWORKTable.cpp]: unexpected alignement\n"));
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
      props.insert("fo:padding-left", get(padding.m_left), librevenge::RVNG_POINT);
    if (padding.m_right)
      props.insert("fo:padding-right", get(padding.m_right), librevenge::RVNG_POINT);
    if (padding.m_top)
      props.insert("fo:padding-top", get(padding.m_top), librevenge::RVNG_POINT);
    if (padding.m_bottom)
      props.insert("fo:padding-bottom", get(padding.m_bottom), librevenge::RVNG_POINT);
  }
}

void writeCellValue(librevenge::RVNGPropertyList &props,
                    const boost::optional<std::string> &styleName,
                    const IWORKCellType type, const boost::optional<std::string> &valueType,
                    const boost::optional<std::string> &value, const boost::optional<IWORKDateTimeData> &dateTime)
try
{
  using namespace property;

  // TODO: what's this anyway?
  if (styleName)
    props.insert("librevenge:name", get(styleName).c_str());
  switch (type)
  {
  case IWORK_CELL_TYPE_NUMBER :
    if (valueType)
    {
      props.insert("librevenge:value-type", get(valueType).c_str());
      props.insert("librevenge:value", value ? get(value).c_str() : "0");
    }
    else if (value)
    {
      props.insert("librevenge:value-type", "double");
      props.insert("librevenge:value", get(value).c_str());
    }
    break;
  case IWORK_CELL_TYPE_DATE_TIME :
    if (value || dateTime)
    {
      props.insert("librevenge:value-type", valueType ? get(valueType).c_str() : "date");

      if (dateTime)
      {
        props.insert("librevenge:day", get(dateTime).m_day);
        props.insert("librevenge:month", get(dateTime).m_month);
        props.insert("librevenge:year", get(dateTime).m_year);
        props.insert("librevenge:hours", get(dateTime).m_hour);
        props.insert("librevenge:minutes", get(dateTime).m_minute);
        props.insert("librevenge:seconds", get(dateTime).m_second);
      }
      else
      {
        boost::optional<double> seconds=try_double_cast(get(value).c_str());
        if (!seconds)
        {
          ETONYEK_DEBUG_MSG(("writeCellValue[IWORKTable.cpp]: can not read seconds\n"));
          break;
        }
        const auto t = std::time_t(ETONYEK_EPOCH_BEGIN + get(seconds));
        struct tm *const time = gmtime(&t);

        if (!time)
        {
          ETONYEK_DEBUG_MSG(("writeCellValue[IWORKTable.cpp]: can not convert seconds in time\n"));
          break;
        }
        props.insert("librevenge:day", time->tm_mday);
        props.insert("librevenge:month", time->tm_mon + 1);
        props.insert("librevenge:year", time->tm_year + 1900);
        props.insert("librevenge:hours", time->tm_hour);
        props.insert("librevenge:minutes", time->tm_min);
        props.insert("librevenge:seconds", time->tm_sec);
      }
    }
    break;
  case IWORK_CELL_TYPE_DURATION :
    if (value)
    {
      const int seconds = double_cast(get(value).c_str());
      props.insert("librevenge:value-type", valueType ? get(valueType).c_str() : "time");
      props.insert("librevenge:hours", int(seconds / 3600));
      props.insert("librevenge:minutes", int((seconds % 3600) / 60));
      props.insert("librevenge:seconds", int((seconds % 3600) % 60));
    }
    break;
  case IWORK_CELL_TYPE_BOOL :
    props.insert("librevenge:value-type", valueType ? get(valueType).c_str() : "boolean");
    props.insert("librevenge:value", value ? get(value).c_str() : "0");  // false is default
    break;
  case IWORK_CELL_TYPE_TEXT :
  default:
    //TODO: librevenge:name ?
    if (value)
      props.insert("librevenge:value-type", "string");
    break;
  }
}
catch (...)
{
  ETONYEK_DEBUG_MSG(("writeCellValue[IWORKTable.cpp]: catch exception\n"));
}

librevenge::RVNGString convertCellValueInText(const IWORKStyleStack &style, const IWORKCellType type, const boost::optional<std::string> &value, const boost::optional<IWORKDateTimeData> &dateTime)
{
  try
  {
    using namespace property;
    switch (type)
    {
    case IWORK_CELL_TYPE_NUMBER :
    {
      int numDecimals=2;
      IWORKCellNumberType formatType=IWORK_CELL_NUMBER_TYPE_DOUBLE;
      std::string currency("$");
      if (style.has<SFTCellStylePropertyNumberFormat>())
      {
        const IWORKNumberFormat &format = style.get<SFTCellStylePropertyNumberFormat>();
        numDecimals=format.m_decimalPlaces;
        formatType=format.m_type;
        currency=format.m_currencyCode.c_str();
      }
      const double val = value ? double_cast(get(value).c_str()) : 0;
      std::stringstream s;
      s << std::setprecision(-numDecimals);
      switch (formatType)
      {
      case IWORK_CELL_NUMBER_TYPE_CURRENCY :
        s << std::fixed << val << currency;
        break;
      case IWORK_CELL_NUMBER_TYPE_PERCENTAGE :
        s << std::fixed << 100*val << "%";
        break;
      case IWORK_CELL_NUMBER_TYPE_SCIENTIFIC :
        s << std::scientific << val;
        break;
      case IWORK_CELL_NUMBER_TYPE_DOUBLE :
        s << val;
        break;
      default:
        ETONYEK_DEBUG_MSG(("convertCellValueInText: unexpected number format\n"));
      }
      return s.str().c_str();
    }
    case IWORK_CELL_TYPE_DATE_TIME :
    {
      if (dateTime)
      {
        librevenge::RVNGString res;
        if (get(dateTime).m_hour)
          res.sprintf("%d/%d/%d %d:%d", get(dateTime).m_month, get(dateTime).m_day, get(dateTime).m_year, get(dateTime).m_hour, get(dateTime).m_minute);
        else
          res.sprintf("%d/%d/%d", get(dateTime).m_month, get(dateTime).m_day, get(dateTime).m_year);
        return res;
      }
      if (!value) break;
      boost::optional<double> seconds=try_double_cast(get(value).c_str());
      if (!seconds)
      {
        ETONYEK_DEBUG_MSG(("convertCellValueInText: can not read seconds\n"));
        break;
      }
      const auto t = std::time_t(ETONYEK_EPOCH_BEGIN + get(seconds));
      struct tm *const time = gmtime(&t);
      librevenge::RVNGString res;
      if (time->tm_hour)
        res.sprintf("%d/%d/%d %d:%d", time->tm_mon + 1, time->tm_mday, time->tm_year + 1900, time->tm_hour, time->tm_min);
      else
        res.sprintf("%d/%d/%d", time->tm_mon + 1, time->tm_mday, time->tm_year + 1900);
      return res;
    }
    case IWORK_CELL_TYPE_DURATION :
    {
      if (!value) break;
      const int seconds = double_cast(get(value).c_str());
      librevenge::RVNGString res;
      res.sprintf("%d:%d:%d", int(seconds / 3600), int((seconds % 3600) / 60), int((seconds % 3600) % 60));
      return res;
    }
    case IWORK_CELL_TYPE_BOOL :
      return value && get(value)!="0" ? "true" : "false";
    case IWORK_CELL_TYPE_TEXT :
    default:
      break;
    }
  }
  catch (...)
  {
    ETONYEK_DEBUG_MSG(("convertCellValueInText: exception\n"));
  }
  return "";
}

}

IWORKTable::Cell::Cell()
  : m_content()
  , m_columnSpan(1)
  , m_rowSpan(1)
  , m_covered(false)
  , m_formula()
  , m_formulaHC()
  , m_style()
  , m_type(IWORK_CELL_TYPE_TEXT)
  , m_value()
  , m_dateTime()
{
}

IWORKTable::IWORKTable(const IWORKTableNameMapPtr_t &tableNameMap, const IWORKLanguageManager &langManager)
  : m_tableNameMap(tableNameMap)
  , m_langManager(langManager)
  , m_formatNameMap()
  , m_table()
  , m_style()
  , m_order()
  , m_columnSizes()
  , m_rowSizes()
  , m_verticalLines()
  , m_verticalRightLines()
  , m_horizontalLines()
  , m_horizontalBottomLines()
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

void IWORKTable::setRecorder(const std::shared_ptr<IWORKTableRecorder> &recorder)
{
  m_recorder = recorder;
}

const std::shared_ptr<IWORKTableRecorder> &IWORKTable::getRecorder() const
{
  return m_recorder;
}

void IWORKTable::setName(std::string const &name)
{
  m_name=name;
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

void IWORKTable::setOrder(int order)
{
  if (bool(m_recorder))
  {
    m_recorder->setOrder(order);
    return;
  }

  m_order = order;
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

void IWORKTable::setBorders(const IWORKGridLineMap_t &verticalLeftLines, const IWORKGridLineMap_t &verticalRightLines,
                            const IWORKGridLineMap_t &horizontalTopLines, const IWORKGridLineMap_t &horizontalBottomLines)
{
  if (bool(m_recorder))
  {
    m_recorder->setBorders(verticalLeftLines, verticalRightLines, horizontalTopLines, horizontalBottomLines);
    return;
  }

  m_verticalLines = verticalLeftLines;
  m_verticalRightLines = verticalRightLines;
  m_horizontalLines = horizontalTopLines;
  m_horizontalBottomLines = horizontalBottomLines;
}

void IWORKTable::insertCell(const unsigned column, const unsigned row, const boost::optional<std::string> &value, const std::shared_ptr<IWORKText> &text, const boost::optional<IWORKDateTimeData> &dateTime, const unsigned columnSpan, const unsigned rowSpan, const IWORKFormulaPtr_t &formula, const boost::optional<unsigned> &formulaHC, const IWORKStylePtr_t &style, const IWORKCellType type)
{
  if (bool(m_recorder))
  {
    m_recorder->insertCell(column, row, value, text, dateTime, columnSpan, rowSpan, formula, formulaHC, style, type);
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
  cell.m_formulaHC = formulaHC;
  cell.m_style = style;
  cell.m_type = type;
  cell.m_value = value;
  cell.m_dateTime = dateTime;
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

boost::optional<std::string> IWORKTable::writeFormat(IWORKOutputElements &elements, const IWORKStylePtr_t &style, const IWORKCellType type, boost::optional<std::string> &rvngValueType)
{
  if (!style) return none;
  librevenge::RVNGPropertyList props;
  switch (type)
  {
  case IWORK_CELL_TYPE_NUMBER :
    if (style->has<property::SFTCellStylePropertyNumberFormat>())
    {
      const IWORKNumberFormat &format = style->get<property::SFTCellStylePropertyNumberFormat>();
      props.insert("librevenge:value-type", format.getRVNGValueType().c_str());
      if (format.m_decimalPlaces>=0) props.insert("number:decimal-places", format.m_decimalPlaces);
      if (format.m_thousandsSeparator) props.insert("number:grouping", true);
      rvngValueType=format.m_type==IWORK_CELL_NUMBER_TYPE_FRACTION ? "double" : format.getRVNGValueType().c_str();
      switch (format.m_type)
      {
      case IWORK_CELL_NUMBER_TYPE_CURRENCY :
      {
        librevenge::RVNGPropertyListVector pVect;
        librevenge::RVNGPropertyList currency;
        currency.insert("librevenge:value-type", "currency-symbol");
        currency.insert("number:language","en");
        currency.insert("number:country","US");
        currency.insert("librevenge:currency",format.m_currencyCode.c_str());
        if (format.m_decimalPlaces>=0) currency.insert("number:decimal-places", format.m_decimalPlaces);
        pVect.append(currency);
        props.insert("librevenge:format", pVect);
        break;
      }
      case IWORK_CELL_NUMBER_TYPE_PERCENTAGE :
      case IWORK_CELL_NUMBER_TYPE_SCIENTIFIC :
      case IWORK_CELL_NUMBER_TYPE_DOUBLE :
        break;
      case IWORK_CELL_NUMBER_TYPE_FRACTION :
        props.insert("number:min-integer-digits", 0);
        props.insert("number:min-numerator-digits",1);
        props.insert("number:min-denominator-digits", 1);
        props.remove("number:decimal-places");
        break;
      default:
        ETONYEK_DEBUG_MSG(("IWORKTable::writeFormat: unexpected number format\n"));
        return none;
      }
    }
    break;
  case IWORK_CELL_TYPE_DATE_TIME : // todo
    if (style->has<property::SFTCellStylePropertyDateTimeFormat>())
    {
      const IWORKDateTimeFormat &format = style->get<property::SFTCellStylePropertyDateTimeFormat>();
      parseDateTimeFormat(format.m_format, props, rvngValueType);
    }
    break;
  case IWORK_CELL_TYPE_DURATION : // todo
  // TODO: How to insert format and convert?
  // const IWORKDurationFormat &format = style->get<SFTCellStylePropertyDurationFormat>();
  // props.insert("librevenge:format",format.m_format.c_str());

  case IWORK_CELL_TYPE_BOOL :
  case IWORK_CELL_TYPE_TEXT :
  default:
    break;
  }
  if (props.empty())
    return none;
  auto hash=props.getPropString();
  auto it=m_formatNameMap.find(hash);
  if (it!=m_formatNameMap.end())
    return it->second;
  std::stringstream name;
  name << "Numbering" << m_formatNameMap.size();
  m_formatNameMap[hash]=name.str();
  props.insert("librevenge:name", name.str().c_str());
  elements.addDefineSheetNumberingStyle(props);
  return name.str();
}

void IWORKTable::draw(const librevenge::RVNGPropertyList &tableProps, IWORKOutputElements &elements, bool drawAsSimpleTable)
{
  assert(!m_recorder);

  librevenge::RVNGPropertyListVector columnSizes;

  for (IWORKColumnSizes_t::const_iterator it = m_columnSizes.begin(); m_columnSizes.end() != it; ++it)
  {
    librevenge::RVNGPropertyList column;
    if (!it->m_size)
      ;
    else if (it->m_exactSize)
      column.insert("style:column-width", pt2in(get(it->m_size)));
    else
      column.insert("style:min-column-width", pt2in(get(it->m_size)));
    columnSizes.append(column);
  }

  librevenge::RVNGPropertyList allTableProps(tableProps);
  allTableProps.insert(drawAsSimpleTable ? "librevenge:table-columns" : "librevenge:columns", columnSizes);

  elements.addOpenTable(allTableProps);
  for (std::size_t r = 0; m_table.size() != r; ++r)
  {
    const Row_t &row = m_table[r];

    librevenge::RVNGPropertyList rowProps;
    auto const &rSize=m_rowSizes[r];
    if (rSize.m_size && rSize.m_exactSize)
      rowProps.insert("style:row-height", pt2in(get(rSize.m_size)));
    else if (rSize.m_size)
      rowProps.insert("style:min-row-height", pt2in(get(rSize.m_size)));
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
      unsigned const rMax= unsigned(r+ std::max(unsigned(1),cell.m_rowSpan));
      unsigned const cMax= unsigned(c+ std::max(unsigned(1),cell.m_columnSpan));
      if (m_horizontalLines.find(unsigned(r))!=m_horizontalLines.end())
        writeBorder(cellProps, "fo:border-top", m_horizontalLines.find(unsigned(r))->second, unsigned(c));
      if (!m_horizontalBottomLines.empty())
      {
        if (m_horizontalBottomLines.find(rMax-1)!=m_horizontalBottomLines.end())
          writeBorder(cellProps, "fo:border-bottom", m_horizontalBottomLines.find(rMax-1)->second, unsigned(c));
      }
      else if (m_horizontalLines.find(rMax)!=m_horizontalLines.end())
        writeBorder(cellProps, "fo:border-bottom", m_horizontalLines.find(rMax)->second, unsigned(c));
      if (m_verticalLines.find(unsigned(c))!=m_verticalLines.end())
        writeBorder(cellProps, "fo:border-left", m_verticalLines.find(unsigned(c))->second, unsigned(r));
      if (!m_verticalRightLines.empty())
      {
        if (m_verticalRightLines.find(cMax-1)!=m_verticalRightLines.end())
          writeBorder(cellProps, "fo:border-right", m_verticalRightLines.find(cMax-1)->second, unsigned(r));
      }
      else if (m_verticalLines.find(cMax)!=m_verticalLines.end())
        writeBorder(cellProps, "fo:border-right", m_verticalLines.find(cMax)->second, unsigned(r));

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
        style.push(getDefaultCellStyle(unsigned(c), unsigned(r)));
        style.push(cell.m_style);
        if (!drawAsSimpleTable)
        {
          optional<std::string> valueType;
          auto formatName=writeFormat(elements, cell.m_style, cell.m_type, valueType);
          if (formatName) cellProps.insert("librevenge:numbering-name", get(formatName).c_str());
          writeCellValue(cellProps, cell.m_style ? cell.m_style->getIdent() : none,
                         cell.m_type, valueType, cell.m_value, cell.m_dateTime);
        }
        writeCellStyle(cellProps, style);

        IWORKStyleStack pStyle;
        pStyle.push(getDefaultParagraphStyle(unsigned(c),unsigned(r)));
        if (style.has<SFTCellStylePropertyParagraphStyle>())
          pStyle.push(style.get<SFTCellStylePropertyParagraphStyle>());
        IWORKText::fillCharPropList(pStyle, m_langManager, cellProps);

        if (!drawAsSimpleTable && cell.m_formula)
          elements.addOpenFormulaCell(cellProps, *cell.m_formula, cell.m_formulaHC, m_tableNameMap);
        else
          elements.addOpenTableCell(cellProps);

        if (!cell.m_content.empty() && cell.m_type!=IWORK_CELL_TYPE_DATE_TIME && cell.m_type!=IWORK_CELL_TYPE_DURATION)
          elements.append(cell.m_content);
        else if (drawAsSimpleTable)
        {
          librevenge::RVNGString value=convertCellValueInText(style, cell.m_type, cell.m_value, cell.m_dateTime);
          if (!value.empty())
          {
            librevenge::RVNGPropertyList const empty;
            elements.addOpenParagraph(empty);
            elements.addOpenSpan(empty);
            elements.addInsertText(value);
            elements.addCloseSpan();
            elements.addCloseParagraph();
          }
        }
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

boost::optional<int> IWORKTable::getOrder() const
{
  return m_order;
}

IWORKStylePtr_t IWORKTable::getStyle() const
{
  return m_style;
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
