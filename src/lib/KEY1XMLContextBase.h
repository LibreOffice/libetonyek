/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY1XMLCONTEXTBASE_H_INCLUDED
#define KEY1XMLCONTEXTBASE_H_INCLUDED

#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class KEY1ParserState;
class KEYCollector;

template<class Base>
class KEY1XMLContextBase : public IWORKXMLContextBase<Base, KEY1ParserState, KEYCollector>
{
  typedef IWORKXMLContextBase<Base, KEY1ParserState, KEYCollector> Parent_t;

public:
  explicit KEY1XMLContextBase(KEY1ParserState &state)
    : Parent_t(state)
  {
  }

  template<typename T>
  KEY1XMLContextBase(KEY1ParserState &state, const T &a)
    : Parent_t(state, a)
  {
  }
};

typedef KEY1XMLContextBase<IWORKXMLContextElement> KEY1XMLElementContextBase;
typedef KEY1XMLContextBase<IWORKXMLContextText> KEY1XMLTextContextBase;
typedef KEY1XMLContextBase<IWORKXMLContextMixed> KEYM2XMLixedContextBase;
typedef KEY1XMLContextBase<IWORKXMLContextEmpty> KEY1XMLEmptyContextBase;

}

#endif // KEY1XMLCONTEXTBASE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
