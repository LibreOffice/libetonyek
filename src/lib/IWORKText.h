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

#include "IWORKStyle_fwd.h"
#include "IWORKStyleStack.h"
#include "IWORKOutputElements.h"

namespace libetonyek
{

class IWORKText
{

public:
  IWORKText();

  void openParagraph(const IWORKStylePtr_t &style);
  void closeParagraph();

  void openSpan(const IWORKStylePtr_t &style);
  void closeSpan();

  void openLink(const std::string &url);
  void closeLink();

  void insertText(const std::string &text);
  void insertTab();
  void insertLineBreak();

  void append(const IWORKOutputElements &elements);

  bool empty() const;

  void draw(IWORKOutputElements &elements);
  void draw(const glm::dmat3 &trafo, const IWORKGeometryPtr_t &boundingBox, IWORKOutputElements &elements);

private:
  void doOpenSpan();
  void doCloseSpan();

  void flushSpan();

private:
  IWORKStyleStack m_styleStack;

  IWORKOutputElements m_elements;

  IWORKStylePtr_t m_currentSpanStyle;
  bool m_spanOpened;
  bool m_pendingSpanClose;
  bool m_inSpan;
};

}

#endif // IWORKTEXT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
