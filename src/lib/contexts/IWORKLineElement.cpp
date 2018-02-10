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
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

namespace
{
// TODO: merge this code with IWORKShapeContent.cpp
class StyleElement : public IWORKXMLElementContextBase
{
public:
  StyleElement(IWORKXMLParserState &state, IWORKStylePtr_t &style);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  IWORKStylePtr_t &m_style;
  boost::optional<ID_t> m_ref;
};

StyleElement::StyleElement(IWORKXMLParserState &state, IWORKStylePtr_t &style)
  : IWORKXMLElementContextBase(state)
  , m_style(style)
  , m_ref()
{
}

IWORKXMLContextPtr_t StyleElement::element(const int name)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::graphic_style_ref))
    return makeContext<IWORKRefContext>(getState(), m_ref);
  return IWORKXMLContextPtr_t();
}

void StyleElement::endOfElement()
{
  if (m_ref)
    m_style = getState().getStyleByName(get(m_ref).c_str(), getState().getDictionary().m_graphicStyles);
}

}

IWORKLineElement::IWORKLineElement(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
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
    return makeContext<IWORKGeometryElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::head :
    return makeContext<IWORKPositionElement>(getState(), m_head);
  case IWORKToken::NS_URI_SF | IWORKToken::style :
    return makeContext<StyleElement>(getState(), m_style);
  case IWORKToken::NS_URI_SF | IWORKToken::tail :
    return makeContext<IWORKPositionElement>(getState(), m_tail);
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
