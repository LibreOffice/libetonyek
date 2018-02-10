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
#include "IWORKToken.h"
#include "IWORKTypes.h"

namespace libetonyek
{

class IWORKFmElement : public IWORKXMLEmptyContextBase
{
public:
  explicit IWORKFmElement(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

IWORKFmElement::IWORKFmElement(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
{
}

IWORKXMLContextPtr_t IWORKFmElement::element(int /*name*/)
{
  // TODO: sfa:pair as child
  static bool first=true;
  if (first)
  {
    ETONYEK_DEBUG_MSG(("IWORKFmElement::element: found some elements, ignored\n"));
    first=false;
  }

  return IWORKXMLContextPtr_t();
}
}

namespace libetonyek
{

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

IWORKXMLContextPtr_t IWORKFormulaElement::element(int name)
{
  switch (name)
  {
  case IWORKToken::fm | IWORKToken::NS_URI_SF :
    return makeContext<IWORKFmElement>(getState());
    break;
  }

  ETONYEK_DEBUG_MSG(("IWORKFormulaElement::element: find unknown element %d\n", name));
  return IWORKXMLContextPtr_t();
}

void IWORKFormulaElement::endOfElement()
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
}

namespace libetonyek
{

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
    return makeContext<IWORKFmElement>(getState());
    break;
  case IWORKToken::mf_ref | IWORKToken::NS_URI_SF :
    return makeContext<IWORKRefContext>(getState(), m_ref);
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

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
