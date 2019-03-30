/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKChartInfoElement.h"

#include "IWORKCollector.h"
#include "IWORKGeometryElement.h"
#include "IWORKNumberElement.h"
#include "IWORKStringElement.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

#include "libetonyek_xml.h"


namespace libetonyek
{

namespace
{

class MutableArrayElement : public IWORKXMLElementContextBase
{
public:
  explicit MutableArrayElement(IWORKXMLParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
};

MutableArrayElement::MutableArrayElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t MutableArrayElement::element(const int)
{
  return IWORKXMLContextPtr_t();
}


}

namespace
{

class CachedDataElement : public IWORKXMLElementContextBase
{
public:
  explicit CachedDataElement(IWORKXMLParserState &state);

private:
  IWORKXMLContextPtr_t element(int name) override;
};

CachedDataElement::CachedDataElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t CachedDataElement::element(const int name)
{
  if (name == (IWORKToken::mutable_array | IWORKToken::NS_URI_SF))
    return std::make_shared<MutableArrayElement>(getState());

  return IWORKXMLContextPtr_t();
}

}

namespace
{

class ChartRowColumnNamesElement : public IWORKXMLElementContextBase
{
public:
  explicit ChartRowColumnNamesElement(IWORKXMLParserState &state, std::deque<std::string> &rowColumnNames);

private:
  IWORKXMLContextPtr_t element(int name) override;

private:
  std::deque<std::string> m_rowColumnNames;
};

ChartRowColumnNamesElement::ChartRowColumnNamesElement(IWORKXMLParserState &state, std::deque<std::string> &rowColumnNames)
  : IWORKXMLElementContextBase(state)
  , m_rowColumnNames(rowColumnNames)
{
}

IWORKXMLContextPtr_t ChartRowColumnNamesElement::element(const int /*name*/)
{
  return IWORKXMLContextPtr_t();
}

}

namespace
{

class FormulaChartModelElement : public IWORKXMLElementContextBase
{
public:
  explicit FormulaChartModelElement(IWORKXMLParserState &state, IWORKChart &chart);

private:
  IWORKXMLContextPtr_t element(int name) override;

private:
  IWORKChart m_chart;
};

FormulaChartModelElement::FormulaChartModelElement(IWORKXMLParserState &state, IWORKChart &chart)
  : IWORKXMLElementContextBase(state)
  , m_chart(chart)
{
}

IWORKXMLContextPtr_t FormulaChartModelElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::chart_column_names | IWORKToken::NS_URI_SF :
    return std::make_shared<ChartRowColumnNamesElement>(getState(), m_chart.m_columnNames);
  case IWORKToken::chart_row_names | IWORKToken::NS_URI_SF :
    return std::make_shared<ChartRowColumnNamesElement>(getState(), m_chart.m_rowNames);
  case IWORKToken::chart_name | IWORKToken::NS_URI_SF :
    return std::make_shared<IWORKStringElement>(getState(), m_chart.m_chartName);
  case IWORKToken::value_title | IWORKToken::NS_URI_SF :
    return std::make_shared<IWORKStringElement>(getState(), m_chart.m_valueTitle);
  case IWORKToken::category_title | IWORKToken::NS_URI_SF :
    return std::make_shared<IWORKStringElement>(getState(), m_chart.m_categoryTitle);
  case IWORKToken::cached_data | IWORKToken::NS_URI_SF :
    return std::make_shared<CachedDataElement>(getState());
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

}


namespace
{

class ChartModelObjectElement : public IWORKXMLElementContextBase
{
public:
  explicit ChartModelObjectElement(IWORKXMLParserState &state, IWORKChart &chart);

private:
  IWORKXMLContextPtr_t element(int name) override;

private:
  IWORKChart m_chart;
};

ChartModelObjectElement::ChartModelObjectElement(IWORKXMLParserState &state, IWORKChart &chart)
  : IWORKXMLElementContextBase(state)
  , m_chart(chart)
{
}

IWORKXMLContextPtr_t ChartModelObjectElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::formula_chart_model | IWORKToken::NS_URI_SF :
    return std::make_shared<FormulaChartModelElement>(getState(), m_chart);
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

}

IWORKChartInfoElement::IWORKChartInfoElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_chart()
{
}

void IWORKChartInfoElement::startOfElement()
{
  if (!isCollector())
    return;
  getCollector().startLevel();
}

void IWORKChartInfoElement::attribute(const int name, const char *value)
{
  switch (name)
  {
  case IWORKToken::chart_type | IWORKToken::NS_URI_SF :
    m_chart.m_chartType = (unsigned) int_cast(value);
  default:
    break;
  }
}

IWORKXMLContextPtr_t IWORKChartInfoElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::geometry | IWORKToken::NS_URI_SF :
    return std::make_shared<IWORKGeometryElement>(getState());
  case IWORKToken::chart_model_object | IWORKToken::NS_URI_SF :
    return std::make_shared<ChartModelObjectElement>(getState(), m_chart);
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

void IWORKChartInfoElement::endOfElement()
{
  if (!isCollector())
    return;
  getCollector().endLevel();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
