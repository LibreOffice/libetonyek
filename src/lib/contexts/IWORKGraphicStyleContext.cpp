/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKGraphicStyleContext.h"

#include "libetonyek_xml.h"
#include "IWORKCollector.h"
#include "IWORKDictionary.h"
#include "IWORKRefContext.h"
#include "IWORKToken.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{
IWORKGraphicStyleContext::IWORKGraphicStyleContext(IWORKXMLParserState &state, IWORKStylePtr_t &style)
  : IWORKXMLElementContextBase(state)
  , m_style(style)
  , m_ref()
{
}

IWORKXMLContextPtr_t IWORKGraphicStyleContext::element(const int name)
{
  switch (name)
  {
  case IWORKToken::NS_URI_SF | IWORKToken::graphic_style_ref:
    return makeContext<IWORKRefContext>(getState(), m_ref);
  default:
    ETONYEK_DEBUG_MSG(("IWORKGraphicStyleContext::element: Oops, find some unknown elements\n"));
    break;
  }
  return IWORKXMLContextPtr_t();
}

void IWORKGraphicStyleContext::endOfElement()
{
  if (m_ref)
    m_style = getState().getStyleByName(get(m_ref).c_str(), getState().getDictionary().m_graphicStyles);
}
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
