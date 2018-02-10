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

class IWORKStyleContext : public IWORKXMLElementContextBase
{
public:
  IWORKStyleContext(IWORKXMLParserState &state, IWORKStyleMap_t *styleMap = nullptr);
  IWORKStyleContext(IWORKXMLParserState &state, IWORKPropertyMap &props, IWORKStyleMap_t *styleMap = nullptr, const char *defaultParent = nullptr);

  void attribute(int name, const char *value) override;
  void endOfElement() override;

  /** returns the created style

      \note must be called after the style is read, ie. after endOfElement have been called*/
  IWORKStylePtr_t getStyle()
  {
    return m_style;
  }
private:
  IWORKXMLContextPtr_t element(int name) override;

private:
  IWORKStyleMap_t *const m_styleMap;
  const std::string m_defaultParent;
  IWORKPropertyMap m_ownProps;
  IWORKPropertyMap &m_props;
  //! the final style
  IWORKStylePtr_t m_style;
  boost::optional<std::string> m_ident;
  boost::optional<std::string> m_parentIdent;
};

}

#endif // IWORKSTYLECONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
