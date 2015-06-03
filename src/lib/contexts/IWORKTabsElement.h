/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKTABSELEMENT_H_INCLUDED
#define IWORKTABSELEMENT_H_INCLUDED

#include <boost/optional.hpp>

#include "IWORKXMLContextBase.h"
#include "IWORKTypes.h"

namespace libetonyek
{

class IWORKTabsElement : public IWORKXMLElementContextBase
{
public:
  IWORKTabsElement(IWORKXMLParserState &state, IWORKTabStops_t &tabs);

private:
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();

private:
  IWORKTabStops_t &m_tabs;
  boost::optional<double> m_current;
};

}

#endif // IWORKTABSELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
