/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KNTEXT_H_INCLUDED
#define KNTEXT_H_INCLUDED

#include <deque>

#include "KNObject.h"
#include "KNStyles_fwd.h"
#include "KNText_fwd.h"

namespace libkeynote
{

class KNText
{
  class Paragraph;
  typedef boost::shared_ptr<Paragraph> ParagraphPtr_t;

public:
  typedef std::deque<ParagraphPtr_t> ParagraphList_t;

public:
  KNText();

  void setLayoutStyle(const KNLayoutStylePtr_t &style);

  void openParagraph(const KNParagraphStylePtr_t &style);
  void closeParagraph();

  void insertText(const std::string &text, const KNCharacterStylePtr_t &style);
  void insertTab();
  void insertLineBreak();

  const KNLayoutStylePtr_t &getLayoutStyle() const;
  const ParagraphList_t &getParagraphs() const;

private:
  void insertDeferredLineBreaks();

private:
  KNLayoutStylePtr_t m_layoutStyle;
  ParagraphList_t m_paragraphs;
  ParagraphPtr_t m_currentParagraph;
  int m_lineBreaks;
};

KNObjectPtr_t makeObject(const KNTextPtr_t &text);

}

#endif // KNTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
