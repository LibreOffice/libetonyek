/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY1STYLESCONTEXT_H_INCLUDED
#define KEY1STYLESCONTEXT_H_INCLUDED

#include <boost/optional.hpp>

#include "KEY1XMLContextBase.h"

#include "IWORKPropertyMap.h"
#include "IWORKStyle_fwd.h"
#include "IWORKTypes.h"

namespace libetonyek
{

class KEY1StylesContext : public KEY1XMLElementContextBase
{
public:
  KEY1StylesContext(KEY1ParserState &state, IWORKStylePtr_t &style, IWORKStylePtr_t parentStyle);

private:
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  IWORKStylePtr_t &m_style;
  IWORKPropertyMap m_propMap;
  IWORKStylePtr_t m_parentStyle;

  boost::optional<IWORKFill> m_fill;
  boost::optional<IWORKMarker> m_lineHead;
  boost::optional<IWORKMarker> m_lineTail;
  boost::optional<IWORKPattern> m_pattern;
  boost::optional<IWORKShadow> m_shadow;
};
}

#endif // KEY1STYLESCONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
