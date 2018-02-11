/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKTABULARINFOELEMENT_H_INCLUDED
#define IWORKTABULARINFOELEMENT_H_INCLUDED

#include <boost/optional.hpp>
#include "IWORKStyle_fwd.h"
#include "IWORKTypes.h"

#include "IWORKXMLContextBase.h"

namespace libetonyek
{
class IWORKTabularInfoElement : public IWORKXMLElementContextBase
{
public:
  explicit IWORKTabularInfoElement(IWORKXMLParserState &state);

private:
  void attribute(int name, const char *value) override;
  void startOfElement() override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

  boost::optional<ID_t> m_tableRef;
  IWORKStylePtr_t m_style;
  boost::optional<int> m_order;
  boost::optional<IWORKWrap> m_wrap;
};

}

#endif // IWORKTABULARINFOELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
