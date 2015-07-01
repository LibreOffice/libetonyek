/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKFOELEMENT_H_INCLUDED
#define IWORKFOELEMENT_H_INCLUDED

#include "IWORKFormula.h"
#include "IWORKToken.h"
#include "IWORKTypes.h"
#include "IWORKXMLContextBase.h"
#include "IWORKXMLParserState.h"

namespace libetonyek
{

class FoElement : public IWORKXMLEmptyContextBase
{
public:
  explicit FoElement(IWORKXMLParserState &state);

private:
  virtual void attribute(int name, const char *value);
};

}

#endif // IWORKFOELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
