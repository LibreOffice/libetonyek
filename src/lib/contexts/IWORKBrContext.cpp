/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKBrContext.h"

#include "IWORKText.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

IWORKBrContext::IWORKBrContext(IWORKXMLParserState &state)
  : IWORKXMLEmptyContextBase(state)
{
}

void IWORKBrContext::endOfElement()
{
  if (bool(getState().m_currentText))
    getState().m_currentText->insertLineBreak();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
