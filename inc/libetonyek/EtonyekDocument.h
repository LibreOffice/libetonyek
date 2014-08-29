/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LIBETONYEK_ETONYEKDOCUMENT_H_INCLUDED
#define LIBETONYEK_ETONYEKDOCUMENT_H_INCLUDED

#include <librevenge/librevenge.h>
#include <librevenge-stream/librevenge-stream.h>

#ifdef DLL_EXPORT
#ifdef LIBETONYEK_BUILD
#define ETONYEKAPI __declspec(dllexport)
#else
#define ETONYEKAPI __declspec(dllimport)
#endif
#else // !DLL_EXPORT
#ifdef LIBETONYEK_VISIBILITY
#define ETONYEKAPI __attribute__((visibility("default")))
#else
#define ETONYEKAPI
#endif
#endif

namespace libetonyek
{

class EtonyekDocument
{
public:
  /** Likelihood that the file format is supported.
    */
  enum Confidence
  {
    CONFIDENCE_NONE, //< not supported
    CONFIDENCE_SUPPORTED_PART, //< the file is only a part of a supported structured format
    CONFIDENCE_EXCELLENT //< supported
  };

  /** Result of parsing the file.
    */
  enum Result
  {
    RESULT_OK, //< parsed without any problem
    RESULT_FILE_ACCESS_ERROR, //< problem when accessing the file
    RESULT_PACKAGE_ERROR, //< problem with parsing structured file's content
    RESULT_PARSE_ERROR, //< problem when parsing the file
    RESULT_UNSUPPORTED_FORMAT, //< unsupported file format
    RESULT_UNKNOWN_ERROR //< an unspecified error
  };

  /** Type of document.
    */
  enum Type
  {
    TYPE_UNKNOWN, //< unrecognized file
    TYPE_KEYNOTE, //< Keynote
    TYPE_NUMBERS, //< Numbers
    TYPE_PAGES //< Pages
  };

public:
  /** Detect if the stream contains a valid iWorks document.
    *
    * The function can also optionally classify the type of the input
    * file. This is needed for proper support of documents saved in
    * "package" format. These "documents" are in fact directories, with
    * the main file called index.apxl.gz or index.xml.gz. But
    * EtonyekDocument only works with a stream, so if a file stream just
    * for the main file is passed, it is not possible to get to the rest
    * of the files (i.e., the images used in the document), because
    * there is no way to get the path. Therefore we provide the @c type
    * as a hint to the application that it passed us just the main file
    * and it can try again with a stream wrapping the whole directory
    * :-)
    *
    * @arg[in] input the stream
    * @arg[out] type type of the input document
    * @returns the type of document
    */
  static ETONYEKAPI Confidence isSupported(librevenge::RVNGInputStream *input, Type *Type = 0);

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
  static ETONYEKAPI bool parse(librevenge::RVNGInputStream *input, librevenge::RVNGPresentationInterface *generator);

  /** Parse the input stream content.
   *
   * It will make callbacks to the functions provided by a
   * librevenge::RVNGSpreadsheetInterface class implementation when needed. This is
   * often commonly called the 'main parsing routine'.
   *
   * @arg[in] input the input stream
   * @arg[in] generator a librevenge::RVNGSpreadsheetInterface implementation
   * @returns a value that indicates whether the parsing was successful
   */
  static ETONYEKAPI bool parse(librevenge::RVNGInputStream *input, librevenge::RVNGSpreadsheetInterface *document);

  /** Parse the input stream content.
   *
   * It will make callbacks to the functions provided by a
   * librevenge::RVNGTextInterface class implementation when needed. This is
   * often commonly called the 'main parsing routine'.
   *
   * @arg[in] input the input stream
   * @arg[in] generator a librevenge::RVNGTextInterface implementation
   * @returns a value that indicates whether the parsing was successful
   */
  static ETONYEKAPI bool parse(librevenge::RVNGInputStream *input, librevenge::RVNGTextInterface *document);
};

} // namespace libetonyek

#endif // LIBETONYEK_ETONYEKDOCUMENT_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
