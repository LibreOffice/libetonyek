/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY2XMLCONTEXTBASE_H_INCLUDED
#define KEY2XMLCONTEXTBASE_H_INCLUDED

#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class KEY2ParserState;
class KEYCollector;

template<class Base>
class KEY2XMLContextBase : public IWORKXMLContextBase<Base, KEY2ParserState, KEYCollector>
{
  typedef IWORKXMLContextBase<Base, KEY2ParserState, KEYCollector> Parent_t;

public:
  explicit KEY2XMLContextBase(KEY2ParserState &state)
    : Parent_t(state)
  {
  }

  template<typename T>
  KEY2XMLContextBase(KEY2ParserState &state, const T &a)
    : Parent_t(state, a)
  {
  }
};

typedef KEY2XMLContextBase<IWORKXMLContextElement> KEY2XMLElementContextBase;
typedef KEY2XMLContextBase<IWORKXMLContextText> KEY2XMLTextContextBase;
typedef KEY2XMLContextBase<IWORKXMLContextMixed> KEYM2XMLixedContextBase;
typedef KEY2XMLContextBase<IWORKXMLContextEmpty> KEY2XMLEmptyContextBase;

}

#endif // KEY2XMLCONTEXTBASE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
