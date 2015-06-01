/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKSTYLEREFCONTEXT_H_INCLUDED
#define IWORKSTYLEREFCONTEXT_H_INCLUDED

#include "IWORKStyle_fwd.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

struct IWORKDictionary;

class IWORKStyleRefContext : public IWORKXMLEmptyContextBase
{
public:
  IWORKStyleRefContext(IWORKXMLParserState &state, const IWORKStyleMap_t &styleMap, bool nested = false, bool anonymous = false);

  virtual void attribute(int name, const char *value);
  virtual void endOfElement();

private:
  const IWORKStyleMap_t &m_styleMap;
  const bool m_nested;
  const bool m_anonymous;
};

}

#endif // IWORKSTYLEREFCONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
