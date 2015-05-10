/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKPELEMENT_H_INCLUDED
#define IWORKPELEMENT_H_INCLUDED

#include "IWORKTypes.h"
#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKPElement : public IWORKXMLMixedContextBase
{
public:
  explicit IWORKPElement(IWORKXMLParserState &state);

protected:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void endOfElement();
  virtual void text(const char *value);

private:
  void ensureOpened();

private:
  IWORKStylePtr_t m_style;
  bool m_opened;
};

}

#endif // IWORKPELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
