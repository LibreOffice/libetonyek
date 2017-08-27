/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKSTYLESCONTEXT_H_INCLUDED
#define IWORKSTYLESCONTEXT_H_INCLUDED

#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKStylesContext : public IWORKXMLElementContextBase
{
public:
  IWORKStylesContext(IWORKXMLParserState &state, bool anonymous);

  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

protected:
  const bool m_anonymous;
};

}

#endif // IWORKSTYLESCONTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
