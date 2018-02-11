/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY1SPANELEMENT_H_INCLUDED
#define KEY1SPANELEMENT_H_INCLUDED

#include "KEY1XMLContextBase.h"

#include "IWORKEnum.h"
#include "IWORKStyle.h"

namespace libetonyek
{
class IWORKText;
class KEY1ParserState;

class KEY1SpanStyle
{
public:
  KEY1SpanStyle(KEY1ParserState &state, IWORKStylePtr_t parentStyle);
  bool readAttribute(int name, const char *value);
  IWORKStylePtr_t getStyle();

private:
  IWORKStylePtr_t m_style;
  IWORKStylePtr_t m_parentStyle;
  IWORKPropertyMap m_propMap;
};

class KEY1SpanElement : public KEY1XMLElementContextBase
{
public:
  KEY1SpanElement(KEY1ParserState &state, IWORKStylePtr_t parentStyle, bool &delayedLineBreak);

  static void sendCDATA(const char *value, std::shared_ptr<IWORKText> currentText, IWORKStylePtr_t spanStyle, bool &delayedLineBreak);

protected:
  void attribute(int name, const char *value) override;
  IWORKXMLContextPtr_t element(int name) override;
  void CDATA(const char *value) override;
  void text(const char *value) override;
  void endOfElement() override;

  void ensureClosed();
  void ensureOpened();

private:
  KEY1SpanStyle m_style;
  bool m_opened;
  bool &m_delayedLineBreak;
};

}

#endif // KEY1SPANELEMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
