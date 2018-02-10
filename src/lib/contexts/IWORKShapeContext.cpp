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

namespace
{
typedef IWORKStyleContainer<IWORKToken::NS_URI_SF | IWORKToken::graphic_style, IWORKToken::NS_URI_SF | IWORKToken::graphic_style_ref> GraphicStyleContext;
}

IWORKShapeContext::IWORKShapeContext(IWORKXMLParserState &state)
  : IWORKXMLElementContextBase(state)
  , m_style()
  , m_locked(false)
{
}

void IWORKShapeContext::attribute(const int name, const char *const value)
{
  switch (name)
  {
  case IWORKToken::sfclass | IWORKToken::NS_URI_SFA : // shape
    break;
  case IWORKToken::locked | IWORKToken::NS_URI_SF :
    m_locked=bool_cast(value);
    break;
  case IWORKToken::ID | IWORKToken::NS_URI_SFA :
    IWORKXMLElementContextBase::attribute(name, value);
    break;
  default :
  {
    // find also can-autosize-h, can-autosize-v, key:inheritance, key:tag
    static bool first=true;
    if (first)
    {
      first=false;
      ETONYEK_DEBUG_MSG(("IWORKShapeContext::attribute: find some unknown attributes\n"));
    }
    IWORKXMLElementContextBase::attribute(name, value);
  }
  }
}

void IWORKShapeContext::startOfElement()
{
  if (isCollector())
  {
    getCollector().startLevel();
    assert(!getState().m_currentText);
    getState().m_currentText = getCollector().createText(getState().m_langManager);
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
    return makeContext<GraphicStyleContext>(getState(), m_style, getState().getDictionary().m_graphicStyles);
  case IWORKToken::NS_URI_SF | IWORKToken::text :
    return makeContext<IWORKTextElement>(getState());
  case IWORKToken::NS_URI_SF | IWORKToken::wrap : // README
    return IWORKXMLContextPtr_t();
  default:
    ETONYEK_DEBUG_MSG(("IWORKShapeContext::element: find some unknown element\n"));
    break;
  }

  return IWORKXMLContextPtr_t();
}

void IWORKShapeContext::endOfElement()
{
  // CHECKME: do we need to store shape with ID ?
  if (isCollector())
  {
    if (m_style)
      getCollector().setGraphicStyle(m_style);
    getCollector().collectText(getState().m_currentText);
    getState().m_currentText.reset();
    getCollector().collectShape(m_locked);
    getCollector().endLevel();
  }
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
