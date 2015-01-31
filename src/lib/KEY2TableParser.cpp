/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <boost/lexical_cast.hpp>

#include "libetonyek_xml.h"
#include "IWORKToken.h"
#include "IWORKXMLReader.h"
#include "KEYCollector.h"
#include "KEY2Parser.h"
#include "KEY2TableParser.h"
#include "KEY2Token.h"

using boost::lexical_cast;

namespace libetonyek
{

KEY2TableParser::KEY2TableParser(KEY2Parser &parser)
  : m_parser(parser)
  , m_columnSizes()
  , m_rowSizes()
  , m_column(0)
  , m_row(0)
  , m_columnSpan()
  , m_rowSpan()
  , m_cellMove()
  , m_content()
{
}

void KEY2TableParser::parse(const IWORKXMLReader &reader)
{
  assert(checkElement(reader, IWORKToken::tabular_info, IWORKToken::NS_URI_SF));

  getCollector()->startLevel();

  IWORKXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (getId(element))
    {
    case IWORKToken::geometry | IWORKToken::NS_URI_SF :
      m_parser.parseGeometry(element);
      break;
    case IWORKToken::tabular_model | IWORKToken::NS_URI_SF :
      parseTabularModel(element);
      break;
    default :
      skipElement(element);
    }
  }

  getCollector()->collectTable();

  getCollector()->endLevel();
}

void KEY2TableParser::parseTabularModel(const IWORKXMLReader &reader)
{
  assert(checkElement(reader, IWORKToken::tabular_model, IWORKToken::NS_URI_SF));

  IWORKXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (getId(element))
    {
    case IWORKToken::grid | IWORKToken::NS_URI_SF :
      parseGrid(element);
      break;
    default :
      skipElement(element);
    }
  }
}

void KEY2TableParser::parseGrid(const IWORKXMLReader &reader)
{
  assert(checkElement(reader, IWORKToken::grid, IWORKToken::NS_URI_SF));

  IWORKXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (getId(element))
    {
    case IWORKToken::columns | IWORKToken::NS_URI_SF :
      parseColumns(element);
      break;
    case IWORKToken::datasource | IWORKToken::NS_URI_SF :
      parseDatasource(element);
      break;
    case IWORKToken::rows | IWORKToken::NS_URI_SF :
      parseRows(element);
      break;
    default :
      skipElement(element);
    }
  }
}

void KEY2TableParser::parseColumns(const IWORKXMLReader &reader)
{
  assert(checkElement(reader, IWORKToken::columns, IWORKToken::NS_URI_SF));
  assert(m_columnSizes.empty());

  IWORKXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (getId(element))
    {
    case IWORKToken::grid_column | IWORKToken::NS_URI_SF :
      parseGridColumn(element);
      break;
    default :
      skipElement(element);
    }
  }
}

void KEY2TableParser::parseGridColumn(const IWORKXMLReader &reader)
{
  assert(checkElement(reader, IWORKToken::grid_column, IWORKToken::NS_URI_SF));

  IWORKXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    switch (getId(attr))
    {
    case IWORKToken::width | IWORKToken::NS_URI_SF :
      m_columnSizes.push_back(lexical_cast<double>(attr.getValue()));
      break;
    default :
      break;
    }
  }

  checkEmptyElement(reader);
}

void KEY2TableParser::parseRows(const IWORKXMLReader &reader)
{
  assert(checkElement(reader, IWORKToken::rows, IWORKToken::NS_URI_SF));
  assert(m_rowSizes.empty());

  IWORKXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (getId(element))
    {
    case IWORKToken::grid_row | IWORKToken::NS_URI_SF :
      parseGridRow(element);
      break;
    default :
      skipElement(element);
    }
  }
}

void KEY2TableParser::parseGridRow(const IWORKXMLReader &reader)
{
  assert(checkElement(reader, IWORKToken::grid_row, IWORKToken::NS_URI_SF));

  IWORKXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    switch (getId(attr))
    {
    case IWORKToken::height | IWORKToken::NS_URI_SF :
      m_rowSizes.push_back(lexical_cast<double>(attr.getValue()));
      break;
    default :
      break;
    }
  }

  checkEmptyElement(reader);
}


void KEY2TableParser::parseDatasource(const IWORKXMLReader &reader)
{
  assert(checkElement(reader, IWORKToken::datasource, IWORKToken::NS_URI_SF));

  // these must be defined before datasource, otherwise we have a problem
  assert(!m_columnSizes.empty());
  assert(!m_rowSizes.empty());

  getCollector()->collectTableSizes(m_rowSizes, m_columnSizes);

  IWORKXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (getId(element))
    {
    case IWORKToken::d | IWORKToken::NS_URI_SF :
      parseD(element);
      break;
    case IWORKToken::du | IWORKToken::NS_URI_SF :
      parseDu(element);
      break;
    case IWORKToken::f | IWORKToken::NS_URI_SF :
      parseF(element);
      break;
    case IWORKToken::g | IWORKToken::NS_URI_SF :
      parseG(element);
      break;
    case IWORKToken::n | IWORKToken::NS_URI_SF :
      parseN(element);
      break;
    case IWORKToken::s | IWORKToken::NS_URI_SF :
      parseS(element);
      break;
    case IWORKToken::t | IWORKToken::NS_URI_SF :
      parseT(element);
      break;
    default :
      skipElement(element);
    }
  }
}

void KEY2TableParser::parseD(const IWORKXMLReader &reader)
{
  assert(checkElement(reader, IWORKToken::d, IWORKToken::NS_URI_SF));

  IWORKXMLReader::AttributeIterator attr(reader);
  while (attr.next())
    parseCommonCellAttribute(attr);

  checkEmptyElement(reader);

  emitCell();
}

void KEY2TableParser::parseDu(const IWORKXMLReader &reader)
{
  assert(checkElement(reader, IWORKToken::du, IWORKToken::NS_URI_SF));

  IWORKXMLReader::AttributeIterator attr(reader);
  while (attr.next())
    parseCommonCellAttribute(attr);

  checkEmptyElement(reader);

  emitCell();
}

void KEY2TableParser::parseF(const IWORKXMLReader &reader)
{
  assert(checkElement(reader, IWORKToken::f, IWORKToken::NS_URI_SF));

  IWORKXMLReader::AttributeIterator attr(reader);
  while (attr.next())
    parseCommonCellAttribute(attr);

  IWORKXMLReader::ElementIterator element(reader);
  while (element.next())
    skipElement(element);

  emitCell();
}

void KEY2TableParser::parseG(const IWORKXMLReader &reader)
{
  assert(checkElement(reader, IWORKToken::g, IWORKToken::NS_URI_SF));

  IWORKXMLReader::AttributeIterator attr(reader);
  while (attr.next())
    parseCommonCellAttribute(attr);

  checkEmptyElement(reader);

  emitCell();
}

void KEY2TableParser::parseN(const IWORKXMLReader &reader)
{
  assert(checkElement(reader, IWORKToken::n, IWORKToken::NS_URI_SF));

  IWORKXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    switch (getId(attr))
    {
    case IWORKToken::v | IWORKToken::NS_URI_SF :
      m_content = attr.getValue();
      break;
    default :
      parseCommonCellAttribute(attr);
    }
  }

  checkEmptyElement(reader);

  emitCell();
}

void KEY2TableParser::parseS(const IWORKXMLReader &reader)
{
  assert(checkElement(reader, IWORKToken::s, IWORKToken::NS_URI_SF));

  IWORKXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    switch (getId(attr))
    {
    case IWORKToken::ct | IWORKToken::NS_URI_SF :
      m_cellMove = lexical_cast<unsigned>(attr.getValue());
      break;
    default :
      break;
    }
  }

  checkEmptyElement(reader);

  emitCell(true);
}

void KEY2TableParser::parseT(const IWORKXMLReader &reader)
{
  assert(checkElement(reader, IWORKToken::t, IWORKToken::NS_URI_SF));

  getCollector()->startText(false);

  IWORKXMLReader::AttributeIterator attr(reader);
  while (attr.next())
    parseCommonCellAttribute(attr);

  IWORKXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (getId(element))
    {
    case IWORKToken::ct | IWORKToken::NS_URI_SF :
      parseCt(element);
      break;
    default :
      skipElement(element);
    }
  }

  emitCell();

  getCollector()->endText();
}

void KEY2TableParser::parseCt(const IWORKXMLReader &reader)
{
  assert(checkElement(reader, IWORKToken::ct, IWORKToken::NS_URI_SF));

  IWORKXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    switch (getId(attr))
    {
    case IWORKToken::s | IWORKToken::NS_URI_SF :
      m_content = attr.getValue();
      break;
    default :
      break;
    }
  }

  IWORKXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (getId(element))
    {
    case IWORKToken::so | IWORKToken::NS_URI_SF :
      if (m_content)
      {
        ETONYEK_DEBUG_MSG(("found a text cell with both simple and formatted content\n"));
      }
      parseSo(element);
      break;
    default :
      skipElement(element);
    }
  }
}

void KEY2TableParser::parseSo(const IWORKXMLReader &reader)
{
  assert(checkElement(reader, IWORKToken::so, IWORKToken::NS_URI_SF));

  IWORKXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (getId(element))
    {
    case IWORKToken::text_body | IWORKToken::NS_URI_SF :
      m_parser.parseTextBody(element);
      break;
    default :
      skipElement(element);
    }
  }
}

void KEY2TableParser::parseCommonCellAttribute(const IWORKXMLReader::AttributeIterator &attr)
{
  switch (getId(attr))
  {
  case IWORKToken::col_span | IWORKToken::NS_URI_SF :
    m_columnSpan = lexical_cast<unsigned>(attr.getValue());
    break;
  case IWORKToken::ct | IWORKToken::NS_URI_SF :
    m_cellMove = lexical_cast<unsigned>(attr.getValue());
    break;
  case IWORKToken::row_span | IWORKToken::NS_URI_SF :
    m_rowSpan = lexical_cast<unsigned>(attr.getValue());
    break;
  }
}

void KEY2TableParser::emitCell(const bool covered)
{
  // determine the cell's position
  if (m_cellMove)
  {
    const unsigned ct = get(m_cellMove);
    if (0x80 > ct)
    {
      m_column += ct;
    }
    else
    {
      ++m_row;
      m_column -= (0x100 - ct);
    }
  }
  else
  {
    ++m_column;
    if (m_columnSizes.size() == m_column)
    {
      m_column = 0;
      ++m_row;
    }
  }
  assert(m_columnSizes.size() > m_column);
  assert(m_rowSizes.size() > m_row);

  // send the cell to collector
  if (covered)
    getCollector()->collectCoveredTableCell(m_row, m_column);
  else
    getCollector()->collectTableCell(m_row, m_column, m_content, get_optional_value_or(m_rowSpan, 1), get_optional_value_or(m_columnSpan, 1));

  // reset cell attributes
  m_columnSpan.reset();
  m_rowSpan.reset();
  m_cellMove.reset();
  m_content.reset();
}

KEYCollector *KEY2TableParser::getCollector()
{
  return m_parser.getCollector();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
