/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKGroupElement.h"

#include "IWORKCollector.h"
#include "IWORKGeometryElement.h"
#include "IWORKImageElement.h"
#include "IWORKLineElement.h"
#include "IWORKMediaElement.h"
#include "IWORKShapeContext.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKGroupElement::IWORKGroupElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

void IWORKGroupElement::startOfElement()
{
  if (isCollector())
  {
    getCollector().startLevel();
    getCollector().startGroup();
  }
}

IWORKXMLContextPtr_t IWORKGroupElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return makeContext<IWORKGeometryElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::group :
    return makeContext<IWORKGroupElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::image :
    return makeContext<IWORKImageElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::line :
    return makeContext<IWORKLineElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::media :
    return makeContext<IWORKMediaElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::shape :
    return makeContext<IWORKShapeContext>(getState());
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

void IWORKGroupElement::endOfElement()
{
  if (isCollector())
  {
    getCollector().endGroup();
    getCollector().endLevel();
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
