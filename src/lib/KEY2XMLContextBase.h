/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYXMLCONTEXTBASE_H_INCLUDED
#define KEYXMLCONTEXTBASE_H_INCLUDED

#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class KEY2ParserState;
class KEYCollector;

typedef IWORKXMLContextBase<IWORKXMLContextElement, KEY2ParserState, KEYCollector> KEY2XMLElementContextBase;
typedef IWORKXMLContextBase<IWORKXMLContextText, KEY2ParserState, KEYCollector> KEY2XMLTextContextBase;
typedef IWORKXMLContextBase<IWORKXMLContextMixed, KEY2ParserState, KEYCollector> KEYM2XMLixedContextBase;
typedef IWORKXMLContextBase<IWORKXMLContextEmpty, KEY2ParserState, KEYCollector> KEY2XMLEmptyContextBase;

}

#endif // KEYXMLCONTEXTBASE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
