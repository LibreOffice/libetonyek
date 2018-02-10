/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKTabularInfoElement.h"

#include <cassert>
#include <ctime>
#include <memory>

#include <boost/lexical_cast.hpp>

#include "libetonyek_xml.h"
#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKFormulaElement.h"
#include "IWORKGeometryElement.h"
#include "IWORKProperties.h"
#include "IWORKRefContext.h"
#include "IWORKTable.h"
#include "IWORKText.h"
#include "IWORKTextBodyElement.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"
#include "IWORKXMLContextBase.h"
#include "IWORKStyle.h"


namespace libetonyek
{

using boost::lexical_cast;
using boost::optional;

using std::shared_ptr;
using std::string;

namespace
{
// condition
class CfElement : public IWORKXMLEmptyContextBase
{
public:
  explicit CfElement(IWORKXMLParserState &state);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
};

CfElement::CfElement(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
{
}

void CfElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::implicit_format_type | IWORKToken::NS_URI_SF :
  {
    // find 256
    break;
  }
  default :
    IWORKXMLEmptyContextBase::attribute(name, value);
  }
}

IWORKXMLContextPtr_t CfElement::element(int name)
{
  switch (name)
  {
  case IWORKToken::date_format | IWORKToken::NS_URI_SF :
  {
    // TODO: read a date-format elements here...
    static bool first=true;
    if (first)
    {
      ETONYEK_DEBUG_MSG(("CfElement::element: found a date format element\n"));
      first=false;
    }
    return IWORKXMLContextPtr_t();
  }
  case IWORKToken::number_format | IWORKToken::NS_URI_SF :
  {
    // TODO: read a number-format elements here...
    static bool first=true;
    if (first)
    {
      ETONYEK_DEBUG_MSG(("CfElement::element: found a number format element\n"));
      first=false;
    }
    return IWORKXMLContextPtr_t();
  }
  }
  return IWORKXMLEmptyContextBase::element(name);
}
}

namespace
{

class CellContextBase : public IWORKXMLEmptyContextBase
{
protected:
  explicit CellContextBase(IWORKXMLParserState &state);

  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

  void emitCell(const bool covered = false);

  boost::optional<ID_t> m_ref;
};

CellContextBase::CellContextBase(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
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
  }
}

IWORKXMLContextPtr_t CellContextBase::element(int name)
{
  switch (name)
  {
  case IWORKToken::cf | IWORKToken::NS_URI_SF :
    return makeContext<CfElement>(getState());
  case IWORKToken::cf_ref | IWORKToken::NS_URI_SF:
    return makeContext<IWORKRefContext>(getState(), m_ref);
  }
  return IWORKXMLEmptyContextBase::element(name);
}

void CellContextBase::endOfElement()
{
  emitCell();
}

void CellContextBase::emitCell(const bool covered)
{
  const IWORKTableDataPtr_t tableData = getState().m_tableData;

  // determine the cell's position
  if (tableData->m_cellMove)
  {
    const unsigned ct = get(tableData->m_cellMove);
    if (0x80 > ct)
    {
      tableData->m_column += ct;
    }
    else
    {
      ++tableData->m_row;
      tableData->m_column -= (0x100 - ct);
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
      IWORKTextPtr_t cellText(getState().m_currentText);
      getState().m_currentText.reset();
      if (bool(tableData->m_content) && tableData->m_type == IWORK_CELL_TYPE_TEXT)
      {
        cellText = getCollector().createText(getState().m_langManager);
        cellText->insertText(get(tableData->m_content));
        cellText->flushParagraph();
      }
      getState().m_currentTable->insertCell(
        tableData->m_column, tableData->m_row,
        tableData->m_content, cellText, tableData->m_dateTime,
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
};

GridColumnElement::GridColumnElement(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
{
}

void GridColumnElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::width | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_columnSizes.push_back(lexical_cast<double>(value));
    break;
  default :
    break;
  }
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
    return makeContext<GridColumnElement>(getState());
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
  explicit DElement(IWORKXMLParserState &state);

private:
  void attribute(int name, const char *value) override;
};

DElement::DElement(IWORKXMLParserState &state)
  : CellContextBase(state)
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

class SoElement : public IWORKXMLElementContextBase
{
public:
  explicit SoElement(IWORKXMLParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
};

SoElement::SoElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t SoElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::text_body | IWORKToken::NS_URI_SF :
    return makeContext<IWORKTextBodyElement>(getState());
  }

  return IWORKXMLContextPtr_t();
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
    return makeContext<SoElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class RbElement : public IWORKXMLEmptyContextBase
{
public:
  explicit RbElement(IWORKXMLParserState &state);

private:
  void attribute(int name, const char *value) override;
};

RbElement::RbElement(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
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
  }
}
}

namespace
{

class RnElement : public IWORKXMLEmptyContextBase
{
public:
  explicit RnElement(IWORKXMLParserState &state);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;

  boost::optional<ID_t> m_ref;
};

RnElement::RnElement(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
{
  getState().m_tableData->m_type = IWORK_CELL_TYPE_NUMBER;
}

void RnElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::v | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_content = value;
    break;
  }
}

IWORKXMLContextPtr_t RnElement::element(int name)
{
  switch (name)
  {
  case IWORKToken::cf | IWORKToken::NS_URI_SF :
    return makeContext<CfElement>(getState());
  case IWORKToken::cf_ref | IWORKToken::NS_URI_SF:
    return makeContext<IWORKRefContext>(getState(), m_ref);
  }
  return IWORKXMLEmptyContextBase::element(name);
}
}

namespace
{

class RtElement : public IWORKXMLEmptyContextBase
{
public:
  explicit RtElement(IWORKXMLParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
};

RtElement::RtElement(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
{
  getState().m_tableData->m_type = IWORK_CELL_TYPE_NUMBER;
}

IWORKXMLContextPtr_t RtElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::ct | IWORKToken::NS_URI_SF :
    return makeContext<CtElement>(getState());
  }

  return IWORKXMLContextPtr_t();
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
    return makeContext<RbElement>(getState());
    break;
  case IWORKToken::rn | IWORKToken::NS_URI_SF :
    return makeContext<RnElement>(getState());
    break;
  case IWORKToken::rt | IWORKToken::NS_URI_SF :
    return makeContext<RtElement>(getState());
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
    return makeContext<IWORKFormulaElement>(getState());
  case IWORKToken::of | IWORKToken::NS_URI_SF :
    return makeContext<IWORKOfElement>(getState());
  case IWORKToken::r | IWORKToken::NS_URI_SF :
    return makeContext<RElement>(getState());
  }

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

class NElement : public CellContextBase
{
public:
  explicit NElement(IWORKXMLParserState &state);

private:
  void attribute(int name, const char *value) override;
};

NElement::NElement(IWORKXMLParserState &state)
  : CellContextBase(state)
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

class PmCtElement : public IWORKXMLEmptyContextBase
{
public:
  explicit PmCtElement(IWORKXMLParserState &state, IWORKContentMap_t &contentMap, const boost::optional<std::string> &id);

private:
  void attribute(int name, const char *value) override;

private:
  IWORKContentMap_t &m_contentMap;
  const boost::optional<std::string> &m_id;
};

PmCtElement::PmCtElement(IWORKXMLParserState &state, IWORKContentMap_t &contentMap, const boost::optional<std::string> &id)
  : IWORKXMLEmptyContextBase(state)
  , m_contentMap(contentMap)
  , m_id(id)
{
}

void PmCtElement::attribute(const int name, const char *const value)
{
  if (name == (IWORKToken::s | IWORKToken::NS_URI_SFA) && m_id)
    m_contentMap[get(m_id)] = value;
}

}

namespace
{

class PmTElement : public IWORKXMLEmptyContextBase
{
public:
  explicit PmTElement(IWORKXMLParserState &state, IWORKContentMap_t &contentMap);

private:
  IWORKXMLContextPtr_t element(int name) override;

private:
  IWORKContentMap_t &m_contentMap;
};

PmTElement::PmTElement(IWORKXMLParserState &state, IWORKContentMap_t &contentMap)
  : IWORKXMLEmptyContextBase(state)
  , m_contentMap(contentMap)
{
}

IWORKXMLContextPtr_t PmTElement::element(const int name)
{
  if (name == (IWORKToken::ct | IWORKToken::NS_URI_SF))
    return makeContext<PmCtElement>(getState(), m_contentMap, getId());

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class MenuChoicesElement : public IWORKXMLElementContextBase
{
public:
  explicit MenuChoicesElement(IWORKXMLParserState &state, IWORKContentMap_t &contentMap);

private:
  IWORKXMLContextPtr_t element(int name) override;

private:
  IWORKContentMap_t &m_contentMap;
};

MenuChoicesElement::MenuChoicesElement(IWORKXMLParserState &state, IWORKContentMap_t &contentMap)
  : IWORKXMLElementContextBase(state)
  , m_contentMap(contentMap)
{
}

IWORKXMLContextPtr_t MenuChoicesElement::element(int name)
{
  if (name == (IWORKToken::t | IWORKToken::NS_URI_SF))
    return makeContext<PmTElement>(getState(), m_contentMap);

  return IWORKXMLContextPtr_t();
}

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
  IWORKContentMap_t m_contentMap;
  boost::optional<ID_t> m_ref;
};

PmElement::PmElement(IWORKXMLParserState &state)
  : CellContextBase(state)
  , m_contentMap()
  , m_ref()
{
}

IWORKXMLContextPtr_t PmElement::element(int name)
{
  switch (name)
  {
  case IWORKToken::menu_choices | IWORKToken::NS_URI_SF :
    return makeContext<MenuChoicesElement>(getState(), m_contentMap);
    break;
  case IWORKToken::proxied_cell_ref | IWORKToken::NS_URI_SF :
    return makeContext<IWORKRefContext>(getState(), m_ref);
    break;
  }

  return IWORKXMLContextPtr_t();
}

void PmElement::endOfElement()
{
  if (m_ref)
  {
    const IWORKContentMap_t::const_iterator it = m_contentMap.find(get(m_ref));
    if (m_contentMap.end() != it)
    {
      getState().m_tableData->m_content = it->second;
      getState().m_tableData->m_type = IWORK_CELL_TYPE_TEXT;
    }
  }
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
  default :
    break;
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
  virtual void attribute(int name, const char *value);
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
  explicit TElement(IWORKXMLParserState &state);

private:
  void startOfElement() override;
  IWORKXMLContextPtr_t element(int name) override;
};

TElement::TElement(IWORKXMLParserState &state)
  : CellContextBase(state)
{
}

void TElement::startOfElement()
{
  if (isCollector())
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
    return makeContext<CtElement>(getState());
  }

  return CellContextBase::element(name);
}

}

namespace
{
class ContentSizeElement : public IWORKXMLElementContextBase
{
public:
  explicit ContentSizeElement(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
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
    ETONYEK_DEBUG_MSG(("ContentSizeElement::attribute: found unexpected attribute\n"));
    break;
  }
}

#ifndef DEBUG
IWORKXMLContextPtr_t ContentSizeElement::element(int)
#else
IWORKXMLContextPtr_t ContentSizeElement::element(int name)
#endif
{
  ETONYEK_DEBUG_MSG(("ContentSizeElement::attribute: found unexpected element %d\n", name));
  return IWORKXMLContextPtr_t();
}
}

namespace
{

class GenericCellElement : public IWORKXMLEmptyContextBase
{
public:
  explicit GenericCellElement(IWORKXMLParserState &state);
protected:
  void emitCell(const bool covered);

  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  boost::optional<ID_t> m_styleRef;
};

GenericCellElement::GenericCellElement(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
  , m_styleRef()
{
}

void GenericCellElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::col | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_column = int_cast(value);
    break;
  case IWORKToken::row | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_row = int_cast(value);
    break;
  case IWORKToken::col_span | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_columnSpan = lexical_cast<unsigned>(value);
    break;
  case IWORKToken::row_span | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_rowSpan = lexical_cast<unsigned>(value);
    break;
  default :
    ETONYEK_DEBUG_MSG(("GenericCellElement::attribute: found unexpected attribute\n"));
  }
}

IWORKXMLContextPtr_t GenericCellElement::element(int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::cell_style_ref :
    return makeContext<IWORKRefContext>(getState(), m_styleRef);
  case IWORKToken::NS_URI_SF | IWORKToken::content_size :
    return makeContext<ContentSizeElement>(getState());
  }

  ETONYEK_DEBUG_MSG(("GenericCellElement::element: found unexpected element\n"));
  return IWORKXMLContextPtr_t();
}

void GenericCellElement::endOfElement()
{
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
class ResultCellElement : public IWORKXMLEmptyContextBase
{
public:
  explicit ResultCellElement(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);

  boost::optional<ID_t> m_resultRef;
};

ResultCellElement::ResultCellElement(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
{
}

IWORKXMLContextPtr_t ResultCellElement::element(int name)
{
  switch (name)
  {
  case IWORKToken::result_number_cell | IWORKToken::NS_URI_SF :
    return makeContext<IWORKRefContext>(getState(), m_resultRef);
  }
  ETONYEK_DEBUG_MSG(("ResultCellElement::element: found unexpected element\n"));
  return IWORKXMLContextPtr_t();
}

}

namespace
{
class DateCellElement : public GenericCellElement
{
public:
  explicit DateCellElement(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
};

DateCellElement::DateCellElement(IWORKXMLParserState &state)
  : GenericCellElement(state)
{
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
      getState().m_tableData->m_dateTime = time;
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
class FormulaCellElement : public GenericCellElement
{
public:
  explicit FormulaCellElement(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
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
    return makeContext<IWORKFormulaElement>(getState());
  case IWORKToken::result_cell | IWORKToken::NS_URI_SF :
    return makeContext<ResultCellElement>(getState());
  }

  return GenericCellElement::element(name);
}
}

namespace
{
class NumberCellElement : public GenericCellElement
{
public:
  explicit NumberCellElement(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
};

NumberCellElement::NumberCellElement(IWORKXMLParserState &state)
  : GenericCellElement(state)
{
  getState().m_tableData->m_type = IWORK_CELL_TYPE_NUMBER;
}

void NumberCellElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::value | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_content = value;
    break;
  default:
    return GenericCellElement::attribute(name,value);
  }
}
}

namespace
{
class TextCellElement : public GenericCellElement
{
public:
  explicit TextCellElement(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
};

TextCellElement::TextCellElement(IWORKXMLParserState &state)
  : GenericCellElement(state)
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

void TextCellElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::flags | IWORKToken::NS_URI_SF : // find 4 ?
    break;
  default:
    return GenericCellElement::attribute(name,value);
  }
}

IWORKXMLContextPtr_t TextCellElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::cell_text | IWORKToken::NS_URI_SF :
    return makeContext<CtElement>(getState());
  }

  return GenericCellElement::element(name);
}

}

namespace
{
class SpanCellElement : public GenericCellElement
{
public:
  explicit SpanCellElement(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual void endOfElement();
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
    return makeContext<CbElement>(getState());
  case IWORKToken::d | IWORKToken::NS_URI_SF :
    return makeContext<DElement>(getState());
  case IWORKToken::du | IWORKToken::NS_URI_SF :
    return makeContext<DuElement>(getState());
  case IWORKToken::f | IWORKToken::NS_URI_SF :
    return makeContext<FElement>(getState());
  case IWORKToken::g | IWORKToken::NS_URI_SF :
    return makeContext<GElement>(getState());
  case IWORKToken::n | IWORKToken::NS_URI_SF :
    return makeContext<NElement>(getState());
  case IWORKToken::pm | IWORKToken::NS_URI_SF :
    return makeContext<PmElement>(getState());
  case IWORKToken::s | IWORKToken::NS_URI_SF :
    return makeContext<SElement>(getState());
  case IWORKToken::sl | IWORKToken::NS_URI_SF :
    return makeContext<SlElement>(getState());
  case IWORKToken::st | IWORKToken::NS_URI_SF :
    return makeContext<StElement>(getState());
  case IWORKToken::t | IWORKToken::NS_URI_SF :
    return makeContext<TElement>(getState());
  case IWORKToken::date_cell | IWORKToken::NS_URI_SF :
    return makeContext<DateCellElement>(getState());
  case IWORKToken::generic_cell | IWORKToken::NS_URI_SF :
    return makeContext<GenericCellElement>(getState());
  case IWORKToken::formula_cell | IWORKToken::NS_URI_SF :
    return makeContext<FormulaCellElement>(getState());
  case IWORKToken::number_cell | IWORKToken::NS_URI_SF :
    return makeContext<NumberCellElement>(getState());
  case IWORKToken::span_cell | IWORKToken::NS_URI_SF :
    return makeContext<SpanCellElement>(getState());
  case IWORKToken::text_cell | IWORKToken::NS_URI_SF :
    return makeContext<TextCellElement>(getState());
  }

  ETONYEK_DEBUG_MSG(("DatasourceElement::element: found unexpected element\n"));
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
{
}

void VectorStyleRefElement::attribute(const int name, const char *const value)
{
  IWORKXMLEmptyContextBase::attribute(name, value);
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::start_index :
    m_startIndex = int_cast(value);
    break;
  case IWORKToken::NS_URI_SF | IWORKToken::stop_index :
    m_stopIndex = int_cast(value);
    break;
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
  optional<unsigned> m_gridlineIndex;
};

StyleRunElement::StyleRunElement(IWORKXMLParserState &state, IWORKGridLineMap_t &gridLines, unsigned maxLines)
  : IWORKXMLElementContextBase(state)
  , m_gridLines(gridLines)
  , m_line(0, maxLines, IWORKStylePtr_t())
  , m_gridlineIndex()
{
}

void StyleRunElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::gridline_index :
    m_gridlineIndex=int_cast(value);
    break;
  default :
    break;
  }
}

IWORKXMLContextPtr_t StyleRunElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::vector_style_ref | IWORKToken::NS_URI_SF :
    return makeContext<VectorStyleRefElement>(getState(), m_line);
  }

  return IWORKXMLContextPtr_t();
}

void StyleRunElement::endOfElement()
{
  if (!m_gridlineIndex)
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
    if (m_gridLines.find(*m_gridlineIndex)!=m_gridLines.end())
    {
      ETONYEK_DEBUG_MSG(("StyleRunElement::endOfElement: oops, line index=%d is already defined\n", int(*m_gridlineIndex)));
    }
    else
      m_gridLines.insert(IWORKGridLineMap_t::value_type(*m_gridlineIndex,m_line));
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
    return makeContext<StyleRunElement>(getState(), m_gridLines, m_maxLines);
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
    getState().m_tableData->m_rowSizes.push_back(lexical_cast<double>(value));
    break;
  default :
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
    return makeContext<GridRowElement>(getState());
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
    getState().m_tableData->m_numColumns = int_cast(value);
    break;
  case IWORKToken::numrows | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_numRows = int_cast(value);
    break;
  default :
    break;
  }
}

IWORKXMLContextPtr_t GridElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::columns | IWORKToken::NS_URI_SF :
    return makeContext<ColumnsElement>(getState());
  case IWORKToken::datasource | IWORKToken::NS_URI_SF :
    return makeContext<DatasourceElement>(getState());
  case IWORKToken::rows | IWORKToken::NS_URI_SF :
    return makeContext<RowsElement>(getState());
  case IWORKToken::vertical_gridline_styles | IWORKToken::NS_URI_SF :
    return makeContext<GridlineElement>(getState(), getState().m_tableData->m_verticalLines, getState().m_tableData->m_numRows);
  case IWORKToken::horizontal_gridline_styles | IWORKToken::NS_URI_SF :
    return makeContext<GridlineElement>(getState(), getState().m_tableData->m_horizontalLines, getState().m_tableData->m_numColumns);
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class TabularModelElement : public IWORKXMLElementContextBase
{
public:
  explicit TabularModelElement(IWORKXMLParserState &state);

private:
  void attribute(int name, const char *value) override;
  void endOfAttributes() override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  void sendStyle(const IWORKStylePtr_t &style, const shared_ptr<IWORKTable> &table);

private:
  boost::optional<string> m_tableName;
  boost::optional<string> m_tableId;
  boost::optional<ID_t> m_styleRef;
  boost::optional<int> m_headerColumns;
  boost::optional<int> m_headerRows;
  boost::optional<int> m_footerRows;
};

TabularModelElement::TabularModelElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_tableName()
  , m_tableId()
  , m_styleRef()
  , m_headerColumns()
  , m_headerRows()
  , m_footerRows()
{
}

void TabularModelElement::attribute(const int name, const char *value)
{
  switch (name)
  {
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
  }
}

void TabularModelElement::endOfAttributes()
{
  if (m_tableId && m_tableName)
    (*(getState().m_tableNameMap))[get(m_tableId)] = get(m_tableName);
}

IWORKXMLContextPtr_t TabularModelElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::grid | IWORKToken::NS_URI_SF :
    return makeContext<GridElement>(getState());
  case IWORKToken::tabular_style_ref | IWORKToken::NS_URI_SF :
    return makeContext<IWORKRefContext>(getState(), m_styleRef);
  }

  return IWORKXMLContextPtr_t();
}

void TabularModelElement::endOfElement()
{
  if (bool(getState().m_currentTable))
  {
    IWORKStylePtr_t style;
    if (m_styleRef)
      style=getState().getStyleByName(get(m_styleRef).c_str(), getState().getDictionary().m_tabularStyles);
    sendStyle(style, getState().m_currentTable);
    getState().m_currentTable->setHeaders(
      get_optional_value_or(m_headerColumns, 0), get_optional_value_or(m_headerRows, 0),
      get_optional_value_or(m_footerRows, 0));
  }
}

void TabularModelElement::sendStyle(const IWORKStylePtr_t &style, const shared_ptr<IWORKTable> &table)
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

IWORKTabularInfoElement::IWORKTabularInfoElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

void IWORKTabularInfoElement::startOfElement()
{
  getState().m_tableData.reset(new IWORKTableData());
  assert(!getState().m_currentTable);
  getState().m_currentTable = getCollector().createTable(getState().m_tableNameMap, getState().m_langManager);
  if (isCollector())
    getCollector().startLevel();
}

IWORKXMLContextPtr_t IWORKTabularInfoElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::geometry | IWORKToken::NS_URI_SF :
    return makeContext<IWORKGeometryElement>(getState());
  case IWORKToken::tabular_model | IWORKToken::NS_URI_SF :
    return makeContext<TabularModelElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void IWORKTabularInfoElement::endOfElement()
{
  if (isCollector())
  {
    getCollector().collectTable(getState().m_currentTable);
    getState().m_currentTable.reset();

    getCollector().endLevel();
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
