/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <librevenge/librevenge.h>

#include "NUMBERSCollector.h"

namespace libetonyek
{

NUMBERSCollector::NUMBERSCollector(librevenge::RVNGSpreadsheetInterface *const document)
  : m_document(document)
{
}

void NUMBERSCollector::startDocument()
{
  m_document->startDocument();
  m_document->setDocumentMetaData(librevenge::RVNGPropertyList());
}

void NUMBERSCollector::endDocument()
{
  m_document->endDocument();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
