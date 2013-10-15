/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libkeynote project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include <libkeynote/KNDocument.h>

#include "libkeynote_utils.h"
#include "KN1Defaults.h"
#include "KN1Parser.h"
#include "KN2Defaults.h"
#include "KN2Parser.h"
#include "KNContentCollector.h"
#include "KNDefaults.h"
#include "KNDictionary.h"
#include "KNThemeCollector.h"
#include "KNSVGGenerator.h"
#include "KNZipStream.h"
#include "KNZlibStream.h"

using boost::scoped_ptr;
using boost::shared_ptr;

namespace libkeynote
{

namespace
{

/** Version of the file format.
  *
  * Versions 2--5 use the same format, with possible changes.
  */
enum Version
{
  VERSION_UNKNOWN,
  VERSION_KEYNOTE_1,
  VERSION_KEYNOTE_2,
  VERSION_KEYNOTE_3,
  VERSION_KEYNOTE_4,
  VERSION_KEYNOTE_5
};

enum Source
{
  SOURCE_UNKNOWN,
  SOURCE_APXL,
  SOURCE_APXL_GZ,
  SOURCE_PACKAGE_APXL,
  SOURCE_PACKAGE_APXL_GZ,
  SOURCE_KEY
};

Version detectVersionFromInput(const WPXInputStreamPtr_t &input)
{
  // TODO: do a real detection
  (void) input;
  return VERSION_KEYNOTE_5;
}

Version detectVersion(const WPXInputStreamPtr_t &input, Source &source)
{
  source = SOURCE_UNKNOWN;

  // check if this is a package
  if (input->isOLEStream())
  {
    scoped_ptr<WPXInputStream> apxl;

    apxl.reset(input->getDocumentOLEStream("index.apxl.gz"));
    if (bool(apxl))
    {
      source = SOURCE_PACKAGE_APXL_GZ;
      return VERSION_KEYNOTE_5;
    }

    apxl.reset(input->getDocumentOLEStream("presentation.apxl.gz"));
    if (bool(apxl))
    {
      source = SOURCE_PACKAGE_APXL_GZ;
      return VERSION_KEYNOTE_1;
    }

    apxl.reset(input->getDocumentOLEStream("index.apxl"));
    if (bool(apxl))
    {
      source = SOURCE_PACKAGE_APXL;
      return VERSION_KEYNOTE_5;
    }

    apxl.reset(input->getDocumentOLEStream("presentation.apxl"));
    if (bool(apxl))
    {
      source = SOURCE_PACKAGE_APXL;
      return VERSION_KEYNOTE_1;
    }
  }

  // check if this is a zip file (Keynote 09)
  KNZipStream zipInput(input);
  if (zipInput.isOLEStream())
  {
    scoped_ptr<WPXInputStream> apxl(zipInput.getDocumentOLEStream("index.apxl"));
    if (bool(apxl))
    {
      source = SOURCE_KEY;
      return VERSION_KEYNOTE_5;
    }
  }

  try
  {
    KNZlibStream compressedInput(input);
    source = SOURCE_APXL_GZ;
    return detectVersionFromInput(WPXInputStreamPtr_t(&compressedInput, KNDummyDeleter()));
  }
  catch (...)
  {
    // ignore
  }

  source = SOURCE_APXL;
  return detectVersionFromInput(input);
}

KNDefaults *makeDefaults(const Version version)
{
  switch (version)
  {
  case VERSION_KEYNOTE_1 :
    return new KN1Defaults();
  case VERSION_KEYNOTE_2 :
  case VERSION_KEYNOTE_3 :
  case VERSION_KEYNOTE_4 :
  case VERSION_KEYNOTE_5 :
    // I am going to suppose these have not changed
    return new KN2Defaults();
  default :
    KN_DEBUG_MSG(("unknown version\n"));
    throw GenericException();
  }

  return 0;
}

shared_ptr<KNParser> makeParser(const Version version, const WPXInputStreamPtr_t &input, const WPXInputStreamPtr_t &package, KNCollector *const collector, const KNDefaults &defaults)
{
  shared_ptr<KNParser> parser;

  switch (version)
  {
  case VERSION_KEYNOTE_1 :
    parser.reset(new KN1Parser(input, package, collector, defaults));
    break;
  case VERSION_KEYNOTE_2 :
  case VERSION_KEYNOTE_3 :
  case VERSION_KEYNOTE_4 :
  case VERSION_KEYNOTE_5 :
    parser.reset(new KN2Parser(input, package, collector, defaults));
    break;
  default :
    KN_DEBUG_MSG(("KNDocument::parse(): unhandled version\n"));
    break;
  }

  return parser;
}

}

/**
Analyzes the content of an input stream to see if it can be parsed
\param input The input stream
\return A value that indicates whether the content from the input
stream is a KN Document that libkeynote is able to parse
*/
bool KNDocument::isSupported(WPXInputStream *const input, KNDocumentType *type) try
{
  if (type)
    *type = KN_DOCUMENT_TYPE_UNKNOWN;

  Source source = SOURCE_UNKNOWN;
  const Version version = detectVersion(WPXInputStreamPtr_t(input, KNDummyDeleter()), source);

  if ((VERSION_UNKNOWN != version) && type)
  {
    switch (source)
    {
    case SOURCE_APXL :
    case SOURCE_APXL_GZ :
      *type = KN_DOCUMENT_TYPE_APXL_FILE;
      break;
    case SOURCE_PACKAGE_APXL :
    case SOURCE_PACKAGE_APXL_GZ :
    case SOURCE_KEY :
      *type = KN_DOCUMENT_TYPE_PACKAGE;
      break;
    default :
      assert(!"detection is broken");
      break;
    }
  }

  return VERSION_UNKNOWN != version;
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
\param painter A WPGPaintInterface implementation
\return A value that indicates whether the parsing was successful
*/
bool KNDocument::parse(::WPXInputStream *const input, libwpg::WPGPaintInterface *const painter) try
{
  WPXInputStreamPtr_t input_(input, KNDummyDeleter());

  Source source = SOURCE_UNKNOWN;
  const Version version = detectVersion(input_, source);

  if (VERSION_UNKNOWN == version)
    return false;

  WPXInputStreamPtr_t package;
  switch (source)
  {
  case SOURCE_PACKAGE_APXL :
    package = input_;
    input_.reset(package->getDocumentOLEStream((VERSION_KEYNOTE_1 == version) ? "presentation.apxl" : "index.apxl"));
    break;
  case SOURCE_PACKAGE_APXL_GZ :
  {
    package = input_;
    const WPXInputStreamPtr_t compressed(
      package->getDocumentOLEStream((VERSION_KEYNOTE_1 == version) ? "presentation.apxl.gz" : "index.apxl.gz"));
    input_.reset(new KNZlibStream(compressed));
    break;
  }
  case SOURCE_KEY :
    package = input_;
    input_.reset(package->getDocumentOLEStream("index.apxl"));
    break;
  default :
    // nothing
    break;
  }

  KNDictionary dict;
  KNLayerMap_t masterPages;
  KNSize presentationSize;
  const scoped_ptr<KNDefaults> defaults(makeDefaults(version));

  input_->seek(0, WPX_SEEK_SET);

  KNThemeCollector themeCollector(dict, masterPages, presentationSize, *defaults);
  shared_ptr<KNParser> parser = makeParser(version, input_, package, &themeCollector, *defaults);
  if (!parser->parse())
    return false;

  input_->seek(0, WPX_SEEK_SET);

  KNContentCollector contentCollector(painter, dict, masterPages, presentationSize, *defaults);
  parser = makeParser(version, input_, package, &contentCollector, *defaults);
  return parser->parse();
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
bool KNDocument::generateSVG(::WPXInputStream *const input, KNStringVector &output) try
{
  KNSVGGenerator generator(output);
  bool result = KNDocument::parse(input, &generator);
  return result;
}
catch (...)
{
  return false;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
