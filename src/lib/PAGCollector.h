/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PAGCOLLECTOR_H_INCLUDED
#define PAGCOLLECTOR_H_INCLUDED

#include <librevenge/librevenge.h>

namespace libetonyek
{

class PAGCollector
{
public:
  explicit PAGCollector(librevenge::RVNGTextInterface *document);

  // collector functions
  void collectText(const char *text);
  void collectLineBreak();
  void collectTab();

  // helper functions
  void startDocument();
  void endDocument();

  void startParagraph();
  void endParagraph();
  void startSpan();
  void endSpan();

private:
  void ensureSpan();

private:
  librevenge::RVNGTextInterface *m_document;

  bool m_spanOpened;
};

} // namespace libetonyek

#endif //  PAGCOLLECTOR_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
