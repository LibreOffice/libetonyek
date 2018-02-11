/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKTableInfoElement.h"

#include "libetonyek_xml.h"
#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKFormulaElement.h"
#include "IWORKGeometryElement.h"
#include "IWORKMutableArrayElement.h"
#include "IWORKNumberElement.h"
#include "IWORKPushCollector.h"
#include "IWORKRefContext.h"
#include "IWORKStringElement.h"
#include "IWORKStyleContainer.h"
#include "IWORKTable.h"
#include "IWORKText.h"
#include "IWORKTextStorageElement.h"
#include "IWORKToken.h"
#include "IWORKWrapElement.h"
#include "IWORKXMLParserState.h"
#include "IWORKXMLContextBase.h"


namespace libetonyek
{

namespace
{
typedef IWORKStyleContainer<IWORKToken::NS_URI_SF | IWORKToken::table_style, IWORKToken::NS_URI_SF | IWORKToken::table_style_ref> TableStyleContext;
}

namespace
{

class TableCellContentElement : public IWORKXMLElementContextBase
{
public:
  explicit TableCellContentElement(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
};

TableCellContentElement::TableCellContentElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
  if (isCollector())
  {
    assert(!getState().m_currentText);
    getState().m_currentText = getCollector().createText(getState().m_langManager, false);
  }
}

void TableCellContentElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::ID | IWORKToken::NS_URI_SFA :
    IWORKXMLElementContextBase::attribute(name, value);
    break;
  default :
    ETONYEK_DEBUG_MSG(("TableCellContentElement::attribute[IWORKTableInfoElement.cpp]: find some unknown attribute\n"));
    break;
  }
}

IWORKXMLContextPtr_t TableCellContentElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::text_storage | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_type = IWORK_CELL_TYPE_TEXT;
    return makeContext<IWORKTextStorageElement>(getState());
  default :
    ETONYEK_DEBUG_MSG(("TableCellContentElement::element[IWORKTableInfoElement.cpp]: find some unknown element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class TableCellValueElement : public IWORKXMLEmptyContextBase
{
public:
  explicit TableCellValueElement(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

  boost::optional<double> m_value;
};

TableCellValueElement::TableCellValueElement(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
  , m_value()
{
}

void TableCellValueElement::attribute(const int name, const char *const value)
{
  ETONYEK_DEBUG_MSG(("TableCellValueElement::attribute[IWORKTableInfoElement.cpp]: find some unknown attribute\n"));
  IWORKXMLEmptyContextBase::attribute(name, value);
}

IWORKXMLContextPtr_t TableCellValueElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::number | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_type = IWORK_CELL_TYPE_NUMBER;
    return makeContext<IWORKNumberElement<double> >(getState(), m_value);
  default:
    ETONYEK_DEBUG_MSG(("TableCellValueElement::element[IWORKTableInfoElement.cpp]: find some unknown element\n"));
  }
  return IWORKXMLContextPtr_t();
}

void TableCellValueElement::endOfElement()
{
  if (m_value)
  {
    std::stringstream s;
    s << get(m_value);
    getState().m_tableData->m_content = s.str();
  }
}
}

namespace
{
class TableCellElement : public IWORKXMLEmptyContextBase
{
public:
  explicit TableCellElement(IWORKXMLParserState &state, boost::optional<IWORKTableCell> &value);
private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
  void emitCell();

  boost::optional<IWORKTableCell> &m_value;
  boost::optional<ID_t> m_styleRef;
  boost::optional<ID_t> m_minXBorderRef, m_maxXBorderRef, m_minYBorderRef, m_maxYBorderRef;
};

TableCellElement::TableCellElement(IWORKXMLParserState &state, boost::optional<IWORKTableCell> &value)
  : IWORKXMLEmptyContextBase(state)
  , m_value(value)
  , m_styleRef()
  , m_minXBorderRef()
  , m_maxXBorderRef()
  , m_minYBorderRef()
  , m_maxYBorderRef()
{
  m_value=IWORKTableCell();
}

void TableCellElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::tableCellPreferredHeight | IWORKToken::NS_URI_SF :
    get(m_value).m_preferredHeight=try_double_cast(value);
    break;
  case IWORKToken::ID | IWORKToken::NS_URI_SFA : // must we store this element ?
    IWORKXMLEmptyContextBase::attribute(name, value);
    break;
  default :
    ETONYEK_DEBUG_MSG(("TableCellElement::attribute[IWORKTableInfoElement.cpp]: find some unknown attribute\n"));
    IWORKXMLEmptyContextBase::attribute(name, value);
  }
}

IWORKXMLContextPtr_t TableCellElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::tableCellStyle_ref | IWORKToken::NS_URI_SF :
    return makeContext<IWORKRefContext>(getState(), m_styleRef);
  case IWORKToken::tableCellMinXSide_ref | IWORKToken::NS_URI_SF :
    return makeContext<IWORKRefContext>(getState(), m_minXBorderRef);
  case IWORKToken::tableCellMaxXSide_ref | IWORKToken::NS_URI_SF :
    return makeContext<IWORKRefContext>(getState(), m_maxXBorderRef);
  case IWORKToken::tableCellMinYSide_ref | IWORKToken::NS_URI_SF :
    return makeContext<IWORKRefContext>(getState(), m_minYBorderRef);
  case IWORKToken::tableCellMaxYSide_ref | IWORKToken::NS_URI_SF :
    return makeContext<IWORKRefContext>(getState(), m_maxYBorderRef);
  case IWORKToken::tableCellContent | IWORKToken::NS_URI_SF :
    return makeContext<TableCellContentElement>(getState());
  case IWORKToken::tableCellFormula | IWORKToken::NS_URI_SF :
    return makeContext<IWORKTableCellFormulaElement>(getState());
  case IWORKToken::tableCellValue | IWORKToken::NS_URI_SF :
    return makeContext<TableCellValueElement>(getState());
  default:
    ETONYEK_DEBUG_MSG(("TableCellElement::element[IWORKTableInfoElement.cpp]: find some unknown element\n"));
    break;
  }
  return IWORKXMLContextPtr_t();
}

void TableCellElement::endOfElement()
{
  IWORKTableCell &cell=get(m_value);
  if (m_styleRef)
    cell.m_style=getState().getStyleByName(get(m_styleRef).c_str(), getState().getDictionary().m_tableCellStyles);
  for (int i=0; i<4; ++i)
  {
    boost::optional<ID_t> const &ref=
      i==0 ? m_minXBorderRef : i==1 ? m_maxXBorderRef : i==2 ? m_minYBorderRef : m_maxYBorderRef;
    if (!ref) continue;
    const IWORKTableVectorMap_t::const_iterator it = getState().getDictionary().m_tableVectors.find(get(ref));
    if (it==getState().getDictionary().m_tableVectors.end())
    {
      ETONYEK_DEBUG_MSG(("TableCellElement::endOfElement[IWORKTableInfoElement.cpp]: can not find vector %s\n", get(ref).c_str()));
    }
    else if (i==0)
      cell.m_minXBorder=it->second;
    else if (i==1)
      cell.m_maxXBorder=it->second;
    else if (i==2)
      cell.m_minYBorder=it->second;
    else
      cell.m_maxYBorder=it->second;
  }
  if (getId())
    getState().getDictionary().m_tableCells[get(getId())]=cell;
  emitCell();
}

void TableCellElement::emitCell()
{
  const IWORKTableDataPtr_t tableData = getState().m_tableData;
  IWORKTableCell &cell=get(m_value);
  if (cell.m_minXBorder.m_along && cell.m_minYBorder.m_along
      && tableData->m_positionToHorizontalLineMap.find(get(cell.m_minXBorder.m_along))
      != tableData->m_positionToHorizontalLineMap.end()
      && tableData->m_positionToVerticalLineMap.find(get(cell.m_minYBorder.m_along))
      != tableData->m_positionToVerticalLineMap.end())
  {
    // column
    unsigned column=tableData->m_positionToHorizontalLineMap.find(get(cell.m_minXBorder.m_along))->second;
    unsigned columnSpan=0;
    unsigned numColumns=(unsigned) tableData->m_columnSizes.size();
    if (cell.m_maxXBorder.m_along && tableData->m_positionToHorizontalLineMap.find(get(cell.m_maxXBorder.m_along))
        != tableData->m_positionToHorizontalLineMap.end())
      columnSpan=unsigned(tableData->m_positionToHorizontalLineMap.find(get(cell.m_maxXBorder.m_along))->second-column);
    if (columnSpan+column>numColumns)
    {
      ETONYEK_DEBUG_MSG(("TableCellElement::endOfElement[IWORKTableInfoElement.cpp]: column span seems bad\n"));
      columnSpan=0;
    }
    // row
    unsigned row=tableData->m_positionToVerticalLineMap.find(get(cell.m_minYBorder.m_along))->second;
    unsigned rowSpan=0;
    unsigned numRows=(unsigned) tableData->m_rowSizes.size();
    if (cell.m_maxYBorder.m_along && tableData->m_positionToVerticalLineMap.find(get(cell.m_maxYBorder.m_along))
        != tableData->m_positionToVerticalLineMap.end())
      rowSpan=unsigned(tableData->m_positionToVerticalLineMap.find(get(cell.m_maxYBorder.m_along))->second-row);
    if (rowSpan+row>numRows)
    {
      ETONYEK_DEBUG_MSG(("TableCellElement::endOfElement[IWORKTableInfoElement.cpp]: row span seems bad\n"));
      rowSpan=0;
    }
    IWORKTextPtr_t text(getState().m_currentText);
    getState().m_currentText.reset();
    if (bool(tableData->m_content) && tableData->m_type == IWORK_CELL_TYPE_TEXT)
    {
      text = getCollector().createText(getState().m_langManager);
      text->insertText(get(tableData->m_content));
      text->flushParagraph();
    }
    getState().m_currentTable->insertCell(
      column, row,
      tableData->m_content, text, tableData->m_dateTime,
      columnSpan ? columnSpan : 0, rowSpan ? rowSpan : 1,
      tableData->m_formula, tableData->m_formulaHC, cell.m_style, tableData->m_type
    );
    // finally update the grid
    if (rowSpan && cell.m_minXBorder.m_style)
    {
      if (tableData->m_verticalLines.find(column)==tableData->m_verticalLines.end())
        tableData->m_verticalLines.insert(IWORKGridLineMap_t::value_type(column,IWORKGridLine_t(0, numRows+1, IWORKStylePtr_t())));
      IWORKGridLine_t &cLine=tableData->m_verticalLines.find(column)->second;
      cLine.insert_back(row, row+rowSpan, cell.m_minXBorder.m_style);
    }
    if (rowSpan && columnSpan && cell.m_maxXBorder.m_style)
    {
      if (tableData->m_verticalLines.find(column+columnSpan)==tableData->m_verticalLines.end())
        tableData->m_verticalLines.insert(IWORKGridLineMap_t::value_type(column+columnSpan,IWORKGridLine_t(0, numRows+1, IWORKStylePtr_t())));
      IWORKGridLine_t &cLine=tableData->m_verticalLines.find(column+columnSpan)->second;
      cLine.insert_back(row, row+rowSpan, cell.m_maxXBorder.m_style);
    }
    if (columnSpan && cell.m_minYBorder.m_style)
    {
      if (tableData->m_horizontalLines.find(row)==tableData->m_horizontalLines.end())
        tableData->m_horizontalLines.insert(IWORKGridLineMap_t::value_type(row,IWORKGridLine_t(0, numColumns+1, IWORKStylePtr_t())));
      IWORKGridLine_t &cLine=tableData->m_horizontalLines.find(row)->second;
      cLine.insert_back(column, column+columnSpan, cell.m_minYBorder.m_style);
    }
    if (columnSpan && columnSpan && cell.m_maxYBorder.m_style)
    {
      if (tableData->m_horizontalLines.find(row+rowSpan)==tableData->m_horizontalLines.end())
        tableData->m_horizontalLines.insert(IWORKGridLineMap_t::value_type(row+rowSpan,IWORKGridLine_t(0, numColumns+1, IWORKStylePtr_t())));
      IWORKGridLine_t &cLine=tableData->m_horizontalLines.find(row+rowSpan)->second;
      cLine.insert_back(column, column+columnSpan, cell.m_maxYBorder.m_style);
    }
  }
  else
  {
    ETONYEK_DEBUG_MSG(("TableCellElement::endOfElement[IWORKTableInfoElement.cpp]: can not find cell positions\n"));
  }

  // reset cell attributes
  tableData->m_columnSpan.reset();
  tableData->m_rowSpan.reset();
  tableData->m_content.reset();
  tableData->m_dateTime.reset();
  tableData->m_formula.reset();
  tableData->m_style.reset();
  tableData->m_type = IWORK_CELL_TYPE_TEXT;
}
}

namespace
{
typedef IWORKMutableArrayElement<IWORKTableCell, TableCellElement, IWORKPushCollector, IWORKToken::NS_URI_SF | IWORKToken::table_cell, IWORKToken::NS_URI_SF | IWORKToken::table_cell_ref> TableCellArrayElement;
}

namespace
{
class TableModelCellsElement : public IWORKXMLEmptyContextBase
{
public:
  explicit TableModelCellsElement(IWORKXMLParserState &state, std::deque<IWORKTableCell> &colCell, std::deque<IWORKTableCell> &rowCell);
private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

  std::unordered_map<ID_t, std::deque<IWORKTableCell> > m_dict;
  std::deque<IWORKTableCell> &m_columnsCell;
  std::deque<IWORKTableCell> &m_rowsCell;
};

TableModelCellsElement::TableModelCellsElement(IWORKXMLParserState &state, std::deque<IWORKTableCell> &colCell, std::deque<IWORKTableCell> &rowCell)
  : IWORKXMLEmptyContextBase(state)
  , m_dict()
  , m_columnsCell(colCell)
  , m_rowsCell(rowCell)
{
}

IWORKXMLContextPtr_t TableModelCellsElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::tableCellArrayCellsByColumn | IWORKToken::NS_URI_SF :
    return makeContext<TableCellArrayElement>(getState(), m_dict, getState().getDictionary().m_tableCells, m_columnsCell);
  case IWORKToken::tableCellArrayCellsByRow | IWORKToken::NS_URI_SF :
    return makeContext<TableCellArrayElement>(getState(), m_dict, getState().getDictionary().m_tableCells, m_rowsCell);
  default:
    ETONYEK_DEBUG_MSG(("TableModelCellsElement::element[IWORKTableInfoElement.cpp]: find some unknown element\n"));
    break;
  }
  return IWORKXMLContextPtr_t();
}

void TableModelCellsElement::endOfElement()
{
}
}

namespace
{
class TableVectorElement : public IWORKXMLEmptyContextBase
{
public:
  explicit TableVectorElement(IWORKXMLParserState &state, boost::optional<IWORKTableVector> &value);
private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

  boost::optional<IWORKTableVector> &m_value;
  boost::optional<ID_t> m_styleRef;
};

TableVectorElement::TableVectorElement(IWORKXMLParserState &state, boost::optional<IWORKTableVector> &value)
  : IWORKXMLEmptyContextBase(state)
  , m_value(value)
  , m_styleRef()
{
  m_value=IWORKTableVector();
}

void TableVectorElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::tableVectorAlong | IWORKToken::NS_URI_SF :
    get(m_value).m_along=try_double_cast(value);
    break;
  case IWORKToken::tableVectorAxis | IWORKToken::NS_URI_SF :
    get(m_value).m_axis=try_int_cast(value);
    break;
  case IWORKToken::tableVectorBegin | IWORKToken::NS_URI_SF :
    get(m_value).m_beginCell=try_int_cast(value);
    break;
  case IWORKToken::tableVectorEnd | IWORKToken::NS_URI_SF :
    get(m_value).m_endCell=try_int_cast(value);
    break;
  case IWORKToken::ID | IWORKToken::NS_URI_SFA :
    IWORKXMLEmptyContextBase::attribute(name, value);
    break;
  default :
    ETONYEK_DEBUG_MSG(("TableVectorElement::attribute[IWORKTableInfoElement.cpp]: find some unknown attribute\n"));
    IWORKXMLEmptyContextBase::attribute(name, value);
  }
}

IWORKXMLContextPtr_t TableVectorElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::tableVectorStyle_ref | IWORKToken::NS_URI_SF :
    return makeContext<IWORKRefContext>(getState(), m_styleRef);
  default:
    ETONYEK_DEBUG_MSG(("TableVectorElement::element[IWORKTableInfoElement.cpp]: find some unknown element\n"));
    break;
  }
  return IWORKXMLContextPtr_t();
}

void TableVectorElement::endOfElement()
{
  if (m_styleRef)
    get(m_value).m_style=getState().getStyleByName(get(m_styleRef).c_str(), getState().getDictionary().m_tableVectorStyles);
  if (getId())
    getState().getDictionary().m_tableVectors[get(getId())]=get(m_value);
}
}

namespace
{
typedef IWORKMutableArrayElement<IWORKTableVector, TableVectorElement, IWORKPushCollector, IWORKToken::NS_URI_SF | IWORKToken::table_vector> TableVectorArrayElement;
}

namespace
{
class TableModelVectorsElement : public IWORKXMLEmptyContextBase
{
public:
  explicit TableModelVectorsElement(IWORKXMLParserState &state, std::deque<IWORKTableVector> &colVector, std::deque<IWORKTableVector> &rowVector);
private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

  std::unordered_map<ID_t, std::deque<IWORKTableVector> > m_dict;
  std::deque<IWORKTableVector> &m_columnsVector;
  std::deque<IWORKTableVector> &m_rowsVector;
};

TableModelVectorsElement::TableModelVectorsElement(IWORKXMLParserState &state, std::deque<IWORKTableVector> &colVector, std::deque<IWORKTableVector> &rowVector)
  : IWORKXMLEmptyContextBase(state)
  , m_dict()
  , m_columnsVector(colVector)
  , m_rowsVector(rowVector)
{
}

IWORKXMLContextPtr_t TableModelVectorsElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::tableVectorArrayColumnVectors | IWORKToken::NS_URI_SF :
    return makeContext<TableVectorArrayElement>(getState(), m_dict, getState().getDictionary().m_tableVectors, m_columnsVector);
  case IWORKToken::tableVectorArrayRowVectors | IWORKToken::NS_URI_SF :
    return makeContext<TableVectorArrayElement>(getState(), m_dict, getState().getDictionary().m_tableVectors, m_rowsVector);
  default:
    ETONYEK_DEBUG_MSG(("TableModelVectorsElement::element[IWORKTableInfoElement.cpp]: find some unknown element\n"));
    break;
  }
  return IWORKXMLContextPtr_t();
}

void TableModelVectorsElement::endOfElement()
{
  const IWORKTableDataPtr_t tableData = getState().m_tableData;

  // normally the columns and rows are sorted
  double pos=0;
  bool posSet=false;
  unsigned line=0;
  for (size_t i=0; i<m_columnsVector.size(); ++i)
  {
    if (!m_columnsVector[i].m_along)
      continue;
    if (!posSet)
    {
      pos=get(m_columnsVector[i].m_along);
      posSet=true;
      tableData->m_positionToHorizontalLineMap[pos]=line;
    }
    else if (get(m_columnsVector[i].m_along)>pos)
    {
      tableData->m_columnSizes.push_back(get(m_columnsVector[i].m_along)-pos);
      pos=get(m_columnsVector[i].m_along);
      tableData->m_positionToHorizontalLineMap[pos]=++line;
    }
  }
  posSet=false;
  line=0;
  for (size_t i=0; i<m_rowsVector.size(); ++i)
  {
    if (!m_rowsVector[i].m_along)
      continue;
    if (!posSet)
    {
      pos=get(m_rowsVector[i].m_along);
      posSet=true;
      tableData->m_positionToVerticalLineMap[pos]=line;
    }
    else if (get(m_rowsVector[i].m_along)>pos)
    {
      tableData->m_rowSizes.push_back(get(m_rowsVector[i].m_along)-pos);
      pos=get(m_rowsVector[i].m_along);
      tableData->m_positionToVerticalLineMap[pos]=++line;
    }
  }
  if (getState().m_currentTable)
    getState().m_currentTable->setSizes(getState().m_tableData->m_columnSizes, getState().m_tableData->m_rowSizes);
}
}

namespace
{
class TableInfoTableElement : public IWORKXMLElementContextBase
{
public:
  explicit TableInfoTableElement(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  boost::optional<ID_t> m_id;
  boost::optional<std::string> m_tableId;
  boost::optional<ID_t> m_styleRef;
  bool m_hasHeaderColumn;
  bool m_hasHeaderRow;
  std::deque<IWORKTableCell> m_columnsCell;
  std::deque<IWORKTableCell> m_rowsCell;
  std::deque<IWORKTableVector> m_columnsVector;
  std::deque<IWORKTableVector> m_rowsVector;
};

TableInfoTableElement::TableInfoTableElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_id()
  , m_tableId()
  , m_styleRef()
  , m_hasHeaderColumn(false)
  , m_hasHeaderRow(false)
  , m_columnsCell()
  , m_rowsCell()
  , m_columnsVector()
  , m_rowsVector()
{
}

void TableInfoTableElement::attribute(const int name, const char *value)
{
  switch (name)
  {
  case IWORKToken::ID | IWORKToken::NS_URI_SFA :
    m_id=value;
    break;
  case IWORKToken::tableModelIsHeaderColumn | IWORKToken::NS_URI_SF :
    m_hasHeaderColumn =bool_cast(value);
    break;
  case IWORKToken::tableModelIsHeaderRow | IWORKToken::NS_URI_SF :
    m_hasHeaderRow =bool_cast(value);
    break;
  default:
    // also tableModelResize, tableIsForPasteboardOnly
    break;
  }
}

void TableInfoTableElement::startOfElement()
{
  getState().m_tableData.reset(new IWORKTableData());
}

IWORKXMLContextPtr_t TableInfoTableElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::tableModelStyle_ref | IWORKToken::NS_URI_SF :
    return makeContext<IWORKRefContext>(getState(), m_styleRef);
  case IWORKToken::tableModelPartitionSource | IWORKToken::NS_URI_SF : // contains id + frame data
    break;
  case IWORKToken::tableModelVectors | IWORKToken::NS_URI_SF :
    return makeContext<TableModelVectorsElement>(getState(), m_columnsVector, m_rowsVector);
  case IWORKToken::tableModelCells | IWORKToken::NS_URI_SF :
    return makeContext<TableModelCellsElement>(getState(), m_columnsCell, m_rowsCell);
  case IWORKToken::tableModelTableID | IWORKToken::NS_URI_SF :
    return makeContext<IWORKStringElement>(getState(), m_tableId);
  default:
    ETONYEK_DEBUG_MSG(("TableInfoTableElement::element[IWORKTableInfoElement.cpp]: find some unknown element\n"));
    break;
  }
  return IWORKXMLContextPtr_t();
}

void TableInfoTableElement::endOfElement()
{
  if (bool(getState().m_currentTable))
  {
    auto const &table=getState().m_currentTable;
    if (m_styleRef)
    {
      IWORKStylePtr_t style;
      style=getState().getStyleByName(get(m_styleRef).c_str(), getState().getDictionary().m_tableStyles);
      table->setStyle(style);
    }
    table->setRepeated(m_hasHeaderColumn, m_hasHeaderRow);
    table->setHeaders(0, 0, 0);
  }
}
}

IWORKTableInfoElement::IWORKTableInfoElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_style()
  , m_wrap()
{
}

void IWORKTableInfoElement::startOfElement()
{
  assert(!getState().m_currentTable);
  getState().m_currentTable = getCollector().createTable(getState().m_tableNameMap, getState().m_langManager);
  if (isCollector())
    getCollector().startLevel();
}

IWORKXMLContextPtr_t IWORKTableInfoElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::geometry | IWORKToken::NS_URI_SF :
    return makeContext<IWORKGeometryElement>(getState());
  case IWORKToken::style | IWORKToken::NS_URI_SF :
    return makeContext<TableStyleContext>(getState(), m_style, getState().getDictionary().m_tableStyles);
  case IWORKToken::tableInfoTable | IWORKToken::NS_URI_SF :
    return makeContext<TableInfoTableElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::wrap : // USEME
    return makeContext<IWORKWrapElement>(getState(), m_wrap);
  default:
    ETONYEK_DEBUG_MSG(("IWORKTableInfoElement::element: find some unknown element\n"));
  }

  return IWORKXMLContextPtr_t();
}

void IWORKTableInfoElement::endOfElement()
{
  if (!isCollector())
    return;

  if (m_style && getState().m_currentTable)
    getState().m_currentTable->setStyle(m_style);
  getCollector().collectTable(getState().m_currentTable);
  getState().m_currentTable.reset();

  getCollector().endLevel();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
