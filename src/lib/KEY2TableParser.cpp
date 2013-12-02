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
#include "KEYCollector.h"
#include "KEY2Parser.h"
#include "KEY2TableParser.h"
#include "KEY2Token.h"
#include "KEYXMLReader.h"

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

void KEY2TableParser::parse(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::tabular_info, KEY2Token::NS_URI_SF));

  getCollector()->startLevel();

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (getId(element))
    {
    case KEY2Token::geometry | KEY2Token::NS_URI_SF :
      m_parser.parseGeometry(element);
      break;
    case KEY2Token::tabular_model | KEY2Token::NS_URI_SF :
      parseTabularModel(element);
      break;
    default :
      skipElement(element);
    }
  }

  getCollector()->collectTable();

  getCollector()->endLevel();
}

void KEY2TableParser::parseTabularModel(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::tabular_model, KEY2Token::NS_URI_SF));

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (getId(element))
    {
    case KEY2Token::grid | KEY2Token::NS_URI_SF :
      parseGrid(element);
      break;
    default :
      skipElement(element);
    }
  }
}

void KEY2TableParser::parseGrid(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::grid, KEY2Token::NS_URI_SF));

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (getId(element))
    {
    case KEY2Token::columns | KEY2Token::NS_URI_SF :
      parseColumns(element);
      break;
    case KEY2Token::datasource | KEY2Token::NS_URI_SF :
      parseDatasource(element);
      break;
    case KEY2Token::rows | KEY2Token::NS_URI_SF :
      parseRows(element);
      break;
    default :
      skipElement(element);
    }
  }
}

void KEY2TableParser::parseColumns(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::columns, KEY2Token::NS_URI_SF));
  assert(m_columnSizes.empty());

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (getId(element))
    {
    case KEY2Token::grid_column | KEY2Token::NS_URI_SF :
      parseGridColumn(element);
      break;
    default :
      skipElement(element);
    }
  }
}

void KEY2TableParser::parseGridColumn(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::grid_column, KEY2Token::NS_URI_SF));

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    switch (getId(attr))
    {
    case KEY2Token::width | KEY2Token::NS_URI_SF :
      m_columnSizes.push_back(lexical_cast<double>(attr.getValue()));
      break;
    default :
      break;
    }
  }

  checkEmptyElement(reader);
}

void KEY2TableParser::parseRows(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::rows, KEY2Token::NS_URI_SF));
  assert(m_rowSizes.empty());

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (getId(element))
    {
    case KEY2Token::grid_row | KEY2Token::NS_URI_SF :
      parseGridRow(element);
      break;
    default :
      skipElement(element);
    }
  }
}

void KEY2TableParser::parseGridRow(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::grid_row, KEY2Token::NS_URI_SF));

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    switch (getId(attr))
    {
    case KEY2Token::height | KEY2Token::NS_URI_SF :
      m_rowSizes.push_back(lexical_cast<double>(attr.getValue()));
      break;
    default :
      break;
    }
  }

  checkEmptyElement(reader);
}


void KEY2TableParser::parseDatasource(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::datasource, KEY2Token::NS_URI_SF));

  // these must be defined before datasource, otherwise we have a problem
  assert(!m_columnSizes.empty());
  assert(!m_rowSizes.empty());

  getCollector()->collectTableSizes(m_rowSizes, m_columnSizes);

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (getId(element))
    {
    case KEY2Token::d | KEY2Token::NS_URI_SF :
      parseD(element);
      break;
    case KEY2Token::du | KEY2Token::NS_URI_SF :
      parseDu(element);
      break;
    case KEY2Token::f | KEY2Token::NS_URI_SF :
      parseF(element);
      break;
    case KEY2Token::g | KEY2Token::NS_URI_SF :
      parseG(element);
      break;
    case KEY2Token::n | KEY2Token::NS_URI_SF :
      parseN(element);
      break;
    case KEY2Token::s | KEY2Token::NS_URI_SF :
      parseS(element);
      break;
    case KEY2Token::t | KEY2Token::NS_URI_SF :
      parseT(element);
      break;
    default :
      skipElement(element);
    }
  }
}

void KEY2TableParser::parseD(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::d, KEY2Token::NS_URI_SF));

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
    parseCommonCellAttribute(attr);

  checkEmptyElement(reader);

  emitCell();
}

void KEY2TableParser::parseDu(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::du, KEY2Token::NS_URI_SF));

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
    parseCommonCellAttribute(attr);

  checkEmptyElement(reader);

  emitCell();
}

void KEY2TableParser::parseF(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::f, KEY2Token::NS_URI_SF));

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
    parseCommonCellAttribute(attr);

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
    skipElement(element);

  emitCell();
}

void KEY2TableParser::parseG(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::g, KEY2Token::NS_URI_SF));

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
    parseCommonCellAttribute(attr);

  checkEmptyElement(reader);

  emitCell();
}

void KEY2TableParser::parseN(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::n, KEY2Token::NS_URI_SF));

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    switch (getId(attr))
    {
    case KEY2Token::v | KEY2Token::NS_URI_SF :
      m_content = attr.getValue();
      break;
    default :
      parseCommonCellAttribute(attr);
    }
  }

  checkEmptyElement(reader);

  emitCell();
}

void KEY2TableParser::parseS(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::s, KEY2Token::NS_URI_SF));

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    switch (getId(attr))
    {
    case KEY2Token::ct | KEY2Token::NS_URI_SF :
      m_cellMove = lexical_cast<unsigned>(attr.getValue());
      break;
    default :
      break;
    }
  }

  checkEmptyElement(reader);

  emitCell(true);
}

void KEY2TableParser::parseT(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::t, KEY2Token::NS_URI_SF));

  getCollector()->startText(false);

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
    parseCommonCellAttribute(attr);

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (getId(element))
    {
    case KEY2Token::ct | KEY2Token::NS_URI_SF :
      parseCt(element);
      break;
    default :
      skipElement(element);
    }
  }

  emitCell();

  getCollector()->endText();
}

void KEY2TableParser::parseCt(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::ct, KEY2Token::NS_URI_SF));

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    switch (getId(attr))
    {
    case KEY2Token::s | KEY2Token::NS_URI_SF :
      m_content = attr.getValue();
      break;
    default :
      break;
    }
  }

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (getId(element))
    {
    case KEY2Token::so | KEY2Token::NS_URI_SF :
      if (m_content)
      {
        KEY_DEBUG_MSG(("found a text cell with both simple and formatted content\n"));
      }
      parseSo(element);
      break;
    default :
      skipElement(element);
    }
  }
}

void KEY2TableParser::parseSo(const KEYXMLReader &reader)
{
  assert(checkElement(reader, KEY2Token::so, KEY2Token::NS_URI_SF));

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (getId(element))
    {
    case KEY2Token::text_body | KEY2Token::NS_URI_SF :
      m_parser.parseTextBody(element);
      break;
    default :
      skipElement(element);
    }
  }
}

void KEY2TableParser::parseCommonCellAttribute(const KEYXMLReader::AttributeIterator &attr)
{
  switch (getId(attr))
  {
  case KEY2Token::col_span | KEY2Token::NS_URI_SF :
    m_columnSpan = lexical_cast<unsigned>(attr.getValue());
    break;
  case KEY2Token::ct | KEY2Token::NS_URI_SF :
    m_cellMove = lexical_cast<unsigned>(attr.getValue());
    break;
  case KEY2Token::row_span | KEY2Token::NS_URI_SF :
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
