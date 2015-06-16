/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef NUM1XMLCONTEXTBASE_H_INCLUDED
#define NUM1XMLCONTEXTBASE_H_INCLUDED

#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class NUM1ParserState;
class NUMCollector;

template<class Base>
class NUM1XMLContextBase : public IWORKXMLContextBase<Base, NUM1ParserState, NUMCollector>
{
  typedef IWORKXMLContextBase<Base, NUM1ParserState, NUMCollector> Parent_t;

public:
  explicit NUM1XMLContextBase(NUM1ParserState &state)
    : Parent_t(state)
  {
  }

  template<typename T>
  NUM1XMLContextBase(NUM1ParserState &state, const T &a)
    : Parent_t(state, a)
  {
  }
};

typedef NUM1XMLContextBase<IWORKXMLContextElement> NUM1XMLElementContextBase;
typedef NUM1XMLContextBase<IWORKXMLContextText> NUM1XMLTextContextBase;
typedef NUM1XMLContextBase<IWORKXMLContextMixed> NUM1XMLixedContextBase;
typedef NUM1XMLContextBase<IWORKXMLContextEmpty> NUM1XMLEmptyContextBase;

}

#endif // NUM1XMLCONTEXTBASE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
