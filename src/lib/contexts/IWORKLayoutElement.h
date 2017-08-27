/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKLAYOUTELEMENT_H_INCLUDED
#define IWORKLAYOUTELEMENT_H_INCLUDED

#include "IWORKStyle_fwd.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKLayoutElement : public IWORKXMLElementContextBase
{
public:
  explicit IWORKLayoutElement(IWORKXMLParserState &state);

protected:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void endOfElement() override;

  void open();

protected:
  bool m_opened;

private:
  IWORKStylePtr_t m_style;
};

}

#endif // IWORKLAYOUTELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
