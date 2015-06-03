/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKUnfilteredElement.h"

#include "IWORKDictionary.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKUnfilteredElement::IWORKUnfilteredElement(IWORKXMLParserState &state, IWORKMediaContentPtr_t &content)
  : IWORKImageContext(state, content)
  , m_content(content)
{
}

void IWORKUnfilteredElement::endOfElement()
{
  IWORKImageContext::endOfElement();

  if (bool(m_content) && getId())
    getState().getDictionary().m_unfiltereds[get(getId())] = m_content;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
