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

#include <libetonyek/libetonyek.h>

#include "libetonyek_utils.h"
#include "KEYZlibStream.h"
#include "NUMBERSCollector.h"
#include "NUMBERSParser.h"

using boost::scoped_ptr;
using boost::shared_ptr;

namespace libetonyek
{

namespace
{

enum Version
{
  VERSION_UNKNOWN,
  VERSION_NUMBERS_1
};

enum Source
{
  SOURCE_UNKNOWN,
  SOURCE_XML,
  SOURCE_XML_GZ,
  SOURCE_PACKAGE_XML,
  SOURCE_PACKAGE_XML_GZ,
  SOURCE_ARCHIVE
};

Version detectVersionFromInput(const RVNGInputStreamPtr_t &input)
{
  // TODO: do a real detection
  (void) input;
  return VERSION_NUMBERS_1;
}

Version detectVersion(const RVNGInputStreamPtr_t &input, Source &source)
{
  source = SOURCE_UNKNOWN;

  // check if this is a package
  if (input->isStructured())
  {
    scoped_ptr<librevenge::RVNGInputStream> xml;

    xml.reset(input->getSubStreamByName("index.xml.gz"));
    if (bool(xml))
    {
      source = SOURCE_PACKAGE_XML_GZ;
      return VERSION_NUMBERS_1;
    }

    xml.reset(input->getSubStreamByName("index.xml"));
    if (bool(xml))
    {
      source = SOURCE_PACKAGE_XML;
      return VERSION_NUMBERS_1;
    }
  }

  try
  {
    KEYZlibStream compressedInput(input);
    source = SOURCE_XML_GZ;
    return detectVersionFromInput(RVNGInputStreamPtr_t(&compressedInput, KEYDummyDeleter()));
  }
  catch (...)
  {
    // ignore
  }

  source = SOURCE_XML;
  return detectVersionFromInput(input);
}

}

KEYAPI bool NUMBERSDocument::isSupported(librevenge::RVNGInputStream *const input, NUMBERSDocumentType *const type) try
{
  if (type)
    *type = NUMBERS_DOCUMENT_TYPE_UNKNOWN;

  Source source = SOURCE_UNKNOWN;
  const Version version = detectVersion(RVNGInputStreamPtr_t(input, KEYDummyDeleter()), source);

  if ((VERSION_UNKNOWN != version) && type)
  {
    switch (source)
    {
    case SOURCE_XML :
    case SOURCE_XML_GZ :
      *type = NUMBERS_DOCUMENT_TYPE_MAIN_FILE;
      break;
    case SOURCE_PACKAGE_XML :
    case SOURCE_PACKAGE_XML_GZ :
    case SOURCE_ARCHIVE :
      *type = NUMBERS_DOCUMENT_TYPE_PACKAGE;
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

KEYAPI bool NUMBERSDocument::parse(librevenge::RVNGInputStream *const input, librevenge::RVNGSpreadsheetInterface *const document) try
{
  RVNGInputStreamPtr_t input_(input, KEYDummyDeleter());

  Source source = SOURCE_UNKNOWN;
  const Version version = detectVersion(input_, source);

  if (VERSION_UNKNOWN == version)
    return false;

  RVNGInputStreamPtr_t package;

  switch (source)
  {
  case SOURCE_ARCHIVE :
  case SOURCE_PACKAGE_XML :
    package = input_;
    input_.reset(package->getSubStreamByName("index.xml"));
    break;
  case SOURCE_PACKAGE_XML_GZ :
  {
    package = input_;
    const RVNGInputStreamPtr_t compressed(package->getSubStreamByName("index.xml.gz"));
    input_.reset(new KEYZlibStream(compressed));
    break;
  }
  default :
    break;
  }

  input_->seek(0, librevenge::RVNG_SEEK_SET);

  NUMBERSCollector collector(document);
  NUMBERSParser parser(input_, package, &collector);
  return parser.parse();
}
catch (...)
{
  return false;
}

} // namespace libetonyek

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
