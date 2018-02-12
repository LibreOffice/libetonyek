/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKTABLEINFOELEMENT_H_INCLUDED
#define IWORKTABLEINFOELEMENT_H_INCLUDED

#include <boost/optional.hpp>
#include "IWORKStyle_fwd.h"
#include "IWORKTypes.h"

#include "IWORKXMLContextBase.h"

namespace libetonyek
{
class IWORKTableInfoElement : public IWORKXMLElementContextBase
{
public:
  explicit IWORKTableInfoElement(IWORKXMLParserState &state);

private:
  void startOfElement() override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

  IWORKStylePtr_t m_style;
  boost::optional<IWORKWrap> m_wrap;
};
}

#endif // IWORKTABLEINFOELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
