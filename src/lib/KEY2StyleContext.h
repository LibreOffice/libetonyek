/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY2STYLECONTEXT_H_INCLUDED
#define KEY2STYLECONTEXT_H_INCLUDED

#include <string>

#include <boost/optional.hpp>

#include "IWORKPropertyMap.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class KEY2StyleContext : public IWORKXMLElementContextBase
{
public:
  KEY2StyleContext(IWORKXMLParserState &state, int id, bool nested = false);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  const int m_id;
  const bool m_nested;
  IWORKPropertyMap m_props;
  boost::optional<std::string> m_ident;
  boost::optional<std::string> m_parentIdent;
};

class KEY2StyleRefContext : public IWORKXMLEmptyContextBase
{
public:
  KEY2StyleRefContext(IWORKXMLParserState &state, int id, bool nested = false, bool anonymous = false);

private:
  virtual void endOfElement();

private:
  const int m_id;
  const bool m_nested;
  const bool m_anonymous;
};

}

#endif // KEY2STYLECONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
