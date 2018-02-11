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
#include "IWORKTabularInfoElement.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKGroupElement::IWORKGroupElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_groupIsOpened(false)
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
    ensureClosed();
    return makeContext<IWORKGroupElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::image :
    ensureOpened();
    return makeContext<IWORKImageElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::line :
    ensureOpened();
    return makeContext<IWORKLineElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::media :
    ensureOpened();
    return makeContext<IWORKMediaElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::drawable_shape :
  case IWORKToken::NS_URI_SF | IWORKToken::shape :
    ensureClosed();
    return makeContext<IWORKShapeContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::tabular_info :
    return makeContext<IWORKTabularInfoElement>(getState());
  default:
    break;
  }

  return IWORKXMLContextPtr_t();
}

void IWORKGroupElement::endOfElement()
{
  ensureClosed();
  if (isCollector())
  {
    getCollector().endGroup();
    getCollector().endLevel();
  }
}

void IWORKGroupElement::ensureOpened()
{
  if (m_groupIsOpened || !isCollector())
    return;
  getCollector().addOpenGroup();
  m_groupIsOpened=true;
}

void IWORKGroupElement::ensureClosed()
{
  if (!m_groupIsOpened || !isCollector())
    return;
  getCollector().addCloseGroup();
  m_groupIsOpened=false;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
