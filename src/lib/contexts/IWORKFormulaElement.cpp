/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKFormulaElement.h"

#include "libetonyek_xml.h"
#include "IWORKDictionary.h"
#include "IWORKFormula.h"
#include "IWORKRefContext.h"
#include "IWORKStringElement.h"
#include "IWORKToken.h"
#include "IWORKTypes.h"

namespace libetonyek
{

namespace
{
class FmElement : public IWORKXMLEmptyContextBase
{
public:
  explicit FmElement(IWORKXMLParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
};

FmElement::FmElement(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
{
}

IWORKXMLContextPtr_t FmElement::element(int /*name*/)
{
  // TODO: sfa:pair as child
  static bool first=true;
  if (first)
  {
    ETONYEK_DEBUG_MSG(("FmElement::element: found some elements, ignored\n"));
    first=false;
  }

  return IWORKXMLContextPtr_t();
}
}

namespace
{
class HostCellIdElement : public IWORKXMLEmptyContextBase
{
public:
  explicit HostCellIdElement(IWORKXMLParserState &state);

private:
  void attribute(int name, const char *value) override;
  void endOfElement() override;

  boost::optional<unsigned> m_col, m_row;
};

HostCellIdElement::HostCellIdElement(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
  , m_col()
  , m_row()
{
}

void HostCellIdElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::column | IWORKToken::NS_URI_SF:
    m_col = int_cast(value);
    break;
  case IWORKToken::row | IWORKToken::NS_URI_SF:
    m_row = int_cast(value);
    break;
  default :
    IWORKXMLEmptyContextBase::attribute(name, value);
  }
}

void HostCellIdElement::endOfElement()
{
  if (!m_col || !m_row)
  {
    ETONYEK_DEBUG_MSG(("HostCellIdElement::endOfElement: can not find cell position\n"));
    return;
  }
  getState().m_tableData->m_formulaHC = 256*get(m_row)+get(m_col);
}
}

namespace
{
class CellAddressElement : public IWORKXMLEmptyContextBase
{
public:
  explicit CellAddressElement(IWORKXMLParserState &state);

private:
  void attribute(int name, const char *value) override;
  void endOfElement() override;

  boost::optional<unsigned> m_x, m_y;
};

CellAddressElement::CellAddressElement(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
  , m_x()
  , m_y()
{
}

void CellAddressElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::x_coordinate | IWORKToken::NS_URI_SF:
    m_x = int_cast(value);
    break;
  case IWORKToken::y_coordinate | IWORKToken::NS_URI_SF:
    m_y = int_cast(value);
    break;
  default :
    IWORKXMLEmptyContextBase::attribute(name, value);
  }
}

void CellAddressElement::endOfElement()
{
  if (!m_x || !m_y)
  {
    ETONYEK_DEBUG_MSG(("CellAddressElement::endOfElement: can not find cell position\n"));
    return;
  }
  getState().m_tableData->m_formulaHC = 256*get(m_y)+get(m_x);
}
}

// FoElement
IWORKFoElement::IWORKFoElement(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
  , m_id()
  , m_formula()
  , m_hc()
{
}

const boost::optional<ID_t> &IWORKFoElement::getId() const
{
  return m_id;
}

void IWORKFoElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SFA | IWORKToken::ID:
    m_id = value;
    break;
  case IWORKToken::fs | IWORKToken::NS_URI_SF :
    m_formula=value;
    break;
  case IWORKToken::hc | IWORKToken::NS_URI_SF :
    m_hc=int_cast(value);
    break;
  case IWORKToken::ht | IWORKToken::NS_URI_SF : // big string
    break;
  default :
    IWORKXMLEmptyContextBase::attribute(name, value);
    break;
  }
}

IWORKXMLContextPtr_t IWORKFoElement::element(int name)
{
  switch (name)
  {
  case IWORKToken::fm | IWORKToken::NS_URI_SF :
    return makeContext<FmElement>(getState());
    break;
  default:
    break;
  }

  ETONYEK_DEBUG_MSG(("IWORKFoElement::element: find unknown element %d\n", name));
  return IWORKXMLContextPtr_t();
}

void IWORKFoElement::endOfElement()
{
  if (!m_formula) return;

  IWORKFormulaPtr_t formula(new IWORKFormula(m_hc));
  if (!formula->parse(get(m_formula)))
    return;
  getState().m_tableData->m_formula = formula;
  getState().m_tableData->m_formulaHC = m_hc;
  if (getId())
    getState().getDictionary().m_formulas[get(getId())]=formula;
}

// OfElement
IWORKOfElement::IWORKOfElement(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
  , m_ref()
  , m_hc()
{
}

void IWORKOfElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::hc | IWORKToken::NS_URI_SF :
    m_hc=int_cast(value);
    break;
  default :
    IWORKXMLEmptyContextBase::attribute(name, value);
    break;
  }
}

IWORKXMLContextPtr_t IWORKOfElement::element(int name)
{
  switch (name)
  {
  case IWORKToken::fm | IWORKToken::NS_URI_SF :
    return makeContext<FmElement>(getState());
    break;
  case IWORKToken::mf_ref | IWORKToken::NS_URI_SF :
    return makeContext<IWORKRefContext>(getState(), m_ref);
  default:
    break;
  }

  ETONYEK_DEBUG_MSG(("IWORKOfElement::element: find unknown element %d\n", name));
  return IWORKXMLContextPtr_t();
}

void IWORKOfElement::endOfElement()
{
  if (!m_ref)
  {
    ETONYEK_DEBUG_MSG(("IWORKOfElement::endOfElement: can not find the ref\n"));
    return;
  }
  const IWORKFormulaMap_t::const_iterator it = getState().getDictionary().m_formulas.find(get(m_ref));
  if (it==getState().getDictionary().m_formulas.end())
  {
    ETONYEK_DEBUG_MSG(("IWORKOfElement::endOfElement: can not find the ref %s\n", get(m_ref).c_str()));
    return;
  }
  getState().m_tableData->m_formula = it->second;
  getState().m_tableData->m_formulaHC = m_hc;
}

// formula
IWORKFormulaElement::IWORKFormulaElement(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
  , m_id()
  , m_formula()
  , m_hc()
{
}

const boost::optional<ID_t> &IWORKFormulaElement::getId() const
{
  return m_id;
}

void IWORKFormulaElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SFA | IWORKToken::ID:
    m_id = value;
    break;
  default :
    IWORKXMLEmptyContextBase::attribute(name, value);
    break;
  }
}

IWORKXMLContextPtr_t IWORKFormulaElement::element(int name)
{
  switch (name)
  {
  case IWORKToken::formula_string | IWORKToken::NS_URI_SF :
    return makeContext<IWORKStringElement>(getState(), m_formula);
  case IWORKToken::host_cell_ID | IWORKToken::NS_URI_SF :
    return makeContext<HostCellIdElement>(getState());
  case IWORKToken::host_table_ID | IWORKToken::NS_URI_SF :
    break;
  default:
    ETONYEK_DEBUG_MSG(("IWORKFormulaElement::element: find unknown element %d\n", name));
  }

  return IWORKXMLContextPtr_t();
}

void IWORKFormulaElement::endOfElement()
{
  if (!m_formula) return;

  IWORKFormulaPtr_t formula(new IWORKFormula(getState().m_tableData->m_formulaHC));
  if (!formula->parse(get(m_formula)))
    return;
  getState().m_tableData->m_formula = formula;
  if (getId())
    getState().getDictionary().m_formulas[get(getId())]=formula;
}

// TableCellFormula
IWORKTableCellFormulaElement::IWORKTableCellFormulaElement(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
  , m_id()
  , m_formula()
  , m_tableId()
{
}

const boost::optional<ID_t> &IWORKTableCellFormulaElement::getId() const
{
  return m_id;
}

void IWORKTableCellFormulaElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SFA | IWORKToken::ID:
    m_id = value;
    break;
  default :
    IWORKXMLEmptyContextBase::attribute(name, value);
    break;
  }
}

IWORKXMLContextPtr_t IWORKTableCellFormulaElement::element(int name)
{
  switch (name)
  {
  case IWORKToken::formula_string | IWORKToken::NS_URI_SF :
    return makeContext<IWORKStringElement>(getState(), m_formula);
  case IWORKToken::cell_address | IWORKToken::NS_URI_SF :
    return makeContext<CellAddressElement>(getState());
  case IWORKToken::host_table_ID | IWORKToken::NS_URI_SF :
    return makeContext<IWORKStringElement>(getState(), m_tableId);
  default:
    ETONYEK_DEBUG_MSG(("IWORKTableCellFormulaElement::element: find unknown element %d\n", name));
  }

  return IWORKXMLContextPtr_t();
}

void IWORKTableCellFormulaElement::endOfElement()
{
  if (!m_formula) return;

  IWORKFormulaPtr_t formula(new IWORKFormula(getState().m_tableData->m_formulaHC));
  if (!formula->parse(get(m_formula)))
    return;
  getState().m_tableData->m_formula = formula;
  if (getId())
    getState().getDictionary().m_formulas[get(getId())]=formula;
}
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
