/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY1CONTENTELEMENT_H_INCLUDED
#define KEY1CONTENTELEMENT_H_INCLUDED

#include <boost/shared_ptr.hpp>

#include "KEY1XMLContextBase.h"

#include "IWORKEnum.h"
#include "IWORKStyle.h"

#include "KEY1SpanElement.h"
#include "KEY1DivElement.h"

namespace libetonyek
{
class IWORKText;
class KEY1ParserState;

class KEY1ContentElement : public KEY1XMLElementContextBase
{
public:
  explicit KEY1ContentElement(KEY1ParserState &state);

protected:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void CDATA(const char *value);
  virtual void text(const char *value);

private:
  KEY1DivStyle m_divStyle;
  KEY1SpanStyle m_spanStyle;
  bool m_delayedLineBreak;
};

}

#endif // KEY1CONTENTELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
