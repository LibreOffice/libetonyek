/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKLineElement.h"

#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKGeometryElement.h"
#include "IWORKPositionElement.h"
#include "IWORKRefContext.h"
#include "IWORKStyleContainer.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{
namespace
{
typedef IWORKStyleContainer<IWORKToken::NS_URI_SF | IWORKToken::graphic_style, IWORKToken::NS_URI_SF | IWORKToken::graphic_style_ref> GraphicStyleContext;
}

IWORKLineElement::IWORKLineElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_head()
  , m_tail()
  , m_style()
{
}

void IWORKLineElement::startOfElement()
{
  if (isCollector())
    getCollector().startLevel();
}

IWORKXMLContextPtr_t IWORKLineElement::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::geometry :
    return std::make_shared<IWORKGeometryElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::head :
    return std::make_shared<IWORKPositionElement>(getState(), m_head);
  case IWORKToken::NS_URI_SF | IWORKToken::style :
    return std::make_shared<GraphicStyleContext>(getState(), m_style, getState().getDictionary().m_graphicStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::tail :
    return std::make_shared<IWORKPositionElement>(getState(), m_tail);
  default:
    break;
  }
  ETONYEK_DEBUG_MSG(("IWORKLineElement::element: find unknown element\n"));
  return IWORKXMLContextPtr_t();
}

void IWORKLineElement::endOfElement()
{
  IWORKLinePtr_t line(new IWORKLine());
  if (m_tail)
  {
    line->m_x1 = get(m_tail).m_x;
    line->m_y1 = get(m_tail).m_y;
  }
  if (m_head)
  {
    line->m_x2 = get(m_head).m_x;
    line->m_y2 = get(m_head).m_y;
  }
  if (isCollector())
  {
    if (m_style)
      getCollector().setGraphicStyle(m_style);
    getCollector().collectLine(line);
    getCollector().endLevel();
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
