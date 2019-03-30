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
#include "IWORKTextElement.h"
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
  {
    getCollector().startLevel();
    assert(!getState().m_currentText);
    getState().m_currentText = getCollector().createText(getState().m_langManager);
  }
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
  case IWORKToken::bubble_cellid | IWORKToken::NS_URI_SF : // sf:row sf:column
  case IWORKToken::bubble_offset | IWORKToken::NS_URI_SF : // sfa:h and sfa:w
    return IWORKXMLContextPtr_t();
  case IWORKToken::NS_URI_SF | IWORKToken::text :
    return std::make_shared<IWORKTextElement>(getState());
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
    if (getId())
    {
      auto &dico=getState().getDictionary().m_tableComments;
      if (dico.find(*getId())==dico.end())
        dico[*getId()]=getState().m_currentText;
      else
      {
        ETONYEK_DEBUG_MSG(("IWORKCellCommentDrawableInfoElement::element: a text with ID=%s already exists\n", getId()->c_str()));
      }
    }
    else
    {
      ETONYEK_DEBUG_MSG(("IWORKCellCommentDrawableInfoElement::element: can not retrieve the ID\n"));
    }
    getState().m_currentText.reset();
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
