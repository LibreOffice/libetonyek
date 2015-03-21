/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKTabularInfoContext.h"

#include <boost/lexical_cast.hpp>

#include "IWORKCollector.h"
#include "IWORKToken.h"
#include "IWORKXMLContexts.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

using boost::lexical_cast;

namespace
{

struct TableData
{
  TableData();

  IWORKTable::ColumnSizes_t m_columnSizes;
  IWORKTable::RowSizes_t m_rowSizes;

  unsigned m_column;
  unsigned m_row;

  boost::optional<unsigned> m_columnSpan;
  boost::optional<unsigned> m_rowSpan;
  boost::optional<unsigned> m_cellMove;
  boost::optional<std::string> m_content;
};

TableData::TableData()
  : m_columnSizes()
  , m_rowSizes()
  , m_column(0)
  , m_row(0)
  , m_columnSpan()
  , m_rowSpan()
  , m_cellMove()
  , m_content()
{
}

}

namespace
{

class TableParserState
{
public:
  TableParserState(IWORKXMLParserState &state, TableData &data);

  IWORKXMLParserState &getState();
  TableData &getData();

private:
  IWORKXMLParserState &m_state;
  TableData &m_data;
};

TableParserState::TableParserState(IWORKXMLParserState &state, TableData &data)
  : m_state(state)
  , m_data(data)
{
}

IWORKXMLParserState &TableParserState::getState()
{
  return m_state;
}

TableData &TableParserState::getData()
{
  return m_data;
}

}

namespace
{

template<class BaseT>
class TableContextBase : public BaseT
{
public:
  explicit TableContextBase(TableParserState &state);

  TableParserState &getState();
  TableData &getData();

private:
  TableParserState &m_state;
};

template<class BaseT>
TableContextBase<BaseT>::TableContextBase(TableParserState &state)
  : BaseT(state.getState())
  , m_state(state)
{
}

template<class BaseT>
TableParserState &TableContextBase<BaseT>::getState()
{
  return m_state;
}

template<class BaseT>
TableData &TableContextBase<BaseT>::getData()
{
  return m_state.getData();
}

}

namespace
{

class CellContextBase : public TableContextBase<IWORKXMLEmptyContextBase>
{
protected:
  explicit CellContextBase(TableParserState &state);

  virtual void attribute(int name, const char *value);
  virtual void endOfElement();

  void emitCell(const bool covered = false);
};

CellContextBase::CellContextBase(TableParserState &state)
  : TableContextBase<IWORKXMLEmptyContextBase>(state)
{
}

void CellContextBase::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::col_span | IWORKToken::NS_URI_SF :
    getData().m_columnSpan = lexical_cast<unsigned>(value);
    break;
  case IWORKToken::ct | IWORKToken::NS_URI_SF :
    getData().m_cellMove = lexical_cast<unsigned>(value);
    break;
  case IWORKToken::row_span | IWORKToken::NS_URI_SF :
    getData().m_rowSpan = lexical_cast<unsigned>(value);
    break;
  }
}

void CellContextBase::endOfElement()
{
  emitCell();
}

void CellContextBase::emitCell(const bool covered)
{
  // determine the cell's position
  if (getData().m_cellMove)
  {
    const unsigned ct = get(getData().m_cellMove);
    if (0x80 > ct)
    {
      getData().m_column += ct;
    }
    else
    {
      ++getData().m_row;
      getData().m_column -= (0x100 - ct);
    }
  }
  else
  {
    ++getData().m_column;
    if (getData().m_columnSizes.size() == getData().m_column)
    {
      getData().m_column = 0;
      ++getData().m_row;
    }
  }
  assert(getData().m_columnSizes.size() > getData().m_column);
  assert(getData().m_rowSizes.size() > getData().m_row);

  // send the cell to collector
  if (covered)
    getCollector()->collectCoveredTableCell(getData().m_row, getData().m_column);
  else
    getCollector()->collectTableCell(getData().m_row, getData().m_column, getData().m_content, get_optional_value_or(getData().m_rowSpan, 1), get_optional_value_or(getData().m_columnSpan, 1));

  // reset cell attributes
  getData().m_columnSpan.reset();
  getData().m_rowSpan.reset();
  getData().m_cellMove.reset();
  getData().m_content.reset();
}

}

namespace
{

class GridColumnContext : public TableContextBase<IWORKXMLEmptyContextBase>
{
public:
  explicit GridColumnContext(TableParserState &state);

private:
  virtual void attribute(int name, const char *value);
};

GridColumnContext::GridColumnContext(TableParserState &state)
  : TableContextBase<IWORKXMLEmptyContextBase>(state)
{
}

void GridColumnContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::width | IWORKToken::NS_URI_SF :
    getData().m_columnSizes.push_back(lexical_cast<double>(value));
    break;
  default :
    break;
  }
}

}

namespace
{

class ColumnsContext : public TableContextBase<IWORKXMLElementContextBase>
{
public:
  explicit ColumnsContext(TableParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

ColumnsContext::ColumnsContext(TableParserState &state)
  : TableContextBase<IWORKXMLElementContextBase>(state)
{
  assert(getData().m_columnSizes.empty());
}

IWORKXMLContextPtr_t ColumnsContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::grid_column | IWORKToken::NS_URI_SF :
    return makeContext<GridColumnContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class DContext : public CellContextBase
{
public:
  explicit DContext(TableParserState &state);
};

DContext::DContext(TableParserState &state)
  : CellContextBase(state)
{
}

}

namespace
{

class DuContext : public CellContextBase
{
public:
  explicit DuContext(TableParserState &state);
};

DuContext::DuContext(TableParserState &state)
  : CellContextBase(state)
{
}

}

namespace
{

class FContext : public CellContextBase
{
public:
  explicit FContext(TableParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

FContext::FContext(TableParserState &state)
  : CellContextBase(state)
{
}

IWORKXMLContextPtr_t FContext::element(int)
{
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class GContext : public CellContextBase
{
public:
  explicit GContext(TableParserState &state);
};

GContext::GContext(TableParserState &state)
  : CellContextBase(state)
{
}

}

namespace
{

class NContext : public CellContextBase
{
public:
  explicit NContext(TableParserState &state);

private:
  virtual void attribute(int name, const char *value);
};

NContext::NContext(TableParserState &state)
  : CellContextBase(state)
{
}

void NContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::v | IWORKToken::NS_URI_SF :
    getData().m_content = value;
    break;
  default :
    CellContextBase::attribute(name, value);
  }
}

}

namespace
{

class SContext : public CellContextBase
{
public:
  explicit SContext(TableParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual void endOfElement();
};

SContext::SContext(TableParserState &state)
  : CellContextBase(state)
{
}

void SContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::ct | IWORKToken::NS_URI_SF :
    getData().m_cellMove = lexical_cast<unsigned>(value);
    break;
  default :
    break;
  }
}

void SContext::endOfElement()
{
  emitCell(true);
}

}

namespace
{

class SoContext : public TableContextBase<IWORKXMLElementContextBase>
{
public:
  explicit SoContext(TableParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

SoContext::SoContext(TableParserState &state)
  : TableContextBase<IWORKXMLElementContextBase>(state)
{
}

IWORKXMLContextPtr_t SoContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::text_body | IWORKToken::NS_URI_SF :
    return makeContext<IWORKTextBodyContext>(getState().getState());
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class CtContext : public TableContextBase<IWORKXMLElementContextBase>
{
public:
  explicit CtContext(TableParserState &state);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
};

CtContext::CtContext(TableParserState &state)
  : TableContextBase<IWORKXMLElementContextBase>(state)
{
}

void CtContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::s | IWORKToken::NS_URI_SF :
    getData().m_content = value;
    break;
  default :
    break;
  }
}

IWORKXMLContextPtr_t CtContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::so | IWORKToken::NS_URI_SF :
    if (getData().m_content)
    {
      ETONYEK_DEBUG_MSG(("found a text cell with both simple and formatted content\n"));
    }
    return makeContext<SoContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class TContext : public CellContextBase
{
public:
  explicit TContext(TableParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

TContext::TContext(TableParserState &state)
  : CellContextBase(state)
{
}

void TContext::startOfElement()
{
  getCollector()->startText(false);
}

IWORKXMLContextPtr_t TContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::ct | IWORKToken::NS_URI_SF :
    return makeContext<CtContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void TContext::endOfElement()
{
  emitCell();

  getCollector()->endText();
}

}

namespace
{

class DatasourceContext : public TableContextBase<IWORKXMLElementContextBase>
{
public:
  explicit DatasourceContext(TableParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
};

DatasourceContext::DatasourceContext(TableParserState &state)
  : TableContextBase<IWORKXMLElementContextBase>(state)
{
  // these must be defined before datasource, otherwise we have a problem
  assert(!getData().m_columnSizes.empty());
  assert(!getData().m_rowSizes.empty());
}

void DatasourceContext::startOfElement()
{
  getCollector()->collectTableSizes(getData().m_rowSizes, getData().m_columnSizes);
}

IWORKXMLContextPtr_t DatasourceContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::d | IWORKToken::NS_URI_SF :
    return makeContext<DContext>(getState());
  case IWORKToken::du | IWORKToken::NS_URI_SF :
    return makeContext<DuContext>(getState());
  case IWORKToken::f | IWORKToken::NS_URI_SF :
    return makeContext<FContext>(getState());
  case IWORKToken::g | IWORKToken::NS_URI_SF :
    return makeContext<GContext>(getState());
  case IWORKToken::n | IWORKToken::NS_URI_SF :
    return makeContext<NContext>(getState());
  case IWORKToken::s | IWORKToken::NS_URI_SF :
    return makeContext<SContext>(getState());
  case IWORKToken::t | IWORKToken::NS_URI_SF :
    return makeContext<TContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class GridRowContext : public TableContextBase<IWORKXMLEmptyContextBase>
{
public:
  explicit GridRowContext(TableParserState &state);

private:
  virtual void attribute(int name, const char *value);
};

GridRowContext::GridRowContext(TableParserState &state)
  : TableContextBase<IWORKXMLEmptyContextBase>(state)
{
}

void GridRowContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::height | IWORKToken::NS_URI_SF :
    getData().m_rowSizes.push_back(lexical_cast<double>(value));
    break;
  default :
    break;
  }
}

}

namespace
{

class RowsContext : public TableContextBase<IWORKXMLElementContextBase>
{
public:
  explicit RowsContext(TableParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

RowsContext::RowsContext(TableParserState &state)
  : TableContextBase<IWORKXMLElementContextBase>(state)
{
  assert(getData().m_rowSizes.empty());
}

IWORKXMLContextPtr_t RowsContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::grid_row | IWORKToken::NS_URI_SF :
    return makeContext<GridRowContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class GridContext : public TableContextBase<IWORKXMLElementContextBase>
{
public:
  explicit GridContext(TableParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

GridContext::GridContext(TableParserState &state)
  : TableContextBase<IWORKXMLElementContextBase>(state)
{
}

IWORKXMLContextPtr_t GridContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::columns | IWORKToken::NS_URI_SF :
    return makeContext<ColumnsContext>(getState());
  case IWORKToken::datasource | IWORKToken::NS_URI_SF :
    return makeContext<DatasourceContext>(getState());
  case IWORKToken::rows | IWORKToken::NS_URI_SF :
    return makeContext<RowsContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class TabularModelContext : public TableContextBase<IWORKXMLElementContextBase>
{
public:
  explicit TabularModelContext(TableParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

TabularModelContext::TabularModelContext(TableParserState &state)
  : TableContextBase<IWORKXMLElementContextBase>(state)
{
}

IWORKXMLContextPtr_t TabularModelContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::grid | IWORKToken::NS_URI_SF :
    return makeContext<GridContext>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}

struct IWORKTabularInfoContext::Impl
{
  explicit Impl(IWORKXMLParserState &state);

  TableData m_data;
  TableParserState m_state;
};

IWORKTabularInfoContext::Impl::Impl(IWORKXMLParserState &state)
  : m_data()
  , m_state(state, m_data)
{
}

IWORKTabularInfoContext::IWORKTabularInfoContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_impl(new Impl(state))
{
}

void IWORKTabularInfoContext::startOfElement()
{
  getCollector()->startLevel();
}

IWORKXMLContextPtr_t IWORKTabularInfoContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::geometry | IWORKToken::NS_URI_SF :
    return makeContext<IWORKGeometryContext>(getState());
    break;
  case IWORKToken::tabular_model | IWORKToken::NS_URI_SF :
    return makeContext<TabularModelContext>(m_impl->m_state);
  }

  return IWORKXMLContextPtr_t();
}

void IWORKTabularInfoContext::endOfElement()
{
  getCollector()->collectTable();

  getCollector()->endLevel();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
