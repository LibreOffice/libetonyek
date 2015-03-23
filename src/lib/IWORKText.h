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

#include "IWORKObject.h"
#include "IWORKStyle_fwd.h"
#include "IWORKStyleStack.h"
#include "IWORKOutputElements.h"

namespace libetonyek
{

class IWORKTransformation;

class IWORKText
{
  struct Paragraph;
  typedef boost::shared_ptr<Paragraph> ParagraphPtr_t;

public:
  typedef std::deque<ParagraphPtr_t> ParagraphList_t;

public:
  explicit IWORKText(bool object = true);

  void setLayoutStyle(const IWORKStylePtr_t &style);

  const IWORKGeometryPtr_t &getBoundingBox() const;
  void setBoundingBox(const IWORKGeometryPtr_t &boundingBox);

  void openParagraph(const IWORKStylePtr_t &style);
  void closeParagraph();

  void insertText(const std::string &text, const IWORKStylePtr_t &style);
  void insertTab();
  void insertLineBreak();

  const IWORKStylePtr_t &getLayoutStyle() const;
  const ParagraphList_t &getParagraphs() const;
  bool isObject() const;

  bool empty() const;

  void draw(const IWORKTransformation &trafo, IWORKOutputElements &elements) const;

private:
  void insertDeferredLineBreaks();

private:
  IWORKStyleStack m_styleStack;
  IWORKStylePtr_t m_layoutStyle;
  ParagraphList_t m_paragraphs;
  ParagraphPtr_t m_currentParagraph;
  int m_lineBreaks;
  const bool m_object;

  IWORKGeometryPtr_t m_boundingBox;
};

}

#endif // IWORKTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
