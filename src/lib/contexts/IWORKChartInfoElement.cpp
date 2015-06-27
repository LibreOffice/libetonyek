/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKChartInfoElement.h"

// #include "IWORKFormula.h"
#include "IWORKGeometryElement.h"
// #include "IWORKStringElement.h"
#include "IWORKToken.h"


namespace libetonyek
{

namespace
{

// TODO: Handle as same in IWORKTabularInfoELement
class FoElement : public IWORKXMLElementContextBase
{
public:
  explicit FoElement(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
};

FoElement::FoElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

void FoElement::attribute(const int name, const char *)
{
  switch (name)
  {
  case IWORKToken::fs | IWORKToken::NS_URI_SF :
  {
    // IWORKFormula formula;
    // if (formula.parse(value))
    //   formula.write(getState().m_tableData->m_formula);
    // break;
  }
  default :
    break;
  }
}

}

namespace
{

class ChartFormulasElement : public IWORKXMLElementContextBase
{
public:
  explicit ChartFormulasElement(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

ChartFormulasElement::ChartFormulasElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t ChartFormulasElement::element(const int)
{
  // switch (name)
  // {
  // case IWORKToken::string | IWORKToken::NS_URI_SF :
  //   return makeContext<IWORKStringElement>(getState(), m_value);
  // }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class ChartRowColumnNamesElement : public IWORKXMLElementContextBase
{
public:
  explicit ChartRowColumnNamesElement(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

ChartRowColumnNamesElement::ChartRowColumnNamesElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t ChartRowColumnNamesElement::element(const int)
{
  // switch (name)
  // {
  // case IWORKToken::string | IWORKToken::NS_URI_SF :
  //   return makeContext<IWORKStringElement>(getState(), m_value);
  // }

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class FormulaChartModelElement : public IWORKXMLElementContextBase
{
public:
  explicit FormulaChartModelElement(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

FormulaChartModelElement::FormulaChartModelElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t FormulaChartModelElement::element(const int name)
{
  switch (name)
  {
  // case IWORKToken::chart_data | IWORKToken::NS_URI_SF :
  //   return makeContext<ChartDataElement>(getState());
  case IWORKToken::chart_column_names | IWORKToken::NS_URI_SF :
    return makeContext<ChartRowColumnNamesElement>(getState());
  case IWORKToken::chart_row_names | IWORKToken::NS_URI_SF :
    return makeContext<ChartRowColumnNamesElement>(getState());
  // case IWORKToken::chart_name | IWORKToken::NS_URI_SF :
  //   return makeContext<IWORKStringElement>(getState(), m_value);
  // case IWORKToken::value_title | IWORKToken::NS_URI_SF :
  //   return makeContext<IWORKStringElement>(getState(), m_value);
  // case IWORKToken::category_title | IWORKToken::NS_URI_SF :
  //   return makeContext<IWORKStringElement>(getState(), m_value);
  // case IWORKToken::entity_id | IWORKToken::NS_URI_SF :
  //   return makeContext<IWORKStringElement>(getState(), m_value);
  case IWORKToken::data_formulas | IWORKToken::NS_URI_SF :
    return makeContext<ChartFormulasElement>(getState());
  case IWORKToken::row_label_formulas | IWORKToken::NS_URI_SF :
    return makeContext<ChartFormulasElement>(getState());
  case IWORKToken::column_label_formulas | IWORKToken::NS_URI_SF :
    return makeContext<ChartFormulasElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}


namespace
{

class ChartModelObjectElement : public IWORKXMLElementContextBase
{
public:
  explicit ChartModelObjectElement(IWORKXMLParserState &state);

private:
  virtual IWORKXMLContextPtr_t element(int name);
};

ChartModelObjectElement::ChartModelObjectElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t ChartModelObjectElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::formula_chart_model | IWORKToken::NS_URI_SF :
    return makeContext<FormulaChartModelElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

}

IWORKChartInfoElement::IWORKChartInfoElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

void IWORKChartInfoElement::startOfElement()
{
}

void IWORKChartInfoElement::attribute(const int name, const char *)
{
  switch (name)
  {
  // case IWORKToken::chart_type | IWORKToken::NS_URI_SF :
  // break;
  }
}

IWORKXMLContextPtr_t IWORKChartInfoElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::geometry | IWORKToken::NS_URI_SF :
    return makeContext<IWORKGeometryElement>(getState());
    break;
  case IWORKToken::chart_model_object | IWORKToken::NS_URI_SF :
    return makeContext<ChartModelObjectElement>(getState());
  // case IWORKToken::wrap | IWORKToken::NS_URI_SF :
  //   return makeContext<WrapElement>(getState());
  // case IWORKToken::style | IWORKToken::NS_URI_SF :
  //   return makeContext<StyleElement>(getState());
  // case IWORKToken::chart_legend_info | IWORKToken::NS_URI_SF :
  //   return makeContext<ChartLegendInfoElement>(getState());
  // case IWORKToken::original_bounds | IWORKToken::NS_URI_SF :
  //   return makeContext<BoundsElement>(getState());
  // case IWORKToken::target_bounds | IWORKToken::NS_URI_SF :
  //   return makeContext<BoundsElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void IWORKChartInfoElement::endOfElement()
{
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
