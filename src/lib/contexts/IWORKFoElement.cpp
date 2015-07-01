/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKFoElement.h"

namespace libetonyek
{

FoElement::FoElement(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
{
}

void FoElement::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::fs | IWORKToken::NS_URI_SF :
  {
    IWORKFormula formula;
    if (formula.parse(value))
      formula.write(getState().m_tableData->m_formula);
    break;
  }
  default :
    break;
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
