/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <librevenge/librevenge.h>

#include "NUMCollector.h"

namespace libetonyek
{

NUMCollector::NUMCollector(librevenge::RVNGSpreadsheetInterface *const document)
  : m_document(document)
{
}

void NUMCollector::startDocument()
{
  m_document->startDocument(librevenge::RVNGPropertyList());
}

void NUMCollector::endDocument()
{
  m_document->endDocument();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
