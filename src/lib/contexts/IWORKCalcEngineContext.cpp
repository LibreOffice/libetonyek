/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKCalcEngineContext.h"

#include <cassert>

#include <boost/optional.hpp>

#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKTabularModelElement.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

namespace
{
class CalcEngineEntities : public IWORKXMLElementContextBase
{
public:
  explicit CalcEngineEntities(IWORKXMLParserState &state);

protected:
  IWORKXMLContextPtr_t element(int name) override;
};

CalcEngineEntities::CalcEngineEntities(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t CalcEngineEntities::element(const int name)
{
  switch (name)
  {
  case IWORKToken::tabular_model | IWORKToken::NS_URI_SF :
    return std::make_shared<IWORKTabularModelElement>(getState(), true);
  default:
    break;
  }
  ETONYEK_DEBUG_MSG(("CalcEngineEntities::element: found unexpected element\n"));
  return IWORKXMLContextPtr_t();
}

}

IWORKCalcEngineContext::IWORKCalcEngineContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t IWORKCalcEngineContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::calc_engine_entities | IWORKToken::NS_URI_SF :
    return std::make_shared<CalcEngineEntities>(getState());
  default:
    break;
  }
  ETONYEK_DEBUG_MSG(("IWORKCalcEngineContext::element: found unexpected element\n"));
  return IWORKXMLContextPtr_t();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
