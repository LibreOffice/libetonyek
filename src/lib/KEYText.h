/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEYTEXT_H_INCLUDED
#define KEYTEXT_H_INCLUDED

#include <deque>

#include "KEYObject.h"
#include "KEYStyles_fwd.h"
#include "KEYText_fwd.h"

namespace libkeynote
{

class KEYText
{
  struct Paragraph;
  typedef boost::shared_ptr<Paragraph> ParagraphPtr_t;

public:
  typedef std::deque<ParagraphPtr_t> ParagraphList_t;

public:
  KEYText();

  void setLayoutStyle(const KEYLayoutStylePtr_t &style);

  void openParagraph(const KEYParagraphStylePtr_t &style);
  void closeParagraph();

  void insertText(const std::string &text, const KEYCharacterStylePtr_t &style);
  void insertTab();
  void insertLineBreak();

  const KEYLayoutStylePtr_t &getLayoutStyle() const;
  const ParagraphList_t &getParagraphs() const;

  bool empty() const;

private:
  void insertDeferredLineBreaks();

private:
  KEYLayoutStylePtr_t m_layoutStyle;
  ParagraphList_t m_paragraphs;
  ParagraphPtr_t m_currentParagraph;
  int m_lineBreaks;

  KEYGeometryPtr_t m_boundingBox;
};

KEYObjectPtr_t makeObject(const KEYTextPtr_t &text);

}

#endif // KEYTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
