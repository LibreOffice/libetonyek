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
#include <stack>

#include <glm/glm.hpp>

#include <librevenge/librevenge.h>

#include "IWORKStyle_fwd.h"
#include "IWORKStyleStack.h"
#include "IWORKOutputElements.h"

namespace libetonyek
{

class IWORKLanguageManager;
class IWORKTextRecorder;

class IWORKText
{
public:
  IWORKText(const IWORKLanguageManager &langManager, bool discardEmptyContent);
  ~IWORKText();

  void setRecorder(const boost::shared_ptr<IWORKTextRecorder> &recorder);
  const boost::shared_ptr<IWORKTextRecorder> &getRecorder() const;

  /// Set style used as base for all layout styles in this text.
  void pushBaseLayoutStyle(const IWORKStylePtr_t &style);
  /// Set style used as base for all paragraph styles in this text.
  void pushBaseParagraphStyle(const IWORKStylePtr_t &style);

  /// Set style used for the next layout.
  void setLayoutStyle(const IWORKStylePtr_t &style);
  /// Flush the current layout.
  void flushLayout();

  /// Set list style used for the next paragraph, if it is in a list.
  void setListStyle(const IWORKStylePtr_t &style);
  /// Set the list level of the next paragraph, 0 meaning no list.
  void setListLevel(const unsigned level);
  /// Flush the current list level(s).
  void flushList();

  /// Set style used for the next paragraph.
  void setParagraphStyle(const IWORKStylePtr_t &style);
  /// Flush the current paragraph.
  void flushParagraph();

  /// Set style used for the next span.
  void setSpanStyle(const IWORKStylePtr_t &style);
  /// Set language used for the next span.
  void setLanguage(const IWORKStylePtr_t &style);
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

  // utility function
  static void fillCharPropList(const IWORKStyleStack &style, const IWORKLanguageManager &langManager, librevenge::RVNGPropertyList &props);

private:
  void openSection();
  void closeSection();
  bool needsSection() const;

  void handleListLevelChange(unsigned level);

  void openPara();
  void closePara();
  void fillParaPropList(librevenge::RVNGPropertyList &propList);

  void openSpan();
  void closeSpan();

private:
  const IWORKLanguageManager &m_langManager;

  IWORKStyleStack m_layoutStyleStack;
  IWORKStyleStack m_paraStyleStack;

  IWORKOutputElements m_elements;

  IWORKStylePtr_t m_layoutStyle;
  bool m_inSection;
  mutable librevenge::RVNGPropertyList m_sectionProps;
  mutable bool m_checkedSection;

  IWORKStylePtr_t m_listStyle;
  unsigned m_listLevel;
  unsigned m_inListLevel;
  /// Saves info about the currently opened list levels being ordered or unordered.
  std::stack<bool> m_isOrderedStack;

  IWORKStylePtr_t m_paraStyle;
  bool m_inPara;
  bool m_ignoreEmptyPara;

  IWORKStylePtr_t m_spanStyle;
  IWORKStylePtr_t m_langStyle;
  bool m_spanStyleChanged;
  bool m_inSpan;

  IWORKStylePtr_t m_oldSpanStyle;

  boost::shared_ptr<IWORKTextRecorder> m_recorder;
};

}

#endif // IWORKTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
