/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include <libetonyek/KEYDocument.h>

#include "libetonyek_utils.h"
#include "libetonyek_xml.h"
#include "KEY1Defaults.h"
#include "KEY1Parser.h"
#include "KEY2Defaults.h"
#include "KEY2Parser.h"
#include "KEY2Token.h"
#include "KEYContentCollector.h"
#include "KEYDefaults.h"
#include "KEYDictionary.h"
#include "KEYThemeCollector.h"
#include "KEYSVGGenerator.h"
#include "KEYZipStream.h"
#include "KEYZlibStream.h"

using boost::scoped_ptr;
using boost::shared_ptr;

namespace libetonyek
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
  if (input->atEOS())
    return VERSION_UNKNOWN;

  const KEY2Tokenizer tokenizer;
  KEYXMLReader reader(input.get(), tokenizer);

  if ((KEY2Token::NS_URI_KEY | KEY2Token::presentation) == getId(reader))
  {
    const std::string version = readOnlyAttribute(reader, KEY2Token::version, KEY2Token::NS_URI_KEY);

    switch (tokenizer(version.c_str()))
    {
    case KEY2Token::VERSION_STR_2 :
      return VERSION_KEYNOTE_2;
    case KEY2Token::VERSION_STR_3 :
      return VERSION_KEYNOTE_3;
    case KEY2Token::VERSION_STR_4 :
      return VERSION_KEYNOTE_4;
    case KEY2Token::VERSION_STR_5 :
      return VERSION_KEYNOTE_5;
    }
  }

  // TODO: test for v.1 and v.6

  return VERSION_UNKNOWN;
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
  KEYZipStream zipInput(input);
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
    KEYZlibStream compressedInput(input);
    source = SOURCE_APXL_GZ;
    return detectVersionFromInput(WPXInputStreamPtr_t(&compressedInput, KEYDummyDeleter()));
  }
  catch (...)
  {
    // ignore
  }

  source = SOURCE_APXL;
  return detectVersionFromInput(input);
}

KEYDefaults *makeDefaults(const Version version)
{
  switch (version)
  {
  case VERSION_KEYNOTE_1 :
    return new KEY1Defaults();
  case VERSION_KEYNOTE_2 :
  case VERSION_KEYNOTE_3 :
  case VERSION_KEYNOTE_4 :
  case VERSION_KEYNOTE_5 :
    // I am going to suppose these have not changed
    return new KEY2Defaults();
  default :
    KEY_DEBUG_MSG(("unknown version\n"));
    throw GenericException();
  }

  return 0;
}

shared_ptr<KEYParser> makeParser(const Version version, const WPXInputStreamPtr_t &input, const WPXInputStreamPtr_t &package, KEYCollector *const collector, const KEYDefaults &defaults)
{
  shared_ptr<KEYParser> parser;

  switch (version)
  {
  case VERSION_KEYNOTE_1 :
    parser.reset(new KEY1Parser(input, package, collector, defaults));
    break;
  case VERSION_KEYNOTE_2 :
  case VERSION_KEYNOTE_3 :
  case VERSION_KEYNOTE_4 :
  case VERSION_KEYNOTE_5 :
    parser.reset(new KEY2Parser(input, package, collector, defaults));
    break;
  default :
    KEY_DEBUG_MSG(("KEYDocument::parse(): unhandled version\n"));
    break;
  }

  return parser;
}

}

bool KEYDocument::isSupported(WPXInputStream *const input, KEYDocumentType *type) try
{
  if (type)
    *type = KEY_DOCUMENT_TYPE_UNKNOWN;

  Source source = SOURCE_UNKNOWN;
  const Version version = detectVersion(WPXInputStreamPtr_t(input, KEYDummyDeleter()), source);

  if ((VERSION_UNKNOWN != version) && type)
  {
    switch (source)
    {
    case SOURCE_APXL :
    case SOURCE_APXL_GZ :
      *type = KEY_DOCUMENT_TYPE_APXL_FILE;
      break;
    case SOURCE_PACKAGE_APXL :
    case SOURCE_PACKAGE_APXL_GZ :
    case SOURCE_KEY :
      *type = KEY_DOCUMENT_TYPE_PACKAGE;
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

bool KEYDocument::parse(::WPXInputStream *const input, KEYPresentationInterface *const generator) try
{
  WPXInputStreamPtr_t input_(input, KEYDummyDeleter());

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
    input_.reset(new KEYZlibStream(compressed));
    break;
  }
  case SOURCE_KEY :
    package.reset(new KEYZipStream(input_));
    input_.reset(package->getDocumentOLEStream("index.apxl"));
    break;
  default :
    // nothing
    break;
  }

  KEYDictionary dict;
  KEYLayerMap_t masterPages;
  KEYSize presentationSize;
  const scoped_ptr<KEYDefaults> defaults(makeDefaults(version));

  input_->seek(0, WPX_SEEK_SET);

  KEYThemeCollector themeCollector(dict, masterPages, presentationSize, *defaults);
  shared_ptr<KEYParser> parser = makeParser(version, input_, package, &themeCollector, *defaults);
  if (!parser->parse())
    return false;

  input_->seek(0, WPX_SEEK_SET);

  KEYContentCollector contentCollector(generator, dict, masterPages, presentationSize, *defaults);
  parser = makeParser(version, input_, package, &contentCollector, *defaults);
  return parser->parse();
}
catch (...)
{
  return false;
}

bool KEYDocument::generateSVG(::WPXInputStream *const input, KEYStringVector &output) try
{
  KEYSVGGenerator generator(output);
  bool result = KEYDocument::parse(input, &generator);
  return result;
}
catch (...)
{
  return false;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
