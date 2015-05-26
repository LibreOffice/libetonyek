/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKTabularInfoElement.h"

#include <boost/lexical_cast.hpp>

#include "IWORKCollector.h"
#include "IWORKGeometryElement.h"
#include "IWORKTextBodyElement.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

using boost::lexical_cast;

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
    getData().m_columnSizes.push_back(lexical_cast<double>(value));
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
  assert(getData().m_columnSizes.empty());
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

class DElement : public CellContextBase
{
public:
  explicit DElement(IWORKXMLParserState &state);
};

DElement::DElement(IWORKXMLParserState &state)
  : CellContextBase(state)
{
}

}

namespace
{

class DuElement : public CellContextBase
{
public:
  explicit DuElement(IWORKXMLParserState &state);
};

DuElement::DuElement(IWORKXMLParserState &state)
  : CellContextBase(state)
{
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

IWORKXMLContextPtr_t FElement::element(int)
{
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
    getData().m_content = value;
    break;
  default :
    CellContextBase::attribute(name, value);
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
    getData().m_cellMove = lexical_cast<unsigned>(value);
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
    getData().m_content = value;
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
    if (getData().m_content)
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

class TElement : public CellContextBase
{
public:
  explicit TElement(IWORKXMLParserState &state);

private:
  virtual void startOfElement();
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
};

TElement::TElement(IWORKXMLParserState &state)
  : CellContextBase(state)
{
}

void TElement::startOfElement()
{
  getCollector()->startText();
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

void TElement::endOfElement()
{
  emitCell();

  getCollector()->endText();
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
  assert(!getData().m_columnSizes.empty());
  assert(!getData().m_rowSizes.empty());
}

void DatasourceElement::startOfElement()
{
  getCollector()->collectTableSizes(getData().m_rowSizes, getData().m_columnSizes);
}

IWORKXMLContextPtr_t DatasourceElement::element(const int name)
{
  switch (name)
  {
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
  case IWORKToken::s | IWORKToken::NS_URI_SF :
    return makeContext<SElement>(getState());
  case IWORKToken::t | IWORKToken::NS_URI_SF :
    return makeContext<TElement>(getState());
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
    getData().m_rowSizes.push_back(lexical_cast<double>(value));
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
  assert(getData().m_rowSizes.empty());
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
  virtual IWORKXMLContextPtr_t element(int name);
};

GridElement::GridElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
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
  virtual IWORKXMLContextPtr_t element(int name);
};

TabularModelElement::TabularModelElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
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
  getCollector()->startLevel();
}

IWORKXMLContextPtr_t IWORKTabularInfoElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::geometry | IWORKToken::NS_URI_SF :
    return makeContext<IWORKGeometryElement>(getState());
    break;
  case IWORKToken::tabular_model | IWORKToken::NS_URI_SF :
    return makeContext<TabularModelElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void IWORKTabularInfoElement::endOfElement()
{
  getCollector()->collectTable();

  getCollector()->endLevel();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
