/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "NUMCollector.h"

#include "IWORKDocumentInterface.h"

namespace libetonyek
{

NUMCollector::NUMCollector(IWORKDocumentInterface *const document)
  : IWORKCollector(document)
{
}

void NUMCollector::startDocument()
{
  IWORKCollector::startDocument();
}

void NUMCollector::endDocument()
{
  librevenge::RVNGPropertyList metadata;
  fillMetadata(metadata);
  m_document->setDocumentMetaData(metadata);
  getOutputManager().getCurrent().write(m_document);

  IWORKCollector::endDocument();
}

void NUMCollector::drawTable()
{
  librevenge::RVNGPropertyList props;
  m_currentTable.draw(props, m_outputManager.getCurrent());
}

void NUMCollector::drawMedia(
  const double x, const double y, const double w, const double h,
  const std::string &mimetype, const librevenge::RVNGBinaryData &data)
{
  // TODO: implement me
  (void) x;
  (void) y;
  (void) w;
  (void) h;
  (void) mimetype;
  (void) data;
  (void) x;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
