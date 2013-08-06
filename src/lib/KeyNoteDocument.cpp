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
#include "KNCollector.h"
#include "KN1Parser.h"
#include "KN2Parser.h"
#include "KNSVGGenerator.h"
#include "KNZipStream.h"
#include "KNZlibStream.h"

using boost::scoped_ptr;
using boost::shared_ptr;

namespace libkeynote
{

namespace
{

enum Version
{
  VERSION_UNKNOWN,
  VERSION_KEYNOTE_1,
  VERSION_KEYNOTE_2
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

Version detectVersionFromInput(WPXInputStream *const input)
{
  // TODO: do a real detection
  (void) input;
  return VERSION_KEYNOTE_2;
}

Version detectVersion(WPXInputStream *const input, Source &source)
{
  source = SOURCE_UNKNOWN;

  // check if this is a package
  if (input->isOLEStream())
  {
    scoped_ptr<WPXInputStream> apxl(input->getDocumentOLEStream("index.apxl"));
    if (bool(apxl))
    {
      source = SOURCE_PACKAGE_APXL;
      return VERSION_KEYNOTE_2;
    }

    apxl.reset(input->getDocumentOLEStream("index.apxl.gz"));
    if (bool(apxl))
    {
      source = SOURCE_PACKAGE_APXL_GZ;
      return VERSION_KEYNOTE_2;
    }

    apxl.reset(input->getDocumentOLEStream("presentation.apxl"));
    if (bool(apxl))
    {
      source = SOURCE_PACKAGE_APXL;
      return VERSION_KEYNOTE_1;
    }

    apxl.reset(input->getDocumentOLEStream("presentation.apxl.gz"));
    if (bool(apxl))
    {
      source = SOURCE_PACKAGE_APXL_GZ;
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
      return VERSION_KEYNOTE_2;
    }
  }

  try
  {
    KNZlibStream compressedInput(input);
    source = SOURCE_APXL_GZ;
    return detectVersionFromInput(&compressedInput);
  }
  catch (...)
  {
    // ignore
  }

  source = SOURCE_APXL;
  return detectVersionFromInput(input);
}

Version detectVersion(WPXInputStream *const input)
{
  Source dummy;
  return detectVersion(input, dummy);
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
  CompositeStream(WPXInputStream *input, Version version, Source source);
  virtual ~CompositeStream();

  virtual bool isOLEStream();
  virtual WPXInputStream *getDocumentOLEStream(const char *name);

  virtual const unsigned char *read(unsigned long numBytes, unsigned long &numBytesRead);
  virtual int seek(long offset, WPX_SEEK_TYPE seekType);
  virtual long tell();
  virtual bool atEOS();

private:
  shared_ptr<WPXInputStream> m_input;
  shared_ptr<WPXInputStream> m_dir;
};

struct DoNotDelete
{
  void operator()(void *)
  {
  }
};

CompositeStream::CompositeStream(WPXInputStream *const input, const Version version, const Source source)
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
    m_input.reset(input, DoNotDelete());
    m_dir.reset(new DummyOLEStream());
    break;
  case SOURCE_APXL_GZ :
    m_input.reset(new KNZlibStream(input));
    m_dir.reset(new DummyOLEStream());
    break;
  case SOURCE_PACKAGE_APXL :
    if (VERSION_KEYNOTE_1 == version)
      m_input.reset(input->getDocumentOLEStream("presentation.apxl"));
    else if (VERSION_KEYNOTE_2 == version)
      m_input.reset(input->getDocumentOLEStream("index.apxl"));
    m_dir.reset(input, DoNotDelete());
    break;
  case SOURCE_PACKAGE_APXL_GZ :
  {
    scoped_ptr<WPXInputStream> compressedInput;
    if (VERSION_KEYNOTE_1 == version)
      compressedInput.reset(input->getDocumentOLEStream("presentation.apxl.gz"));
    else if (VERSION_KEYNOTE_2 == version)
      compressedInput.reset(input->getDocumentOLEStream("index.apxl.gz"));
    m_input.reset(new KNZlibStream(compressedInput.get()));
    m_dir.reset(input, DoNotDelete());
    break;
  }
  case SOURCE_KEY :
    m_input.reset(input->getDocumentOLEStream("index.apxl"));
    m_dir.reset(input, DoNotDelete());
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
  const Version version = detectVersion(input);
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
\param painter A WPGPainterInterface implementation
\return A value that indicates whether the parsing was successful
*/
bool KeyNoteDocument::parse(::WPXInputStream *const input, libwpg::WPGPaintInterface *const painter) try
{
  Source source = SOURCE_UNKNOWN;
  const Version version = detectVersion(input, source);

  if (VERSION_UNKNOWN == version)
    return false;

  CompositeStream compositeInput(input, version, source);
  KNCollector collector(painter);

  switch (version)
  {
  case VERSION_KEYNOTE_1 :
  {
    KN1Parser parser(&compositeInput, &collector);
    return parser.parse();
  }
  case VERSION_KEYNOTE_2 :
  {
    KN2Parser parser(&compositeInput, &collector);
    return parser.parse();
  }
  default :
    KN_DEBUG_MSG(("KeyNoteDocument::parse(): unhandled version\n"));
    break;
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
