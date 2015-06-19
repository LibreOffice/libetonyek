/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "PAG1FootnotesElement.h"

#include "IWORKToken.h"
#include "PAG1ParserState.h"
#include "PAG1TextStorageElement.h"

namespace libetonyek
{

PAG1FootnotesElement::PAG1FootnotesElement(PAG1ParserState &state)
  : PAG1XMLElementContextBase(state)
{
}

IWORKXMLContextPtr_t PAG1FootnotesElement::element(const int name)
{
  if (name == (IWORKToken::NS_URI_SF | IWORKToken::text_storage))
    return makeContext<PAG1TextStorageElement>(getState(), true);
  return IWORKXMLContextPtr_t();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
