/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "PAGCollector.h"

#include "IWORKDocumentInterface.h"
#include "IWORKOutputElements.h"
#include "IWORKText.h"

namespace libetonyek
{

PAGCollector::PAGCollector(IWORKDocumentInterface *const document)
  : IWORKCollector(document)
{
}

void PAGCollector::collectTextBody()
{
  assert(bool(m_currentText));

  IWORKOutputElements text;
  m_currentText->draw(text);
  m_currentText.reset();
  text.write(m_document);
}

void PAGCollector::collectAttachment(const IWORKZoneID_t &id)
{
  assert(bool(m_currentText));

  // FIXME: this will currently place the attachment (table) outside of
  // any paragraph and insert an empty paragraph after it.
  m_currentText->append(getZoneManager().get(id));
}

void PAGCollector::startDocument()
{
  m_document->startDocument(librevenge::RVNGPropertyList());
}

void PAGCollector::endDocument()
{
  m_document->endDocument();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
