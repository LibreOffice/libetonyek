/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LIBETONYEK_NUMBERSDOCUMENT_H_INCLUDED
#define LIBETONYEK_NUMBERSDOCUMENT_H_INCLUDED

#include <librevenge/librevenge.h>
#include <librevenge-stream/librevenge-stream.h>

namespace libetonyek
{

enum NUMBERSDocumentType
{
  NUMBERS_DOCUMENT_TYPE_UNKNOWN,
  // A separate main document file (index.xml.gz)
  NUMBERS_DOCUMENT_TYPE_MAIN_FILE,
  // A complete package
  NUMBERS_DOCUMENT_TYPE_PACKAGE
};

class NUMBERSDocument
{
public:
  static bool isSupported(librevenge::RVNGInputStream *input, NUMBERSDocumentType *type = 0);
  static bool parse(librevenge::RVNGInputStream *input, librevenge::RVNGSpreadsheetInterface *document);
};

} // namespace libetonyek

#endif // LIBETONYEK_NUMBERSDOCUMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
