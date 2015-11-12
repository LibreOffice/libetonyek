/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKShapeContext.h"

#include <cassert>

#include <boost/optional.hpp>

#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKGeometryElement.h"
#include "IWORKPathElement.h"
#include "IWORKRefContext.h"
#include "IWORKTextElement.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

namespace
{

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
  {
    const IWORKStyleMap_t::const_iterator it = getState().getDictionary().m_graphicStyles.find(get(m_ref));
    if (it != getState().getDictionary().m_graphicStyles.end())
      m_style = it->second;
  }
}

}

IWORKShapeContext::IWORKShapeContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_style()
{
}

void IWORKShapeContext::startOfElement()
{
  if (isCollector())
  {
    getCollector().startLevel();
    assert(!getState().m_currentText);
    getState().m_currentText = getCollector().createText();
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
  case IWORKToken::NS_URI_SF | IWORKToken::style :
    return makeContext<StyleElement>(getState(), m_style);
  case IWORKToken::NS_URI_SF | IWORKToken::text :
    return makeContext<IWORKTextElement>(getState());
  }

  return IWORKXMLContextPtr_t();
}

void IWORKShapeContext::endOfElement()
{
  if (isCollector())
  {
    if (m_style)
      getCollector().setGraphicStyle(m_style);
    getCollector().collectText(getState().m_currentText);
    getState().m_currentText.reset();
    getCollector().collectShape();
    getCollector().endLevel();
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
