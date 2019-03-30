/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKTabularModelElement.h"

#include <cassert>
#include <ctime>
#include <memory>
#include <sstream>

#include <boost/lexical_cast.hpp>

#include "libetonyek_xml.h"
#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKFormatElement.h"
#include "IWORKFormulaElement.h"
#include "IWORKProperties.h"
#include "IWORKRefContext.h"
#include "IWORKStyle.h"
#include "IWORKTable.h"
#include "IWORKText.h"
#include "IWORKTextStorageElement.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"
#include "IWORKXMLContextBase.h"


namespace libetonyek
{

using boost::lexical_cast;
using boost::optional;
using std::shared_ptr;
using std::string;
namespace
{

class CellCoordinates: public IWORKXMLEmptyContextBase
{
public:
  CellCoordinates(IWORKXMLParserState &state, boost::optional<std::pair<unsigned,unsigned> > &coordinates)
    : IWORKXMLEmptyContextBase(state)
    , m_column()
    , m_row()
    , m_coordinates(coordinates)
  {
  }
protected:
  void attribute(int name, const char *value) final;
  void endOfElement() final;

  boost::optional<unsigned> m_column, m_row;
  boost::optional<std::pair<unsigned, unsigned> > &m_coordinates;
};

void CellCoordinates::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::column | IWORKToken::NS_URI_SF :
    m_column=lexical_cast<unsigned>(value);
    break;
  case IWORKToken::row | IWORKToken::NS_URI_SF :
    m_row=lexical_cast<unsigned>(value);
    break;
  default : // none
    IWORKXMLEmptyContextBase::attribute(name, value);
  }
}

void CellCoordinates::endOfElement()
{
  if (m_column && m_row)
    m_coordinates=std::make_pair(*m_column,*m_row);
  else
  {
    ETONYEK_DEBUG_MSG(("CellCoordinates[IWORKTabularModelElement.cpp]::endOfElement: uncomplet data\n"));
  }
}
class CellCommentMappingKey: public IWORKXMLEmptyContextBase
{
public:
  CellCommentMappingKey(IWORKXMLParserState &state, boost::optional<std::pair<unsigned,unsigned> > &coordinates)
    : IWORKXMLEmptyContextBase(state)
    , m_coordinates(coordinates)
  {
  }
protected:
  // attribute: sfa:ID sfa:class
  IWORKXMLContextPtr_t element(int name) final;

  boost::optional<std::pair<unsigned, unsigned> > &m_coordinates;
};

IWORKXMLContextPtr_t CellCommentMappingKey::element(int name)
{
  switch (name)
  {
  case IWORKToken::cell_coordinates | IWORKToken::NS_URI_SF :
    return std::make_shared<CellCoordinates>(getState(), m_coordinates);
  default:
    return IWORKXMLEmptyContextBase::element(name);
  }
}

class CellCommentMappingPair: public IWORKXMLEmptyContextBase
{
public:
  CellCommentMappingPair(IWORKXMLParserState &state, std::map<std::pair<unsigned,unsigned>,ID_t> &coordinateCommentRefMap)
    : IWORKXMLEmptyContextBase(state)
    , m_coordinates()
    , m_ref()
    , m_coordinateCommentRefMap(coordinateCommentRefMap)
  {
  }
protected:
  // attribute: none
  IWORKXMLContextPtr_t element(int name) final;
  void endOfElement() final;

  boost::optional<std::pair<unsigned, unsigned> > m_coordinates;
  boost::optional<ID_t> m_ref;
  std::map<std::pair<unsigned,unsigned>,ID_t> &m_coordinateCommentRefMap;
};

IWORKXMLContextPtr_t CellCommentMappingPair::element(int name)
{
  switch (name)
  {
  case IWORKToken::value_ref | IWORKToken::NS_URI_SFA : // attributes: sfa:IDREF and sfa:class
    return std::make_shared<IWORKRefContext>(getState(), m_ref);
  case IWORKToken::key | IWORKToken::NS_URI_SFA :
    return std::make_shared<CellCommentMappingKey>(getState(), m_coordinates);
  default:
    return IWORKXMLEmptyContextBase::element(name);
  }
}

void CellCommentMappingPair::endOfElement()
{
  if (m_coordinates && m_ref)
    m_coordinateCommentRefMap[*m_coordinates]=*m_ref;
  else
  {
    ETONYEK_DEBUG_MSG(("CellCommentMappingPair[IWORKTabularModelElement.cpp]::endOfElement: uncomplete data\n"));
  }
}

class CellCommentMapping: public IWORKXMLEmptyContextBase
{
public:
  CellCommentMapping(IWORKXMLParserState &state, std::map<std::pair<unsigned,unsigned>,ID_t> &coordinateCommentRefMap)
    : IWORKXMLEmptyContextBase(state)
    , m_coordinateCommentRefMap(coordinateCommentRefMap)
  {
  }
protected:
  // attribute: only ID ?
  IWORKXMLContextPtr_t element(int name) final;

  std::map<std::pair<unsigned,unsigned>,ID_t> &m_coordinateCommentRefMap;
};

IWORKXMLContextPtr_t CellCommentMapping::element(int name)
{
  switch (name)
  {
  case IWORKToken::pair | IWORKToken::NS_URI_SFA :
    return std::make_shared<CellCommentMappingPair>(getState(), m_coordinateCommentRefMap);
  default:
    return IWORKXMLEmptyContextBase::element(name);
  }
}

}

namespace
{
// cell format ?
class CfElement : public IWORKXMLEmptyContextBase
{
public:
  explicit CfElement(IWORKXMLParserState &state);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;

  optional<IWORKDateTimeFormat> m_dateTimeFormat;
  optional<IWORKDurationFormat> m_durationFormat;
  optional<IWORKNumberFormat> m_numberFormat;
};

CfElement::CfElement(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
  , m_dateTimeFormat()
  , m_durationFormat()
  , m_numberFormat()
{
}

void CfElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::implicit_format_type | IWORKToken::NS_URI_SF : // find 256|261|269
    break;
  case IWORKToken::ID | IWORKToken::NS_URI_SFA : // must we store this element ?
    IWORKXMLEmptyContextBase::attribute(name, value);
    break;
  default :
    ETONYEK_DEBUG_MSG(("CfElement::attribute[IWORKTabularModelElement.cpp]: find some unknown attribute\n"));
    IWORKXMLEmptyContextBase::attribute(name, value);
  }
}

IWORKXMLContextPtr_t CfElement::element(int name)
{
  switch (name)
  {
  case IWORKToken::date_format | IWORKToken::NS_URI_SF : // USEME
    return std::make_shared<IWORKDateTimeFormatElement>(getState(), m_dateTimeFormat);
  case IWORKToken::duration_format | IWORKToken::NS_URI_SF : // USEME
    return std::make_shared<IWORKDurationFormatElement>(getState(), m_durationFormat);
  case IWORKToken::number_format | IWORKToken::NS_URI_SF : // USEME
    return std::make_shared<IWORKNumberFormatElement>(getState(), m_numberFormat);
  default:
    ETONYEK_DEBUG_MSG(("CfElement::element[IWORKTabularModelElement.cpp]: find some unknown element\n"));
  }
  return IWORKXMLEmptyContextBase::element(name);
}
}

namespace
{

class CellContextBase : public IWORKXMLEmptyContextBase
{
protected:
  explicit CellContextBase(IWORKXMLParserState &state, bool isResult=false);

  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

  void emitCell(const bool covered = false);

  boost::optional<ID_t> m_ref;
  bool m_isResult;
};

CellContextBase::CellContextBase(IWORKXMLParserState &state, bool isResult)
  : IWORKXMLEmptyContextBase(state)
  , m_ref()
  , m_isResult(isResult)
{
}

void CellContextBase::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::col_span | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_columnSpan = lexical_cast<unsigned>(value);
    break;
  case IWORKToken::ct | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_cellMove = lexical_cast<unsigned>(value);
    break;
  case IWORKToken::row_span | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_rowSpan = lexical_cast<unsigned>(value);
    break;
  case IWORKToken::s | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_style = getState().getStyleByName(value, getState().getDictionary().m_cellStyles);
    break;
  case IWORKToken::f | IWORKToken::NS_URI_SF : // format?
  case IWORKToken::h | IWORKToken::NS_URI_SF : // height
  case IWORKToken::w | IWORKToken::NS_URI_SF : // width
    break;
  case IWORKToken::ID | IWORKToken::NS_URI_SFA : // can appear in result, store me?
    IWORKXMLEmptyContextBase::attribute(name,value);
    break;
  default:
    ETONYEK_DEBUG_MSG(("CellContextBase::attribute[IWORKTabularModelElement.cpp]: find some unknown attribute\n"));
  }
}

IWORKXMLContextPtr_t CellContextBase::element(int name)
{
  switch (name)
  {
  case IWORKToken::cf | IWORKToken::NS_URI_SF :
    return std::make_shared<CfElement>(getState());
  case IWORKToken::cf_ref | IWORKToken::NS_URI_SF:
    return std::make_shared<IWORKRefContext>(getState(), m_ref);
  default:
    ETONYEK_DEBUG_MSG(("CellContextBase::element[IWORKTabularModelElement.cpp]: find some unknown element\n"));
  }
  return IWORKXMLEmptyContextBase::element(name);
}

void CellContextBase::endOfElement()
{
  if (!m_isResult)
    emitCell();
}

void CellContextBase::emitCell(const bool covered)
{
  const IWORKTableDataPtr_t tableData = getState().m_tableData;

  // determine the cell's position
  if (tableData->m_cellMove)
  {
    const unsigned ct = get(tableData->m_cellMove);
    tableData->m_column += ct;
    if (tableData->m_column>=256)
    {
      tableData->m_row+=(tableData->m_column/256);
      tableData->m_column%=256;
    }
  }
  else
  {
    ++tableData->m_column;
    if (tableData->m_columnSizes.size() == tableData->m_column)
    {
      tableData->m_column = 0;
      ++tableData->m_row;
    }
  }
  assert(tableData->m_columnSizes.size() > tableData->m_column);
  assert(tableData->m_rowSizes.size() > tableData->m_row);

  // send the cell to collector
  if (bool(getState().m_currentTable))
  {
    if (covered)
    {
      getState().m_currentTable->insertCoveredCell(tableData->m_column, tableData->m_row);
    }
    else
    {
      IWORKTextPtr_t text(getState().m_currentText);
      getState().m_currentText.reset();
      if (bool(tableData->m_content) && tableData->m_type == IWORK_CELL_TYPE_TEXT)
      {
        text = getCollector().createText(getState().m_langManager);
        text->insertText(get(tableData->m_content));
        text->flushParagraph();
      }
      getState().m_currentTable->insertCell(
        tableData->m_column, tableData->m_row,
        tableData->m_content, text, tableData->m_dateTime,
        get_optional_value_or(tableData->m_columnSpan, 1), get_optional_value_or(tableData->m_rowSpan, 1),
        tableData->m_formula, tableData->m_formulaHC, tableData->m_style, tableData->m_type
      );
    }
  }

  // reset cell attributes
  tableData->m_columnSpan.reset();
  tableData->m_rowSpan.reset();
  tableData->m_cellMove.reset();
  tableData->m_content.reset();
  tableData->m_dateTime.reset();
  tableData->m_formula.reset();
  tableData->m_style.reset();
  tableData->m_type = IWORK_CELL_TYPE_TEXT;
}

}

namespace
{

class GridColumnElement : public IWORKXMLEmptyContextBase
{
public:
  explicit GridColumnElement(IWORKXMLParserState &state);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
};

GridColumnElement::GridColumnElement(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
{
}

void GridColumnElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::fitting_width | IWORKToken::NS_URI_SF :
  case IWORKToken::nc | IWORKToken::NS_URI_SF : // look like the number of cells in column
  case IWORKToken::ncoc | IWORKToken::NS_URI_SF :
  case IWORKToken::nsc | IWORKToken::NS_URI_SF : // look like the number of special cells in column, ie covered cell
  case IWORKToken::manually_sized | IWORKToken::NS_URI_SF :
  case IWORKToken::preferred_width | IWORKToken::NS_URI_SF :
    break;
  case IWORKToken::width | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_columnSizes.push_back(IWORKColumnRowSize(lexical_cast<double>(value)));
    break;
  default :
    ETONYEK_DEBUG_MSG(("GridColumnElement::attribute[IWORKTabularModelElement.cpp]: find some unknown attribute\n"));
    break;
  }
}

IWORKXMLContextPtr_t GridColumnElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::grouping_display | IWORKToken::NS_URI_SF :
  {
    static bool first=true;
    if (first)
    {
      ETONYEK_DEBUG_MSG(("GridColumnElement::element: find some grouping-display\n"));
      first=false;
    }
    return IWORKXMLContextPtr_t();
  }
  default:
    ETONYEK_DEBUG_MSG(("GridColumnElement::element[IWORKTabularModelElement.cpp]: find some unknown element\n"));
  }

  return IWORKXMLEmptyContextBase::element(name);
}
}

namespace
{

class ColumnsElement : public IWORKXMLElementContextBase
{
public:
  explicit ColumnsElement(IWORKXMLParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
};

ColumnsElement::ColumnsElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
  assert(getState().m_tableData->m_columnSizes.empty());
}

IWORKXMLContextPtr_t ColumnsElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::grid_column | IWORKToken::NS_URI_SF :
    return std::make_shared<GridColumnElement>(getState());
  default:
    ETONYEK_DEBUG_MSG(("ColumnsElement::element[IWORKTabularModelElement.cpp]: find some unknown element\n"));
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class CbElement : public CellContextBase
{
public:
  explicit CbElement(IWORKXMLParserState &state);
private:
  void attribute(int name, const char *value) override;
};

CbElement::CbElement(IWORKXMLParserState &state)
  : CellContextBase(state)
{
  getState().m_tableData->m_type = IWORK_CELL_TYPE_BOOL;
}

void CbElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::v | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_content = value;
    break;
  default :
    CellContextBase::attribute(name, value);
  }
}

}

namespace
{

class DElement : public CellContextBase
{
public:
  explicit DElement(IWORKXMLParserState &state, bool isResult=false);

private:
  void attribute(int name, const char *value) override;
};

DElement::DElement(IWORKXMLParserState &state, bool isResult)
  : CellContextBase(state, isResult)
{
  getState().m_tableData->m_type = IWORK_CELL_TYPE_DATE_TIME;
}

void DElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::cell_date | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_content = value;
    break;
  default :
    CellContextBase::attribute(name, value);
  }
}

}

namespace
{

class DuElement : public CellContextBase
{
public:
  explicit DuElement(IWORKXMLParserState &state);

private:
  void attribute(int name, const char *value) override;
};

DuElement::DuElement(IWORKXMLParserState &state)
  : CellContextBase(state)
{
}

void DuElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::du | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_content = value;
    getState().m_tableData->m_type = IWORK_CELL_TYPE_DURATION;
    break;
  default :
    CellContextBase::attribute(name, value);
  }
}

}

namespace
{

class CtElement : public IWORKXMLElementContextBase
{
public:
  explicit CtElement(IWORKXMLParserState &state);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
};

CtElement::CtElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

void CtElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::s | IWORKToken::NS_URI_SFA :
  case IWORKToken::string | IWORKToken::NS_URI_SFA :
    getState().m_tableData->m_content = value;
    getState().m_tableData->m_type = IWORK_CELL_TYPE_TEXT;
    break;
  default :
    ETONYEK_DEBUG_MSG(("CtElement::attribute[IWORKTabularModelElement.cpp]: find some unknown attribute\n"));
    break;
  }
}

IWORKXMLContextPtr_t CtElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::so | IWORKToken::NS_URI_SF :
  case IWORKToken::cell_storage | IWORKToken::NS_URI_SF :
    if (getState().m_tableData->m_content)
    {
      ETONYEK_DEBUG_MSG(("found a text cell with both simple and formatted content\n"));
    }
    return std::make_shared<IWORKTextStorageElement>(getState());
  default :
    ETONYEK_DEBUG_MSG(("CtElement::element[IWORKTabularModelElement.cpp]: find some unknown element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class NElement : public CellContextBase
{
public:
  explicit NElement(IWORKXMLParserState &state, bool isResult=false);

private:
  void attribute(int name, const char *value) override;
};

NElement::NElement(IWORKXMLParserState &state, bool isResult)
  : CellContextBase(state, isResult)
{
  getState().m_tableData->m_type = IWORK_CELL_TYPE_NUMBER;
}

void NElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::v | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_content = value;
    break;
  default :
    CellContextBase::attribute(name, value);
  }
}
}

namespace
{

class TElement : public CellContextBase
{
public:
  explicit TElement(IWORKXMLParserState &state, bool isResult=false);

private:
  void startOfElement() override;
  IWORKXMLContextPtr_t element(int name) override;
};

TElement::TElement(IWORKXMLParserState &state, bool isResult)
  : CellContextBase(state, isResult)
{
}

void TElement::startOfElement()
{
  if (isCollector() && !m_isResult)
  {
    // CHECKME: can we move this code in the constructor ?
    assert(!getState().m_currentText);
    getState().m_currentText = getCollector().createText(getState().m_langManager, false);
  }
}

IWORKXMLContextPtr_t TElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::ct | IWORKToken::NS_URI_SF :
    if (m_isResult && !getState().m_currentText)
      getState().m_currentText = getCollector().createText(getState().m_langManager, false);
    return std::make_shared<CtElement>(getState());
  default:
    break;
  }

  return CellContextBase::element(name);
}
}

namespace
{

class RbElement : public CellContextBase
{
public:
  explicit RbElement(IWORKXMLParserState &state);

private:
  void attribute(int name, const char *value) override;
};

RbElement::RbElement(IWORKXMLParserState &state)
  : CellContextBase(state, true)
{
  getState().m_tableData->m_type = IWORK_CELL_TYPE_BOOL;
}

void RbElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::v | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_content = value;
    break;
  default :
    CellContextBase::attribute(name, value);
  }
}
}

namespace
{

class RElement : public IWORKXMLElementContextBase
{
public:
  explicit RElement(IWORKXMLParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
};

RElement::RElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t RElement::element(int name)
{
  switch (name)
  {
  case IWORKToken::rb | IWORKToken::NS_URI_SF :
    return std::make_shared<RbElement>(getState());
    break;
  case IWORKToken::rd | IWORKToken::NS_URI_SF :
    return std::make_shared<DElement>(getState(), true);
    break;
  case IWORKToken::rn | IWORKToken::NS_URI_SF :
    return std::make_shared<NElement>(getState(), true);
    break;
  case IWORKToken::rt | IWORKToken::NS_URI_SF :
    return std::make_shared<TElement>(getState(), true);
    break;
  default:
    break;
  }

  ETONYEK_DEBUG_MSG(("RElement::element: found unexpected element\n"));
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class FElement : public CellContextBase
{
public:
  explicit FElement(IWORKXMLParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
};

FElement::FElement(IWORKXMLParserState &state)
  : CellContextBase(state)
{
}

IWORKXMLContextPtr_t FElement::element(int name)
{
  switch (name)
  {
  case IWORKToken::fo | IWORKToken::NS_URI_SF :
    return std::make_shared<IWORKFoElement>(getState());
  case IWORKToken::of | IWORKToken::NS_URI_SF :
    return std::make_shared<IWORKOfElement>(getState());
  case IWORKToken::r | IWORKToken::NS_URI_SF :
    return std::make_shared<RElement>(getState());
  default:
    break;
  }

  ETONYEK_DEBUG_MSG(("FElement::element: found unexpected element\n"));
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class GElement : public CellContextBase
{
public:
  explicit GElement(IWORKXMLParserState &state);
};

GElement::GElement(IWORKXMLParserState &state)
  : CellContextBase(state)
{
}

}

namespace
{

class GroupingElement : public CellContextBase
{
public:
  explicit GroupingElement(IWORKXMLParserState &state);
private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
};

GroupingElement::GroupingElement(IWORKXMLParserState &state)
  : CellContextBase(state)
{
}

void GroupingElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::group_formula_string | IWORKToken::NS_URI_SF : // value
  case IWORKToken::group_formula_value | IWORKToken::NS_URI_SF : // value
  case IWORKToken::group_formula_value_valid | IWORKToken::NS_URI_SF : // true or false
    break;
  default :
    CellContextBase::attribute(name, value);
  }
}

IWORKXMLContextPtr_t GroupingElement::element(int name)
{
  switch (name)
  {
  case IWORKToken::groupings_element | IWORKToken::NS_URI_SF :
  {
    static bool first=true;
    if (first)
    {
      first=false;
      ETONYEK_DEBUG_MSG(("GroupingElement::element: oops, find some grouping elements\n"));
    }
    return IWORKXMLContextPtr_t();
  }
  case IWORKToken::fo | IWORKToken::NS_URI_SF :
    return std::make_shared<IWORKFoElement>(getState());
  default:
    break;
  }

  return CellContextBase::element(name);
}
}

namespace
{

struct CellData
{
  boost::optional<std::string> m_content;
  boost::optional<IWORKDateTimeData> m_dateTime;
  IWORKFormulaPtr_t m_formula;
  IWORKStylePtr_t m_style;
  IWORKCellType m_type;
  IWORKTextPtr_t m_text;
};
}

namespace
{
class PmElement : public CellContextBase
{
public:
  explicit PmElement(IWORKXMLParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  std::map<ID_t,CellData> m_contentMap;
  boost::optional<ID_t> m_ref;
};

PmElement::PmElement(IWORKXMLParserState &state)
  : CellContextBase(state)
  , m_contentMap()
  , m_ref()
{
}

}

namespace
{
class TElementInMenu : public TElement
{
public:
  explicit TElementInMenu(IWORKXMLParserState &state, std::map<ID_t,CellData> &contentMap);

private:
  void endOfElement() override;

private:
  std::map<ID_t,CellData> &m_contentMap;
};

TElementInMenu::TElementInMenu(IWORKXMLParserState &state, std::map<ID_t,CellData> &contentMap)
  : TElement(state)
  , m_contentMap(contentMap)
{
}

void TElementInMenu::endOfElement()
{
  const IWORKTableDataPtr_t tableData = getState().m_tableData;
  if (getId())
  {
    // save data
    auto &data=m_contentMap[*getId()];
    data.m_content=tableData->m_content;
    data.m_dateTime=tableData->m_dateTime;
    data.m_formula=tableData->m_formula;
    data.m_type=tableData->m_type;
    data.m_text=getState().m_currentText;
  }

  // reset content attributes
  getState().m_currentText.reset();
  tableData->m_content.reset();
  tableData->m_dateTime.reset();
  tableData->m_formula.reset();
  tableData->m_style.reset();
  tableData->m_type = IWORK_CELL_TYPE_TEXT;
}
}

namespace
{

class MenuChoicesElement : public IWORKXMLElementContextBase
{
public:
  explicit MenuChoicesElement(IWORKXMLParserState &state, std::map<ID_t,CellData> &contentMap);

private:
  IWORKXMLContextPtr_t element(int name) override;

private:
  std::map<ID_t,CellData> &m_contentMap;
};

MenuChoicesElement::MenuChoicesElement(IWORKXMLParserState &state, std::map<ID_t,CellData> &contentMap)
  : IWORKXMLElementContextBase(state)
  , m_contentMap(contentMap)
{
}

IWORKXMLContextPtr_t MenuChoicesElement::element(int name)
{
  if (name == (IWORKToken::t | IWORKToken::NS_URI_SF))
    return std::make_shared<TElementInMenu>(getState(), m_contentMap);

  return IWORKXMLContextPtr_t();
}

}

namespace
{
IWORKXMLContextPtr_t PmElement::element(int name)
{
  switch (name)
  {
  case IWORKToken::menu_choices | IWORKToken::NS_URI_SF :
    return std::make_shared<MenuChoicesElement>(getState(), m_contentMap);
    break;
  case IWORKToken::proxied_cell_ref | IWORKToken::NS_URI_SF :
    return std::make_shared<IWORKRefContext>(getState(), m_ref);
    break;
  default:
    ETONYEK_DEBUG_MSG(("PmElement::element[IWORKTabularModelElement.cpp]: found unexpected element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}

void PmElement::endOfElement()
{
  if (m_ref)
  {
    auto const it = m_contentMap.find(get(m_ref));
    if (m_contentMap.end() != it)
    {
      // restore data
      const IWORKTableDataPtr_t tableData = getState().m_tableData;
      auto const &data=it->second;
      tableData->m_content=data.m_content;
      tableData->m_dateTime=data.m_dateTime;
      tableData->m_formula=data.m_formula;
      tableData->m_type=data.m_type;
      getState().m_currentText=data.m_text;
    }
    else
    {
      ETONYEK_DEBUG_MSG(("PmElement::endOfElement[IWORKTabularModelElement.cpp]: can not found %s references\n", m_ref->c_str()));
    }
  }
  CellContextBase::endOfElement();
}
}

namespace
{

class SElement : public CellContextBase
{
public:
  explicit SElement(IWORKXMLParserState &state);

private:
  void attribute(int name, const char *value) override;
  void endOfElement() override;
};

SElement::SElement(IWORKXMLParserState &state)
  : CellContextBase(state)
{
}

void SElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::ct | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_cellMove = lexical_cast<unsigned>(value);
    break;
  case IWORKToken::ho | IWORKToken::NS_URI_SF : // offset to main cell
  case IWORKToken::vo | IWORKToken::NS_URI_SF :
    break;
  default :
    CellContextBase::attribute(name,value);
  }
}

void SElement::endOfElement()
{
  emitCell(true);
}

}

namespace
{

class SlElement : public CellContextBase
{
public:
  explicit SlElement(IWORKXMLParserState &state);

private:
  void attribute(int name, const char *value) override;
};

SlElement::SlElement(IWORKXMLParserState &state)
  : CellContextBase(state)
{
  getState().m_tableData->m_type = IWORK_CELL_TYPE_NUMBER;
}

void SlElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::v | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_content = value;
    break;
  case IWORKToken::increment | IWORKToken::NS_URI_SF :
  case IWORKToken::min_value | IWORKToken::NS_URI_SF :
  case IWORKToken::max_value | IWORKToken::NS_URI_SF :
  case IWORKToken::slider_orientation | IWORKToken::NS_URI_SF :
  case IWORKToken::slider_position | IWORKToken::NS_URI_SF :
    break;
  default :
    CellContextBase::attribute(name, value);
  }
}

}

namespace
{

class StElement : public CellContextBase
{
public:
  explicit StElement(IWORKXMLParserState &state);

private:
  void attribute(int name, const char *value) override;
};

StElement::StElement(IWORKXMLParserState &state)
  : CellContextBase(state)
{
  getState().m_tableData->m_type = IWORK_CELL_TYPE_NUMBER;
}

void StElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::v | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_content = value;
    break;
  case IWORKToken::increment | IWORKToken::NS_URI_SF :
  case IWORKToken::min_value | IWORKToken::NS_URI_SF :
  case IWORKToken::max_value | IWORKToken::NS_URI_SF :
    break;
  default :
    CellContextBase::attribute(name, value);
  }
}

}

namespace
{
class ContentSizeElement : public IWORKXMLElementContextBase
{
public:
  explicit ContentSizeElement(IWORKXMLParserState &state);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
};

ContentSizeElement::ContentSizeElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

void ContentSizeElement::attribute(const int name, const char *const /*value*/)
{
  switch (name)
  {
  case IWORKToken::h | IWORKToken::NS_URI_SFA : // horizontal size
  case IWORKToken::w | IWORKToken::NS_URI_SFA : // vertical size
    break;
  default :
    ETONYEK_DEBUG_MSG(("ContentSizeElement::attribute[IWORKTabularModelElement.cpp]: found unexpected attribute\n"));
    break;
  }
}

#ifndef DEBUG
IWORKXMLContextPtr_t ContentSizeElement::element(int)
#else
IWORKXMLContextPtr_t ContentSizeElement::element(int name)
#endif
{
  ETONYEK_DEBUG_MSG(("ContentSizeElement::element[IWORKTabularModelElement.cpp]: found unexpected element %d\n", name));
  return IWORKXMLContextPtr_t();
}
}

namespace
{

class GenericCellElement : public IWORKXMLEmptyContextBase
{
public:
  explicit GenericCellElement(IWORKXMLParserState &state, bool isResult=false);
protected:
  void emitCell(const bool covered);

  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

protected:
  bool m_isResult;
private:
  boost::optional<std::string> m_id;
  boost::optional<ID_t> m_styleRef;
};

GenericCellElement::GenericCellElement(IWORKXMLParserState &state, bool isResult)
  : IWORKXMLEmptyContextBase(state)
  , m_isResult(isResult)
  , m_id()
  , m_styleRef()
{
}

void GenericCellElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::ID | IWORKToken::NS_URI_SFA :
    if (!m_isResult)
    {
      ETONYEK_DEBUG_MSG(("GenericCellElement::attribute: found unexpected id field\n"));
    }
    m_id=value;
    break;
  case IWORKToken::col | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_column = (unsigned) int_cast(value);
    break;
  case IWORKToken::flags | IWORKToken::NS_URI_SF : // find 4 ?
    break;
  case IWORKToken::row | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_row = (unsigned) int_cast(value);
    break;
  case IWORKToken::col_span | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_columnSpan = lexical_cast<unsigned>(value);
    break;
  case IWORKToken::row_span | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_rowSpan = lexical_cast<unsigned>(value);
    break;
  default :
    ETONYEK_DEBUG_MSG(("GenericCellElement::attribute[IWORKTabularModelElement.cpp]: found unexpected attribute\n"));
  }
}

IWORKXMLContextPtr_t GenericCellElement::element(int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::cell_style_ref :
    return std::make_shared<IWORKRefContext>(getState(), m_styleRef);
  case IWORKToken::NS_URI_SF | IWORKToken::content_size :
    return std::make_shared<ContentSizeElement>(getState());
  default:
    break;
  }
  ETONYEK_DEBUG_MSG(("GenericCellElement::element[IWORKTabularModelElement.cpp]: found unexpected element\n"));
  return IWORKXMLContextPtr_t();
}

void GenericCellElement::endOfElement()
{
  if (!m_isResult)
    emitCell(false);
}

void GenericCellElement::emitCell(const bool covered)
{
  // determine the style
  if (m_styleRef)
    getState().m_tableData->m_style = getState().getStyleByName(get(m_styleRef).c_str(), getState().getDictionary().m_cellStyles);

  const IWORKTableDataPtr_t tableData = getState().m_tableData;
  assert(tableData->m_columnSizes.size() > tableData->m_column);
  assert(tableData->m_rowSizes.size() > tableData->m_row);

  // send the cell to collector
  if (bool(getState().m_currentTable))
  {
    if (covered)
    {
      getState().m_currentTable->insertCoveredCell(tableData->m_column, tableData->m_row);
    }
    else
    {
      IWORKTextPtr_t text(getState().m_currentText);
      getState().m_currentText.reset();
      if (bool(tableData->m_content) && tableData->m_type == IWORK_CELL_TYPE_TEXT)
      {
        text = getCollector().createText(getState().m_langManager);
        text->insertText(get(tableData->m_content));
        text->flushParagraph();
      }
      getState().m_currentTable->insertCell(
        tableData->m_column, tableData->m_row,
        tableData->m_content, text, tableData->m_dateTime,
        get_optional_value_or(tableData->m_columnSpan, 1), get_optional_value_or(tableData->m_rowSpan, 1),
        tableData->m_formula, tableData->m_formulaHC, tableData->m_style, tableData->m_type
      );
    }
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

class BoolCellElement : public GenericCellElement
{
public:
  explicit BoolCellElement(IWORKXMLParserState &state, bool isResult=false);
private:
  void attribute(int name, const char *value) override;
};

BoolCellElement::BoolCellElement(IWORKXMLParserState &state, bool isResult)
  : GenericCellElement(state, isResult)
{
  if (!m_isResult)
    getState().m_tableData->m_type = IWORK_CELL_TYPE_BOOL;
}

void BoolCellElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::value | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_type = IWORK_CELL_TYPE_BOOL;
    getState().m_tableData->m_content = value;
    break;
  default :
    GenericCellElement::attribute(name, value);
  }
}

}

namespace
{
class DateCellElement : public GenericCellElement
{
public:
  explicit DateCellElement(IWORKXMLParserState &state, bool isResult=false);

private:
  void attribute(int name, const char *value) override;
};

DateCellElement::DateCellElement(IWORKXMLParserState &state, bool isResult)
  : GenericCellElement(state, isResult)
{
  if (!m_isResult)
    getState().m_tableData->m_type = IWORK_CELL_TYPE_DATE_TIME;
}

void DateCellElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::cell_date | IWORKToken::NS_URI_SF :
  {
    IWORKDateTimeData time;
    if (value && sscanf(value,"%d-%d-%dT%d:%d:%f",&time.m_year, &time.m_month, &time.m_day, &time.m_hour, &time.m_minute, &time.m_second)==6)
    {
      getState().m_tableData->m_type = IWORK_CELL_TYPE_DATE_TIME;
      getState().m_tableData->m_dateTime = time;
    }
    else
    {
      ETONYEK_DEBUG_MSG(("DateCellElement::attribute: can not convert %s\n", value));
    }
    break;
  }
  default:
    return GenericCellElement::attribute(name,value);
  }
}
}

namespace
{
class NumberCellElement : public GenericCellElement
{
public:
  explicit NumberCellElement(IWORKXMLParserState &state, bool isResult=false);

private:
  void attribute(int name, const char *value) override;
};

NumberCellElement::NumberCellElement(IWORKXMLParserState &state, bool isResult)
  : GenericCellElement(state, isResult)
{
  if (!m_isResult)
    getState().m_tableData->m_type = IWORK_CELL_TYPE_NUMBER;
}

void NumberCellElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::value | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_type = IWORK_CELL_TYPE_NUMBER;
    getState().m_tableData->m_content = value;
    break;
  default:
    return GenericCellElement::attribute(name,value);
  }
}
}

namespace
{
class SpanCellElement : public GenericCellElement
{
public:
  explicit SpanCellElement(IWORKXMLParserState &state);

private:
  void attribute(int name, const char *value) override;
  void endOfElement() override;
};

SpanCellElement::SpanCellElement(IWORKXMLParserState &state)
  : GenericCellElement(state)
{
}

void SpanCellElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::horizontal_offset | IWORKToken::NS_URI_SF :
  case IWORKToken::vertical_offset | IWORKToken::NS_URI_SF :
    break;
  default:
    return GenericCellElement::attribute(name,value);
  }
}

void SpanCellElement::endOfElement()
{
  emitCell(true);
}
}

namespace
{
class TextCellElement : public GenericCellElement
{
public:
  explicit TextCellElement(IWORKXMLParserState &state, bool isResult=false);

private:
  void startOfElement() override;
  IWORKXMLContextPtr_t element(int name) override;
};

TextCellElement::TextCellElement(IWORKXMLParserState &state, bool isResult)
  : GenericCellElement(state, isResult)
{
}

void TextCellElement::startOfElement()
{
  if (isCollector())
  {
    // CHECKME: can we move this code in the constructor ?
    assert(!getState().m_currentText);
    getState().m_currentText = getCollector().createText(getState().m_langManager, false);
  }
}

IWORKXMLContextPtr_t TextCellElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::cell_text | IWORKToken::NS_URI_SF :
    return std::make_shared<CtElement>(getState());
  default:
    break;
  }

  return GenericCellElement::element(name);
}

}

namespace
{
class ResultCellElement : public IWORKXMLEmptyContextBase
{
public:
  explicit ResultCellElement(IWORKXMLParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;

  boost::optional<ID_t> m_resultRef;
};

ResultCellElement::ResultCellElement(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
  , m_resultRef()
{
}

IWORKXMLContextPtr_t ResultCellElement::element(int name)
{
  switch (name)
  {
  case IWORKToken::result_bool_cell | IWORKToken::NS_URI_SF :
    return std::make_shared<BoolCellElement>(getState(), true);
  case IWORKToken::result_date_cell | IWORKToken::NS_URI_SF :
    return std::make_shared<DateCellElement>(getState(), true);
  case IWORKToken::result_number_cell | IWORKToken::NS_URI_SF :
    return std::make_shared<NumberCellElement>(getState(), true);
  case IWORKToken::result_text_cell | IWORKToken::NS_URI_SF :
    return std::make_shared<TextCellElement>(getState(), true);
  default:
    break;
  }
  ETONYEK_DEBUG_MSG(("ResultCellElement::element[IWORKTabularModelElement.cpp]: found unexpected element\n"));
  return IWORKXMLContextPtr_t();
}

}

namespace
{
class FormulaCellElement : public GenericCellElement
{
public:
  explicit FormulaCellElement(IWORKXMLParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
};

FormulaCellElement::FormulaCellElement(IWORKXMLParserState &state)
  : GenericCellElement(state)
{
}

IWORKXMLContextPtr_t FormulaCellElement::element(int name)
{
  switch (name)
  {
  case IWORKToken::formula | IWORKToken::NS_URI_SF :
    return std::make_shared<IWORKFormulaElement>(getState());
  case IWORKToken::result_cell | IWORKToken::NS_URI_SF :
    return std::make_shared<ResultCellElement>(getState());
  default:
    break;
  }

  return GenericCellElement::element(name);
}
}

namespace
{

class DatasourceElement : public IWORKXMLElementContextBase
{
public:
  explicit DatasourceElement(IWORKXMLParserState &state);

private:
  void startOfElement() override;
  IWORKXMLContextPtr_t element(int name) override;
};

DatasourceElement::DatasourceElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
  // these must be defined before datasource, otherwise we have a problem
  assert(!getState().m_tableData->m_columnSizes.empty());
  assert(!getState().m_tableData->m_rowSizes.empty());
}

void DatasourceElement::startOfElement()
{
  if (bool(getState().m_currentTable))
  {
    getState().m_currentTable->setSizes(getState().m_tableData->m_columnSizes, getState().m_tableData->m_rowSizes);
    getState().m_currentTable->setBorders(getState().m_tableData->m_verticalLines, getState().m_tableData->m_horizontalLines);
  }
}

IWORKXMLContextPtr_t DatasourceElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::cb | IWORKToken::NS_URI_SF :
    return std::make_shared<CbElement>(getState());
  case IWORKToken::d | IWORKToken::NS_URI_SF :
    return std::make_shared<DElement>(getState());
  case IWORKToken::du | IWORKToken::NS_URI_SF :
    return std::make_shared<DuElement>(getState());
  case IWORKToken::f | IWORKToken::NS_URI_SF :
    return std::make_shared<FElement>(getState());
  case IWORKToken::g | IWORKToken::NS_URI_SF :
    return std::make_shared<GElement>(getState());
  case IWORKToken::grouping | IWORKToken::NS_URI_SF :
    return std::make_shared<GroupingElement>(getState());
  case IWORKToken::n | IWORKToken::NS_URI_SF :
    return std::make_shared<NElement>(getState());
  case IWORKToken::pm | IWORKToken::NS_URI_SF :
    return std::make_shared<PmElement>(getState());
  case IWORKToken::s | IWORKToken::NS_URI_SF :
    return std::make_shared<SElement>(getState());
  case IWORKToken::sl | IWORKToken::NS_URI_SF :
    return std::make_shared<SlElement>(getState());
  case IWORKToken::st | IWORKToken::NS_URI_SF :
    return std::make_shared<StElement>(getState());
  case IWORKToken::t | IWORKToken::NS_URI_SF :
    return std::make_shared<TElement>(getState());
  case IWORKToken::date_cell | IWORKToken::NS_URI_SF :
    return std::make_shared<DateCellElement>(getState());
  case IWORKToken::generic_cell | IWORKToken::NS_URI_SF :
    return std::make_shared<GenericCellElement>(getState());
  case IWORKToken::formula_cell | IWORKToken::NS_URI_SF :
    return std::make_shared<FormulaCellElement>(getState());
  case IWORKToken::number_cell | IWORKToken::NS_URI_SF :
    return std::make_shared<NumberCellElement>(getState());
  case IWORKToken::span_cell | IWORKToken::NS_URI_SF :
    return std::make_shared<SpanCellElement>(getState());
  case IWORKToken::text_cell | IWORKToken::NS_URI_SF :
    return std::make_shared<TextCellElement>(getState());
  default:
    break;
  }

  ETONYEK_DEBUG_MSG(("DatasourceElement::element[IWORKTabularModelElement.cpp]: found unexpected element\n"));
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class VectorStyleRefElement : public IWORKXMLEmptyContextBase
{
public:
  explicit VectorStyleRefElement(IWORKXMLParserState &state, IWORKGridLine_t &line);

private:
  void attribute(int name, const char *value) override;
  void endOfElement() override;

private:
  IWORKGridLine_t &m_line;
  optional<unsigned> m_startIndex;
  optional<unsigned> m_stopIndex;
};

VectorStyleRefElement::VectorStyleRefElement(IWORKXMLParserState &state, IWORKGridLine_t &line)
  : IWORKXMLEmptyContextBase(state)
  , m_line(line)
  , m_startIndex()
  , m_stopIndex()
{
}

void VectorStyleRefElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::start_index :
    m_startIndex = int_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::stop_index :
    m_stopIndex = int_cast(value);
    break;
  default:
    IWORKXMLEmptyContextBase::attribute(name, value);
  }
}

void VectorStyleRefElement::endOfElement()
{
  if (getRef() && m_startIndex && m_stopIndex)
  {
    IWORKStylePtr_t style= getState().getStyleByName(get(getRef()).c_str(), getState().getDictionary().m_vectorStyles);
    if (style) m_line.insert_back(m_startIndex.get(), m_stopIndex.get(), style);
  }
}

}

namespace
{

class StyleRunElement : public IWORKXMLElementContextBase
{
public:
  explicit StyleRunElement(IWORKXMLParserState &state, IWORKGridLineMap_t &gridLines, unsigned maxLines);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  IWORKGridLineMap_t &m_gridLines;
  IWORKGridLine_t m_line;
  optional<unsigned> m_gridlineIndex_;
  optional<unsigned> m_count;
};

StyleRunElement::StyleRunElement(IWORKXMLParserState &state, IWORKGridLineMap_t &gridLines, unsigned maxLines)
  : IWORKXMLElementContextBase(state)
  , m_gridLines(gridLines)
  , m_line(0, maxLines, IWORKStylePtr_t())
  , m_gridlineIndex_()
  , m_count()
{
}

void StyleRunElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::gridline_index :
    m_gridlineIndex_=int_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::count : // number of element
    m_count=int_cast(value);
    break;
  default :
    ETONYEK_DEBUG_MSG(("StyleRunElement::attribute[IWORKTabularModelElement.cpp]: find some unknown attribute\n"));
    break;
  }
}

IWORKXMLContextPtr_t StyleRunElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::vector_style_ref | IWORKToken::NS_URI_SF :
    return std::make_shared<VectorStyleRefElement>(getState(), m_line);
  default:
    ETONYEK_DEBUG_MSG(("StyleRunElement::element[IWORKTabularModelElement.cpp]: find some unknown element\n"));
  }

  return IWORKXMLContextPtr_t();
}

void StyleRunElement::endOfElement()
{
  if (m_gridlineIndex_==boost::none)
  {
    ETONYEK_DEBUG_MSG(("StyleRunElement::endOfElement: can not find the line index\n"));
    unsigned lineId=0;
    if (!m_gridLines.empty())
    {
      IWORKGridLineMap_t::const_iterator it=m_gridLines.end();
      lineId=(--it)->first+1;
    }
    m_gridLines.insert(IWORKGridLineMap_t::value_type(lineId,m_line));
  }
  else
  {
    if (m_gridLines.find(*m_gridlineIndex_)!=m_gridLines.end())
    {
      ETONYEK_DEBUG_MSG(("StyleRunElement::endOfElement: oops, line index=%d is already defined\n", int(*m_gridlineIndex_)));
    }
    else
      m_gridLines.insert(IWORKGridLineMap_t::value_type(*m_gridlineIndex_,m_line));
  }
}

}


namespace
{

class GridlineElement : public IWORKXMLElementContextBase
{
public:
  explicit GridlineElement(IWORKXMLParserState &state, IWORKGridLineMap_t &gridLines, unsigned maxLines);

private:
  IWORKXMLContextPtr_t element(int name) override;
private:
  IWORKGridLineMap_t &m_gridLines;
  unsigned m_maxLines;
};

GridlineElement::GridlineElement(IWORKXMLParserState &state, IWORKGridLineMap_t &gridLines, unsigned maxLines)
  : IWORKXMLElementContextBase(state)
  , m_gridLines(gridLines)
  , m_maxLines(maxLines)
{
}

IWORKXMLContextPtr_t GridlineElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::style_run | IWORKToken::NS_URI_SF :
    return std::make_shared<StyleRunElement>(getState(), m_gridLines, m_maxLines);
  default:
    ETONYEK_DEBUG_MSG(("GridlineElement::element[IWORKTabularModelElement.cpp]: find some unknown element\n"));
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class GridRowElement : public IWORKXMLEmptyContextBase
{
public:
  explicit GridRowElement(IWORKXMLParserState &state);

private:
  void attribute(int name, const char *value) override;
};

GridRowElement::GridRowElement(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
{
}

void GridRowElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::height | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_rowSizes.push_back(IWORKColumnRowSize(lexical_cast<double>(value)));
    break;
  case IWORKToken::fitting_height | IWORKToken::NS_URI_SF :
  case IWORKToken::manually_sized | IWORKToken::NS_URI_SF :
  case IWORKToken::nc | IWORKToken::NS_URI_SF : // look like the number of cells in row...
  case IWORKToken::ncoc | IWORKToken::NS_URI_SF :
  case IWORKToken::nsc | IWORKToken::NS_URI_SF : // look like the number of special cells in row, ie covered cell
  case IWORKToken::preferred_height | IWORKToken::NS_URI_SF :
    break;
  default :
    ETONYEK_DEBUG_MSG(("GridRowElement::attribute[IWORKTabularModelElement.cpp]: find some unknown attribute\n"));
    break;
  }
}

}

namespace
{

class RowsElement : public IWORKXMLElementContextBase
{
public:
  explicit RowsElement(IWORKXMLParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
};

RowsElement::RowsElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
  assert(getState().m_tableData->m_rowSizes.empty());
}

IWORKXMLContextPtr_t RowsElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::grid_row | IWORKToken::NS_URI_SF :
    return std::make_shared<GridRowElement>(getState());
  default:
    ETONYEK_DEBUG_MSG(("RowsElement::element[IWORKTabularModelElement.cpp]: find some unknown element\n"));
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class GridElement : public IWORKXMLElementContextBase
{
public:
  explicit GridElement(IWORKXMLParserState &state);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
};

GridElement::GridElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

void GridElement::attribute(const int name, const char *value)
{
  switch (name)
  {
  case IWORKToken::numcols | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_numColumns = (unsigned) int_cast(value);
    break;
  case IWORKToken::numrows | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_numRows = (unsigned) int_cast(value);
    break;
  default :
    // also hiddennumcols hiddennumrows ncc ocnt
    break;
  }
}

IWORKXMLContextPtr_t GridElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::columns | IWORKToken::NS_URI_SF :
    return std::make_shared<ColumnsElement>(getState());
  case IWORKToken::datasource | IWORKToken::NS_URI_SF :
    return std::make_shared<DatasourceElement>(getState());
  case IWORKToken::rows | IWORKToken::NS_URI_SF :
    return std::make_shared<RowsElement>(getState());
  case IWORKToken::vertical_gridline_styles | IWORKToken::NS_URI_SF :
    return std::make_shared<GridlineElement>(getState(), getState().m_tableData->m_verticalLines, getState().m_tableData->m_numRows);
  case IWORKToken::horizontal_gridline_styles | IWORKToken::NS_URI_SF :
    return std::make_shared<GridlineElement>(getState(), getState().m_tableData->m_horizontalLines, getState().m_tableData->m_numColumns);
  default:
    ETONYEK_DEBUG_MSG(("GridElement::element[IWORKTabularModelElement.cpp]: find some unknown element\n"));
  }

  return IWORKXMLContextPtr_t();
}

}

IWORKTabularModelElement::IWORKTabularModelElement(IWORKXMLParserState &state, bool isDefinition)
  : IWORKXMLElementContextBase(state)
  , m_isDefinition(isDefinition)
  , m_id()
  , m_tableName()
  , m_tableId()
  , m_styleRef()
  , m_headerColumns()
  , m_headerRows()
  , m_footerRows()
  , m_coordinateCommentRefMap()
{
}

void IWORKTabularModelElement::attribute(const int name, const char *value)
{
  switch (name)
  {
  case IWORKToken::ID | IWORKToken::NS_URI_SFA :
    m_id=value;
    break;
  case IWORKToken::name | IWORKToken::NS_URI_SF :
    m_tableName = value;
    break;
  case IWORKToken::num_footer_rows | IWORKToken::NS_URI_SF :
    m_footerRows = try_int_cast(value);
    break;
  case IWORKToken::num_header_columns | IWORKToken::NS_URI_SF :
    m_headerColumns = try_int_cast(value);
    break;
  case IWORKToken::num_header_rows | IWORKToken::NS_URI_SF :
    m_headerRows = try_int_cast(value);
    break;
  case IWORKToken::id | IWORKToken::NS_URI_SF :
    m_tableId = "SFTGlobalID_" + string(value);
    break;
  default:
    // also name-is-visible,grouping-enabled,header-rows-frozen,header-columns-frozen
    break;
  }
}

void IWORKTabularModelElement::startOfElement()
{
  getState().m_tableData = std::make_shared<IWORKTableData>();
  if (m_isDefinition)
  {
    assert(!getState().m_currentTable);
    getState().m_currentTable = getCollector().createTable(getState().m_tableNameMap, getState().m_langManager);
  }
}

IWORKXMLContextPtr_t IWORKTabularModelElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::grid | IWORKToken::NS_URI_SF :
    return std::make_shared<GridElement>(getState());
  case IWORKToken::tabular_style_ref | IWORKToken::NS_URI_SF :
    return std::make_shared<IWORKRefContext>(getState(), m_styleRef);

  case IWORKToken::cell_comment_mapping | IWORKToken::NS_URI_SF :
    return std::make_shared<CellCommentMapping>(getState(), m_coordinateCommentRefMap);
  case IWORKToken::error_warning_mapping | IWORKToken::NS_URI_SF :
  case IWORKToken::filterset | IWORKToken::NS_URI_SF :
  case IWORKToken::grouping_order | IWORKToken::NS_URI_SF :
  case IWORKToken::grouping_state | IWORKToken::NS_URI_SF :
  case IWORKToken::sort | IWORKToken::NS_URI_SF :
    break;
  default:
    ETONYEK_DEBUG_MSG(("IWORKTabularModelElement::element: find some unknown element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}

void IWORKTabularModelElement::endOfElement()
{
  if (m_tableName && !get(m_tableName).empty())
  {
    auto &tableMap=*getState().m_tableNameMap;
    auto finalName=get(m_tableName);
    if (tableMap.find(finalName)!=tableMap.end())
    {
      ETONYEK_DEBUG_MSG(("IWORKTabularModelElement::endElement: a table with name %s already exists\n", finalName.c_str()));
      // let create an unique name
      int id=0;
      while (true)
      {
        std::stringstream s;
        s << finalName << "_" << ++id;
        if (tableMap.find(s.str())!=tableMap.end()) continue;
        finalName=s.str();
        break;
      }
    }
    tableMap[finalName]=finalName;
    if (m_tableId)
      tableMap[get(m_tableId)] = finalName;
    if (bool(getState().m_currentTable))
      getState().m_currentTable->setName(finalName);
  }
  if (bool(getState().m_currentTable))
  {
    IWORKStylePtr_t style;
    if (m_styleRef)
      style=getState().getStyleByName(get(m_styleRef).c_str(), getState().getDictionary().m_tabularStyles);
    sendStyle(style, getState().m_currentTable);
    getState().m_currentTable->setHeaders(
      get_optional_value_or(m_headerColumns, 0), get_optional_value_or(m_headerRows, 0),
      get_optional_value_or(m_footerRows, 0));
    if (!m_coordinateCommentRefMap.empty())
    {
      auto &dico=getState().getDictionary().m_tableComments;
      for (auto const &it : m_coordinateCommentRefMap)
      {
        auto dIt=dico.find(it.second);
        if (dIt==dico.end() || !dIt->second)
        {
          ETONYEK_DEBUG_MSG(("IWORKTabularModelElement::endElement: can not find comment with name %s\n", it.second.c_str()));
          continue;
        }
        IWORKOutputElements noteElements;
        // FIXME: we must set the correct sheetref before calling this
        dIt->second->draw(noteElements);
        getState().m_currentTable->setComment(it.first.first, it.first.second, noteElements);
      }
    }
  }
  if (!m_isDefinition)
    return;

  if (m_id)
    getState().getDictionary().m_tabulars[get(m_id)]=getState().m_currentTable;
  else
  {
    ETONYEK_DEBUG_MSG(("IWORKTabularModelElement::endOfElement: can not find the table id\n"));
  }
  getState().m_tableData.reset();
  getState().m_currentTable.reset();
}

void IWORKTabularModelElement::sendStyle(const IWORKStylePtr_t &style, const shared_ptr<IWORKTable> &table)
{
  assert(bool(table));

  table->setStyle(style);
  if (style)
  {
    using namespace property;
    if (style->has<SFTTableBandedRowsProperty>())
      table->setBandedRows(style->get<SFTTableBandedRowsProperty>());
    bool headerColumnRepeats = false;
    if (style->has<SFTHeaderColumnRepeatsProperty>())
      headerColumnRepeats = style->get<SFTHeaderColumnRepeatsProperty>();
    bool headerRowRepeats = false;
    if (style->has<SFTHeaderRowRepeatsProperty>())
      headerRowRepeats = style->get<SFTHeaderRowRepeatsProperty>();
    table->setRepeated(headerColumnRepeats, headerRowRepeats);
    if (style->has<SFTDefaultBodyCellStyleProperty>())
      table->setDefaultCellStyle(IWORKTable::CELL_TYPE_BODY, style->get<SFTDefaultBodyCellStyleProperty>());
    if (style->has<SFTDefaultHeaderRowCellStyleProperty>())
      table->setDefaultCellStyle(IWORKTable::CELL_TYPE_ROW_HEADER, style->get<SFTDefaultHeaderRowCellStyleProperty>());
    if (style->has<SFTDefaultHeaderColumnCellStyleProperty>())
      table->setDefaultCellStyle(IWORKTable::CELL_TYPE_COLUMN_HEADER, style->get<SFTDefaultHeaderColumnCellStyleProperty>());
    if (style->has<SFTDefaultFooterRowCellStyleProperty>())
      table->setDefaultCellStyle(IWORKTable::CELL_TYPE_ROW_FOOTER, style->get<SFTDefaultFooterRowCellStyleProperty>());
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
