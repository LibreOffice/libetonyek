/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKTEXT_H_INCLUDED
#define IWORKTEXT_H_INCLUDED

#include "IWORKText_fwd.h"

#include <deque>

#include <glm/glm.hpp>

#include <librevenge/librevenge.h>

#include "IWORKStyle_fwd.h"
#include "IWORKStyleStack.h"
#include "IWORKOutputElements.h"

namespace libetonyek
{

class IWORKText
{

public:
  explicit IWORKText(bool discardEmptyContent);

  /// Set style used as base for all layout styles in this text.
  void pushBaseLayoutStyle(const IWORKStylePtr_t &style);
  /// Set style used as base for all paragraph styles in this text.
  void pushBaseParagraphStyle(const IWORKStylePtr_t &style);

  /// Set style used for the next layout.
  void setLayoutStyle(const IWORKStylePtr_t &style);
  /// Flush the current layout.
  void flushLayout();

  /// Set style used for the next paragraph.
  void setParagraphStyle(const IWORKStylePtr_t &style);
  /// Flush the current paragraph.
  void flushParagraph();

  /// Set style used for the next span.
  void setSpanStyle(const IWORKStylePtr_t &style);
  /// Flush the current span.
  void flushSpan();

  void openLink(const std::string &url);
  void closeLink();

  void insertText(const std::string &text);
  void insertTab();
  void insertSpace();
  void insertLineBreak();

  void insertInlineContent(const IWORKOutputElements &elements);
  void insertBlockContent(const IWORKOutputElements &elements);

  bool empty() const;

  void draw(IWORKOutputElements &elements);

private:
  void openSection();
  void closeSection();
  bool needsSection() const;

  void openPara();
  void closePara();

  void openSpan();
  void closeSpan();

private:
  IWORKStyleStack m_layoutStyleStack;
  IWORKStyleStack m_paraStyleStack;

  IWORKOutputElements m_elements;

  IWORKStylePtr_t m_layoutStyle;
  bool m_inSection;
  mutable librevenge::RVNGPropertyList m_sectionProps;
  mutable bool m_checkedSection;

  IWORKStylePtr_t m_paraStyle;
  bool m_inPara;
  bool m_ignoreEmptyPara;

  IWORKStylePtr_t m_spanStyle;
  bool m_spanStyleChanged;
  bool m_inSpan;

  IWORKStylePtr_t m_oldSpanStyle;
};

}

#endif // IWORKTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
