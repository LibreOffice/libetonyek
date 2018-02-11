/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY1DIVELEMENT_H_INCLUDED
#define KEY1DIVELEMENT_H_INCLUDED

#include "KEY1XMLContextBase.h"

#include "IWORKEnum.h"
#include "IWORKStyle.h"
#include "KEY1SpanElement.h"

namespace libetonyek
{
class KEY1ParserState;

class KEY1DivStyle
{
public:
  explicit KEY1DivStyle(KEY1ParserState &state, IWORKStylePtr_t parentStyle);
  bool readAttribute(int name, const char *value);
  IWORKStylePtr_t getStyle();

private:
  KEY1ParserState &m_state;
  IWORKStylePtr_t m_style;
  IWORKStylePtr_t m_parentStyle;
  IWORKPropertyMap m_propMap;
};

class KEY1DivElement : public KEY1XMLElementContextBase
{
public:
  KEY1DivElement(KEY1ParserState &state, IWORKStylePtr_t spanStyle, IWORKStylePtr_t parentStyle, bool &delayedLineBreak);

protected:
  virtual void attribute(int name, const char *value);
  virtual IWORKXMLContextPtr_t element(int name);
  virtual void CDATA(const char *value);
  virtual void text(const char *value);
  virtual void endOfElement();

  void ensureOpened();

private:
  KEY1SpanStyle m_spanStyle;
  KEY1DivStyle m_style;
  bool m_opened;
  bool &m_delayedLineBreak;
};

}

#endif // KEY1DIVELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
