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

#include "IWORKXMLContextBase.h"

namespace libetonyek
{

class IWORKLayoutElement : public IWORKXMLElementContextBase
{
public:
  explicit IWORKLayoutElement(IWORKXMLParserState &state);

protected:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
};

}

#endif // IWORKLAYOUTELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
