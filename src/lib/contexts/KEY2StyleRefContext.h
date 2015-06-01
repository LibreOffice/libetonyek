/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY2STYLEREFCONTEXT_H_INCLUDED
#define KEY2STYLEREFCONTEXT_H_INCLUDED

#include "IWORKStyleRefContext.h"
#include "KEY2XMLContextBase.h"

namespace libetonyek
{

struct KEYDictionary;

class KEY2StyleRefContext : public KEY2XMLEmptyContextBase
{
public:
  KEY2StyleRefContext(KEY2ParserState &state, const IWORKStyleMap_t &styleMap, bool nested = false, bool anonymous = false);

private:
  virtual void attribute(int name, const char *value);
  virtual void endOfElement();

private:
  IWORKStyleRefContext m_base;
  const IWORKStyleMap_t &m_styleMap;
  const bool m_nested;
  const bool m_anonymous;
};

}

#endif // KEY2STYLEREFCONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
