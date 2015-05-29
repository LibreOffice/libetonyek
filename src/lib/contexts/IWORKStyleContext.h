/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKSTYLECONTEXT_H_INCLUDED
#define IWORKSTYLECONTEXT_H_INCLUDED

#include <string>

#include <boost/optional.hpp>

#include "IWORKPropertyMap.h"
#include "IWORKStyle_fwd.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

struct IWORKDictionary;

class IWORKStyleContext : public IWORKXMLElementContextBase
{
public:
  IWORKStyleContext(IWORKXMLParserState &state, IWORKStyleMap_t *styleMap, bool nested = false);
  IWORKStyleContext(IWORKXMLParserState &state, IWORKStyleMap_t *styleMap, IWORKPropertyMap &props, bool nested = false);

  virtual void attribute(int name, const char *value);
  virtual void endOfElement();

private:
  virtual IWORKXMLContextPtr_t element(int name);

private:
  IWORKStyleMap_t *const m_styleMap;
  const bool m_nested;
  IWORKPropertyMap m_ownProps;
  IWORKPropertyMap &m_props;
  boost::optional<std::string> m_ident;
  boost::optional<std::string> m_parentIdent;
};

}

#endif // IWORKSTYLECONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
