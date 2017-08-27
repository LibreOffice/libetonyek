/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKMEDIAELEMENT_H_INCLUDED
#define IWORKMEDIAELEMENT_H_INCLUDED

#include "IWORKTypes_fwd.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKMediaElement : public IWORKXMLElementContextBase
{
public:
  explicit IWORKMediaElement(IWORKXMLParserState &state);

private:
  void startOfElement() override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

private:
  IWORKMediaContentPtr_t m_content;
};

}

#endif // IWORKMEDIAELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
