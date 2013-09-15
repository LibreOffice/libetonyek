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

#include <libkeynote/KeyNoteDocument.h>

#include "libkeynote_utils.h"
#include "KN1Parser.h"
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
  VERSION_KEYNOTE_5,
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

Version detectVersion(const WPXInputStreamPtr_t &input)
{
  Source dummy;
  return detectVersion(input, dummy);
}

KNDefaults *makeDefaults(const Version version)
{
  switch (version)
  {
  case VERSION_KEYNOTE_1 :
  case VERSION_KEYNOTE_2 :
  case VERSION_KEYNOTE_3 :
  case VERSION_KEYNOTE_4 :
  case VERSION_KEYNOTE_5 :
    return 0;
  default :
    KN_DEBUG_MSG(("unknown version\n"));
    throw GenericException();
  }

  return 0;
}

shared_ptr<KNParser> makeParser(const Version version, const WPXInputStreamPtr_t &input, KNCollector *const collector, const KNDefaults &defaults)
{
  shared_ptr<KNParser> parser;

  switch (version)
  {
  case VERSION_KEYNOTE_1 :
    parser.reset(new KN1Parser(input, collector, defaults));
    break;
  case VERSION_KEYNOTE_2 :
  case VERSION_KEYNOTE_3 :
  case VERSION_KEYNOTE_4 :
  case VERSION_KEYNOTE_5 :
    parser.reset(new KN2Parser(input, collector, defaults));
    break;
  default :
    KN_DEBUG_MSG(("KeyNoteDocument::parse(): unhandled version\n"));
    break;
  }

  return parser;
}

}

namespace
{

class DummyOLEStream : public WPXInputStream
{
public:
  virtual bool isOLEStream();
  virtual WPXInputStream *getDocumentOLEStream(const char *name);

  virtual const unsigned char *read(unsigned long numBytes, unsigned long &numBytesRead);
  virtual int seek(long offset, WPX_SEEK_TYPE seekType);
  virtual long tell();
  virtual bool atEOS();
};

bool DummyOLEStream::isOLEStream()
{
  return true;
}

WPXInputStream *DummyOLEStream::getDocumentOLEStream(const char *)
{
  return 0;
}

const unsigned char *DummyOLEStream::read(unsigned long, unsigned long &numBytesRead)
{
  numBytesRead = 0;
  return 0;
}

int DummyOLEStream::seek(long, WPX_SEEK_TYPE)
{
  return -1;
}

long DummyOLEStream::tell()
{
  return 0;
}

bool DummyOLEStream::atEOS()
{
  return true;
}

}

namespace
{

class CompositeStream : public WPXInputStream
{

public:
  CompositeStream(const WPXInputStreamPtr_t &input, Version version, Source source);
  virtual ~CompositeStream();

  virtual bool isOLEStream();
  virtual WPXInputStream *getDocumentOLEStream(const char *name);

  virtual const unsigned char *read(unsigned long numBytes, unsigned long &numBytesRead);
  virtual int seek(long offset, WPX_SEEK_TYPE seekType);
  virtual long tell();
  virtual bool atEOS();

private:
  WPXInputStreamPtr_t m_input;
  WPXInputStreamPtr_t m_dir;
};

CompositeStream::CompositeStream(const WPXInputStreamPtr_t &input, const Version version, const Source source)
  : m_input()
  , m_dir()
{
  if (VERSION_UNKNOWN == version)
  {
    KN_DEBUG_MSG(("cannot create a stream for unknown version\n"));
    throw GenericException();
  }

  switch (source)
  {
  case SOURCE_APXL :
    m_input = input;
    m_dir.reset(new DummyOLEStream());
    break;
  case SOURCE_APXL_GZ :
    m_input.reset(new KNZlibStream(input));
    m_dir.reset(new DummyOLEStream());
    break;
  case SOURCE_PACKAGE_APXL :
    if (VERSION_KEYNOTE_1 == version)
      m_input.reset(input->getDocumentOLEStream("presentation.apxl"));
    else if (VERSION_UNKNOWN != version)
      m_input.reset(input->getDocumentOLEStream("index.apxl"));
    m_dir = input;
    break;
  case SOURCE_PACKAGE_APXL_GZ :
  {
    WPXInputStreamPtr_t compressedInput;
    if (VERSION_KEYNOTE_1 == version)
      compressedInput.reset(input->getDocumentOLEStream("presentation.apxl.gz"));
    else if (VERSION_UNKNOWN != version)
      compressedInput.reset(input->getDocumentOLEStream("index.apxl.gz"));
    m_input.reset(new KNZlibStream(compressedInput));
    m_dir = input;
    break;
  }
  case SOURCE_KEY :
    m_input.reset(input->getDocumentOLEStream("index.apxl"));
    m_dir = input;
    break;
  default :
    KN_DEBUG_MSG(("cannot create a stream for unknown source type\n"));
    throw GenericException();
  }
}

CompositeStream::~CompositeStream()
{
}

bool CompositeStream::isOLEStream()
{
  return true;
}

WPXInputStream *CompositeStream::getDocumentOLEStream(const char *const name)
{
  return m_dir->getDocumentOLEStream(name);
}

const unsigned char *CompositeStream::read(const unsigned long numBytes, unsigned long &numBytesRead)
{
  return m_input->read(numBytes, numBytesRead);
}

int CompositeStream::seek(const long offset, const WPX_SEEK_TYPE seekType)
{
  return m_input->seek(offset, seekType);
}

long CompositeStream::tell()
{
  return m_input->tell();
}

bool CompositeStream::atEOS()
{
  return m_input->atEOS();
}

}

/**
Analyzes the content of an input stream to see if it can be parsed
\param input The input stream
\return A value that indicates whether the content from the input
stream is a KeyNote Document that libkeynote is able to parse
*/
bool KeyNoteDocument::isSupported(WPXInputStream *const input) try
{
  const Version version = detectVersion(WPXInputStreamPtr_t(input, KNDummyDeleter()));
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
bool KeyNoteDocument::parse(::WPXInputStream *const input, libwpg::WPGPaintInterface *const painter) try
{
  WPXInputStreamPtr_t input_(input, KNDummyDeleter());

  Source source = SOURCE_UNKNOWN;
  const Version version = detectVersion(input_, source);

  if (VERSION_UNKNOWN == version)
    return false;

  CompositeStream compositeInput(input_, version, source);
  const WPXInputStreamPtr_t compositeInput_(&compositeInput, KNDummyDeleter());

  KNDictionary dict;
  KNLayerMap_t masterPages;
  KNSize presentationSize;
  const scoped_ptr<KNDefaults> defaults(makeDefaults(version));

  compositeInput.seek(0, WPX_SEEK_SET);

  KNThemeCollector themeCollector(dict, masterPages, presentationSize, *defaults);
  shared_ptr<KNParser> parser = makeParser(version, compositeInput_, &themeCollector, *defaults);
  if (!parser->parse())
    return false;

  compositeInput.seek(0, WPX_SEEK_SET);

  KNContentCollector contentCollector(painter, dict, masterPages, presentationSize, *defaults);
  parser = makeParser(version, compositeInput_, &contentCollector, *defaults);
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
