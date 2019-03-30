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
#include "IWORKTableInfoElement.h"
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
    return std::make_shared<IWORKGeometryElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::group :
    ensureClosed(); // checkme: creating a group in a group must be often possible
    return std::make_shared<IWORKGroupElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::image :
    ensureOpened();
    return std::make_shared<IWORKImageElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::line :
    ensureOpened();
    return std::make_shared<IWORKLineElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::media :
    ensureOpened();
    return std::make_shared<IWORKMediaElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::drawable_shape :
  case IWORKToken::NS_URI_SF | IWORKToken::shape :
    ensureOpened();
    return std::make_shared<IWORKShapeContext>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::table_info :
    ensureClosed();
    return std::make_shared<IWORKTableInfoElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::tabular_info :
    ensureClosed();
    return std::make_shared<IWORKTabularInfoElement>(getState());
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
  getCollector().openGroup();
  m_groupIsOpened=true;
}

void IWORKGroupElement::ensureClosed()
{
  if (!m_groupIsOpened || !isCollector())
    return;
  getCollector().closeGroup();
  m_groupIsOpened=false;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
