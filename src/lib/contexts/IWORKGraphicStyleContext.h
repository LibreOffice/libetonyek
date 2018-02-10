/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKGRAPHICSTYLECONTEXT_H_INCLUDED
#define IWORKGRAPHICSTYLECONTEXT_H_INCLUDED

#include <string>

#include <boost/optional.hpp>

#include "IWORKStyle_fwd.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKGraphicStyleContext : public IWORKXMLElementContextBase
{
public:
  IWORKGraphicStyleContext(IWORKXMLParserState &state, IWORKStylePtr_t &style);

private:
  virtual void endOfElement();
  virtual IWORKXMLContextPtr_t element(int name);

private:
  IWORKGraphicStyleContext(IWORKGraphicStyleContext &);
  IWORKGraphicStyleContext operator=(IWORKGraphicStyleContext &);

  IWORKStylePtr_t &m_style;
  boost::optional<ID_t> m_ref;
};

}

#endif // IWORKGRAPHICSTYLECONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
