/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PAG1XMLCONTEXTBASE_H_INCLUDED
#define PAG1XMLCONTEXTBASE_H_INCLUDED

#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class PAG1ParserState;
class PAGCollector;

template<class Base>
class PAG1XMLContextBase : public IWORKXMLContextBase<Base, PAG1ParserState, PAGCollector>
{
  typedef IWORKXMLContextBase<Base, PAG1ParserState, PAGCollector> Parent_t;

public:
  explicit PAG1XMLContextBase(PAG1ParserState &state)
    : Parent_t(state)
  {
  }
};

typedef PAG1XMLContextBase<IWORKXMLContextElement> PAG1XMLElementContextBase;
typedef PAG1XMLContextBase<IWORKXMLContextText> PAG1XMLTextContextBase;
typedef PAG1XMLContextBase<IWORKXMLContextMixed> PAG1XMLixedContextBase;
typedef PAG1XMLContextBase<IWORKXMLContextEmpty> PAG1XMLEmptyContextBase;

}

#endif // PAG1XMLCONTEXTBASE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
