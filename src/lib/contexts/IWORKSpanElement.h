/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKSPANELEMENT_H_INCLUDED
#define IWORKSPANELEMENT_H_INCLUDED

#include "IWORKXMLContextBase.h"

#include "IWORKStyle.h"

namespace libetonyek
{

class IWORKSpanElement : public IWORKXMLElementContextBase
{
public:
  explicit IWORKSpanElement(IWORKXMLParserState &state);

protected:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void text(const char *value) override;
  void endOfElement() override;

  void ensureOpened();

private:
  IWORKStylePtr_t m_style;
  bool m_opened;
};

}

#endif // IWORKSPANELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
