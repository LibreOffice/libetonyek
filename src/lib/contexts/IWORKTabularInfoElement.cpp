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

#include <boost/lexical_cast.hpp>

#include "libetonyek_xml.h"
#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKFoElement.h"
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
using std::string;

namespace
{

class CellContextBase : public IWORKXMLEmptyContextBase
{
protected:
  explicit CellContextBase(IWORKXMLParserState &state);

  virtual void attribute(int name, const char *value);
  virtual void endOfElement();

  void emitCell(const bool covered = false);
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
    const IWORKStyleMap_t::const_iterator it = getState().getDictionary().m_cellStyles.find(value);
    if (getState().getDictionary().m_cellStyles.end() != it)
      getState().m_tableData->m_style = it->second;
    break;
  }
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
      // TODO: Handle simple text content already in parser, instead of passing it
      // into the collector.
      IWORKOutputElements elements;

      if (bool(tableData->m_content) && tableData->m_type == IWORK_CELL_TYPE_TEXT)
      {
        librevenge::RVNGPropertyList props;
        elements.addOpenParagraph(props);
        elements.addOpenSpan(props);
        elements.addInsertText(librevenge::RVNGString(get(tableData->m_content).c_str()));
        elements.addCloseSpan();
        elements.addCloseParagraph();
      }
      else if (bool(getState().m_currentText))
      {
        getState().m_currentText->draw(elements);
      }
      getState().m_currentText.reset();

      getState().m_currentTable->insertCell(
        tableData->m_column, tableData->m_row,
        tableData->m_content, elements,
        get_optional_value_or(tableData->m_columnSpan, 1), get_optional_value_or(tableData->m_rowSpan, 1),
        tableData->m_formula, tableData->m_style, tableData->m_type
      );
    }
  }

  // reset cell attributes
  tableData->m_columnSpan.reset();
  tableData->m_rowSpan.reset();
  tableData->m_cellMove.reset();
  tableData->m_content.reset();
  tableData->m_formula.reset();
  tableData->m_style.reset();
}

}

namespace
{

class GridColumnElement : public IWORKXMLEmptyContextBase
{
public:
  explicit GridColumnElement(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
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
  virtual IWORKXMLContextPtr_t element(int name);
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
  virtual void attribute(int name, const char *value);
};

CbElement::CbElement(IWORKXMLParserState &state)
  : CellContextBase(state)
{
}

void CbElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::v | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_content = value;
    getState().m_tableData->m_type = IWORK_CELL_TYPE_BOOL;
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
  virtual void attribute(int name, const char *value);
};

DElement::DElement(IWORKXMLParserState &state)
  : CellContextBase(state)
{
}

void DElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::cell_date | IWORKToken::NS_URI_SF :
  {
    std::time_t t = ETONYEK_EPOCH_BEGIN + lexical_cast<unsigned>(value);
    char time_buf[21];
    strftime(time_buf, 21, "%Y-%m-%dT%H:%S:%MZ", gmtime(&t));
    getState().m_tableData->m_content = value;
    getState().m_tableData->m_type = IWORK_CELL_TYPE_DATE_TIME;
    break;
  }
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
  virtual void attribute(int name, const char *value);
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
  virtual IWORKXMLContextPtr_t element(int name);
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
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
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

class RnElement : public IWORKXMLEmptyContextBase
{
public:
  explicit RnElement(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
};

RnElement::RnElement(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
{
}

void RnElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::v | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_content = value;
    getState().m_tableData->m_type = IWORK_CELL_TYPE_NUMBER;
    break;
  }
}

}

namespace
{

class RtElement : public IWORKXMLEmptyContextBase
{
public:
  explicit RtElement(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

RtElement::RtElement(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
{
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
  virtual IWORKXMLContextPtr_t element(int name);
};

RElement::RElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t RElement::element(int name)
{
  switch (name)
  {
  case IWORKToken::rn | IWORKToken::NS_URI_SF :
    return makeContext<RnElement>(getState());
    break;
  case IWORKToken::rt | IWORKToken::NS_URI_SF :
    return makeContext<RtElement>(getState());
    break;
  }

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
  virtual IWORKXMLContextPtr_t element(int name);
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
    return makeContext<IWORKFoElement>(getState());
    break;
  case IWORKToken::r | IWORKToken::NS_URI_SF :
    return makeContext<RElement>(getState());
    break;
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
  virtual void attribute(int name, const char *value);
};

NElement::NElement(IWORKXMLParserState &state)
  : CellContextBase(state)
{
}

void NElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::v | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_content = value;
    getState().m_tableData->m_type = IWORK_CELL_TYPE_NUMBER;
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
  virtual void attribute(int name, const char *value);

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
  virtual IWORKXMLContextPtr_t element(int name);

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
  virtual IWORKXMLContextPtr_t element(int name);

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
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

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
  virtual void attribute(int name, const char *value);
  virtual void endOfElement();
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
  virtual void attribute(int name, const char *value);
};

SlElement::SlElement(IWORKXMLParserState &state)
  : CellContextBase(state)
{
}

void SlElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::v | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_content = value;
    getState().m_tableData->m_type = IWORK_CELL_TYPE_NUMBER;
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
}

void StElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::v | IWORKToken::NS_URI_SF :
    getState().m_tableData->m_content = value;
    getState().m_tableData->m_type = IWORK_CELL_TYPE_NUMBER;
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
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
};

TElement::TElement(IWORKXMLParserState &state)
  : CellContextBase(state)
{
}

void TElement::startOfElement()
{
  if (isCollector())
  {
    // TODO: This will have to be moved to the parent class, so all
    // cells get the correct style, not only text cells.
    assert(!getState().m_currentText);
    getState().m_currentText = getCollector().createText(false);
    IWORKStyleStack styleStack;
    styleStack.push(getState().m_currentTable->getDefaultCellStyle(getState().m_tableData->m_column, getState().m_tableData->m_row));
    styleStack.push(getState().m_tableData->m_style);
    using namespace property;
    if (styleStack.has<SFTCellStylePropertyParagraphStyle>())
      getState().m_currentText->pushParagraphStyle(styleStack.get<SFTCellStylePropertyParagraphStyle>());
    if (styleStack.has<SFTCellStylePropertyLayoutStyle>())
      getState().m_currentText->pushLayoutStyle(styleStack.get<SFTCellStylePropertyLayoutStyle>());
  }
}

IWORKXMLContextPtr_t TElement::element(const int name)
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

class DatasourceElement : public IWORKXMLElementContextBase
{
public:
  explicit DatasourceElement(IWORKXMLParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
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
    return makeContext<SlElement>(getState());
  case IWORKToken::t | IWORKToken::NS_URI_SF :
    return makeContext<TElement>(getState());
  }

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
  virtual void attribute(int name, const char *value);
  virtual void endOfElement();

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
    const IWORKStyleMap_t::const_iterator it = getState().getDictionary().m_vectorStyles.find(get(getRef()));
    if (getState().getDictionary().m_vectorStyles.end() != it)
      m_line.insert_back(m_startIndex.get(), m_stopIndex.get(), it->second);
  }
}

}

namespace
{

class StyleRunElement : public IWORKXMLElementContextBase
{
public:
  explicit StyleRunElement(IWORKXMLParserState &state, IWORKGridLineList_t &gridLines, unsigned maxLines);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  IWORKGridLineList_t &m_gridLines;
  IWORKGridLine_t m_line;
};

StyleRunElement::StyleRunElement(IWORKXMLParserState &state, IWORKGridLineList_t &gridLines, unsigned maxLines)
  : IWORKXMLElementContextBase(state)
  , m_gridLines(gridLines)
  , m_line(0, maxLines, IWORKStylePtr_t())
{
}

void StyleRunElement::attribute(const int name, const char *const /*value*/)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::gridline_index :
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
  m_gridLines.push_back(m_line);
}

}


namespace
{

class GridlineElement : public IWORKXMLElementContextBase
{
public:
  explicit GridlineElement(IWORKXMLParserState &state, IWORKGridLineList_t &gridLines, unsigned maxLines);

private:
  virtual IWORKXMLContextPtr_t element(int name);
private:
  IWORKGridLineList_t &m_gridLines;
  unsigned m_maxLines;
};

GridlineElement::GridlineElement(IWORKXMLParserState &state, IWORKGridLineList_t &gridLines, unsigned maxLines)
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
  virtual void attribute(int name, const char *value);
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
  virtual IWORKXMLContextPtr_t element(int name);
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
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
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
  virtual void attribute(int name, const char *value);
  virtual void endOfAttributes();
  virtual IWORKXMLContextPtr_t element(int name);

private:
  boost::optional<string> m_tableName;
  boost::optional<string> m_tableId;

};

TabularModelElement::TabularModelElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

void TabularModelElement::attribute(const int name, const char *value)
{
  switch (name)
  {
  case IWORKToken::name | IWORKToken::NS_URI_SF :
    m_tableName = value;
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
  }

  return IWORKXMLContextPtr_t();
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
  getState().m_currentTable = getCollector().createTable(getState().m_tableNameMap);
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
