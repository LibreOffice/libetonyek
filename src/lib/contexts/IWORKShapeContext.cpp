/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKShapeContext.h"

#include "IWORKCollector.h"
#include "IWORKGeometryElement.h"
#include "IWORKPathElement.h"
#include "IWORKTextElement.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKShapeContext::IWORKShapeContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

void IWORKShapeContext::startOfElement()
{
  if (isCollector())
  {
    getCollector().startLevel();
    getCollector().startText();
  }
}

IWORKXMLContextPtr_t IWORKShapeContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::path :
    return makeContext<IWORKPathElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::text :
    return makeContext<IWORKTextElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void IWORKShapeContext::endOfElement()
{
  if (isCollector())
  {
    getCollector().collectShape();
    getCollector().endText();
    getCollector().endLevel();
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
