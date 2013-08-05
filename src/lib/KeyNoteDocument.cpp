/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <libkeynote/KeyNoteDocument.h>

#include "libkeynote_utils.h"
#include "KNCollector.h"
#include "KNParser.h"
#include "KNSVGGenerator.h"

namespace libkeynote
{

/**
Analyzes the content of an input stream to see if it can be parsed
\param input The input stream
\return A value that indicates whether the content from the input
stream is a KeyNote Document that libkeynote is able to parse
*/
bool KeyNoteDocument::isSupported(WPXInputStream *const input) try
{
  // TODO: implement me
  (void) input;
  return false;
}
catch (...)
{
  return false;
}

/**
Parses the input stream content. It will make callbacks to the functions provided by a
WPGPaintInterface class implementation when needed. This is often commonly called the
'main parsing routine'.
\param input The input stream
\param painter A WPGPainterInterface implementation
\return A value that indicates whether the parsing was successful
*/
bool KeyNoteDocument::parse(::WPXInputStream *const input, libwpg::WPGPaintInterface *const painter) try
{
  if (KeyNoteDocument::isSupported(input))
  {
    KNCollector collector(painter);
    KNParser parser(input, &collector);
    return parser.parse();
  }

  return false;
}
catch (...)
{
  return false;
}

/**
Parses the input stream content and generates a valid Scalable Vector Graphics
Provided as a convenience function for applications that support SVG internally.
\param input The input stream
\param output The output string whose content is the resulting SVG
\return A value that indicates whether the SVG generation was successful.
*/
bool KeyNoteDocument::generateSVG(::WPXInputStream *const input, KNStringVector &output) try
{
  KNSVGGenerator generator(output);
  bool result = KeyNoteDocument::parse(input, &generator);
  return result;
}
catch (...)
{
  return false;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
