/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PAG1STYLECONTEXT_H_INCLUDED
#define PAG1STYLECONTEXT_H_INCLUDED

#include <string>

#include <boost/optional.hpp>

#include "IWORKPropertyMap.h"
#include "IWORKStyleContext.h"
#include "PAG1XMLContextBase.h"

namespace libetonyek
{

class PAG1StyleContext : public PAG1XMLElementContextBase
{
public:
  PAG1StyleContext(PAG1ParserState &state, IWORKStyleMap_t *styleMap, const char *defaultParent, bool nested = false);

private:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  IWORKPropertyMap m_props;
  IWORKStyleContext m_base;
  IWORKStyleMap_t *const m_styleMap;
  const bool m_nested;
  boost::optional<std::string> m_ident;
  boost::optional<std::string> m_parentIdent;
};

}

#endif // PAG1STYLECONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
