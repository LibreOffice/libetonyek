/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <librevenge/librevenge.h>

#include "PAGESCollector.h"

namespace libetonyek
{

PAGESCollector::PAGESCollector(librevenge::RVNGTextInterface *const document)
  : m_document(document)
  , m_spanOpened(false)
{
}

void PAGESCollector::collectText(const char *text)
{
  ensureSpan();
  m_document->insertText(librevenge::RVNGString(text));
}

void PAGESCollector::collectLineBreak()
{
  ensureSpan();
  m_document->insertLineBreak();
}

void PAGESCollector::collectTab()
{
  ensureSpan();
  m_document->insertTab();
}

void PAGESCollector::startDocument()
{
  m_document->startDocument();
  m_document->setDocumentMetaData(librevenge::RVNGPropertyList());
}

void PAGESCollector::endDocument()
{
  m_document->endDocument();
}

void PAGESCollector::startParagraph()
{
  m_document->openParagraph(librevenge::RVNGPropertyList());
}

void PAGESCollector::endParagraph()
{
  if (m_spanOpened)
    endSpan();

  m_document->closeParagraph();
}

void PAGESCollector::startSpan()
{
  if (m_spanOpened) // automatic span
    endSpan();

  m_spanOpened = true;
  m_document->openSpan(librevenge::RVNGPropertyList());
}

void PAGESCollector::endSpan()
{
  m_spanOpened = false;
  m_document->closeSpan();
}

void PAGESCollector::ensureSpan()
{
  if (!m_spanOpened)
    startSpan();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
