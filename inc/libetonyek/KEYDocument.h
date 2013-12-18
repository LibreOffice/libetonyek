/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LIBETONYEK_KEYDOCUMENT_H_INCLUDED
#define LIBETONYEK_KEYDOCUMENT_H_INCLUDED

#include <librevenge/librevenge.h>
#include <librevenge-stream/librevenge-stream.h>
#include "libetonyek_api.h"

namespace libetonyek
{

/** The type of Keynote file.
  */
enum KEYDocumentType
{
  KEY_DOCUMENT_TYPE_UNKNOWN,
  /// A standalone APXL file.
  KEY_DOCUMENT_TYPE_APXL_FILE,
  /// A complete package.
  KEY_DOCUMENT_TYPE_PACKAGE
};

class KEYDocument
{
public:

  /** Detect if the stream contains a valid Keynote document.
    *
    * The function can also optionally classify the type of the input
    * file. This is needed for proper support of documents created by
    * Keynote versions older than 5. The "documents" produced by these
    * versions were in fact directories, with the main file called
    * index.apxl.gz. But KEYDocument only works with a stream, so if a
    * file stream just for the main file is passed, it is not possible
    * to get to the rest of the files (i.e., the images used in the
    * document), because there is no way to get the path. Therefore we
    * provide the @c type as a hint to the application that it passed us
    * just the main file and it can try again with a stream wrapping the
    * whole directory :-)
    *
    * @arg[in] input the stream
    * @arg[out] type of document
    * @returns true if the stream contains a Keynote document, false
    * otherwise
    */
  static KEYAPI bool isSupported(librevenge::RVNGInputStream *input, KEYDocumentType *type = 0);

  /** Parse the input stream content.
   *
   * It will make callbacks to the functions provided by a
   * librevenge::RVNGPresentationInterface class implementation when needed. This is
   * often commonly called the 'main parsing routine'.
   *
   * @arg[in] input the input stream
   * @arg[in] generator a librevenge::RVNGPresentationInterface implementation
   * @returns a value that indicates whether the parsing was successful
   */
  static KEYAPI bool parse(librevenge::RVNGInputStream *input, librevenge::RVNGPresentationInterface *generator);
};

} // namespace libetonyek

#endif // LIBETONYEK_KEYDOCUMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
