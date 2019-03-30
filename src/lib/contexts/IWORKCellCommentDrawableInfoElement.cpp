/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKCellCommentDrawableInfoElement.h"

#include <memory>

#include "libetonyek_xml.h"
#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKGeometryElement.h"
#include "IWORKPathElement.h"
#include "IWORKRefContext.h"
#include "IWORKStyleContainer.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKCellCommentDrawableInfoElement::IWORKCellCommentDrawableInfoElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
{
}

void IWORKCellCommentDrawableInfoElement::startOfElement()
{
  if (isCollector())
    getCollector().startLevel();
}

void IWORKCellCommentDrawableInfoElement::attribute(const int name, const char *const value)
{
  IWORKXMLElementContextBase::attribute(name, value);
}

IWORKXMLContextPtr_t IWORKCellCommentDrawableInfoElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::geometry | IWORKToken::NS_URI_SF :
    return std::make_shared<IWORKGeometryElement>(getState());
  case IWORKToken::path | IWORKToken::NS_URI_SF :
    return std::make_shared<IWORKPathElement>(getState());
  case IWORKToken::style | IWORKToken::NS_URI_SF :
    return std::make_shared<IWORKStyleContext>(getState(), &getState().getDictionary().m_cellCommentStyles);
  case IWORKToken::bubble_cellid | IWORKToken::NS_URI_SF :
  case IWORKToken::bubble_offset | IWORKToken::NS_URI_SF :
    return IWORKXMLContextPtr_t();
  case IWORKToken::text | IWORKToken::NS_URI_SF :
  default:
    break;
  }
  ETONYEK_DEBUG_MSG(("IWORKCellCommentDrawableInfoElement::element: find some unknown element\n"));
  return IWORKXMLContextPtr_t();
}

void IWORKCellCommentDrawableInfoElement::endOfElement()
{
  if (isCollector())
  {
    getCollector().endLevel();
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
