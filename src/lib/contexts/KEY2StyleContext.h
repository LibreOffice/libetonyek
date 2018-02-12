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
#include "IWORKStyleContext.h"
#include "KEY2XMLContextBase.h"

namespace libetonyek
{

class KEY2StyleContext : public KEY2XMLElementContextBase
{
public:
  KEY2StyleContext(KEY2ParserState &state, IWORKStyleMap_t *styleMap, const char *defaultParent=nullptr, bool nested = false);

private:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  IWORKPropertyMap m_props;
  IWORKStyleContext m_base;
  boost::optional<std::string> m_ident;
  boost::optional<std::string> m_parentIdent;
};

}

#endif // KEY2STYLECONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
