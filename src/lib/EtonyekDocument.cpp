/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <libetonyek/libetonyek.h>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/logic/tribool.hpp>

#include <libxml/xmlreader.h>

#include "libetonyek_utils.h"
#include "libetonyek_xml.h"
#include "IWORKZlibStream.h"
#include "KEY1Parser.h"
#include "KEY2Parser.h"
#include "KEY2Token.h"
#include "KEYContentCollector.h"
#include "KEYDictionary.h"
#include "KEYThemeCollector.h"
#include "NUMCollector.h"
#include "NUMParser.h"
#include "NUMToken.h"
#include "PAGCollector.h"
#include "PAGParser.h"
#include "PAGToken.h"
#include "PAGDictionary.h"

using boost::logic::indeterminate;
using boost::logic::tribool;
using boost::scoped_ptr;
using boost::shared_ptr;
using std::string;

using librevenge::RVNG_SEEK_SET;

namespace libetonyek
{

namespace
{

enum CheckType
{
  CHECK_TYPE_NONE = 0,
  CHECK_TYPE_KEYNOTE = 1 << 1,
  CHECK_TYPE_NUMBERS = 1 << 2,
  CHECK_TYPE_PAGES = 1 << 3,
  CHECK_TYPE_ANY = CHECK_TYPE_KEYNOTE | CHECK_TYPE_NUMBERS | CHECK_TYPE_PAGES
};

struct DetectionInfo
{
  DetectionInfo();

  RVNGInputStreamPtr_t input;
  RVNGInputStreamPtr_t package;
  EtonyekDocument::Confidence confidence;
  EtonyekDocument::Type type;
  unsigned version;
};

DetectionInfo::DetectionInfo()
  : input()
  , package()
  , confidence(EtonyekDocument::CONFIDENCE_NONE)
  , type(EtonyekDocument::TYPE_UNKNOWN)
  , version(0)
{
}

typedef boost::shared_ptr<xmlTextReader> xmlTextReader_t;
typedef bool (*ProbeXMLFun_t)(const RVNGInputStreamPtr_t &, unsigned &, const xmlTextReader_t &);
typedef boost::function<int(const char *)> TokenizerFunction_t;


std::string queryAttribute(const xmlTextReaderPtr &reader, const int name, const int ns, const TokenizerFunction_t &tokenizer)
{
  std::string value;

  if (xmlTextReaderHasAttributes(reader))
  {
    int ret = xmlTextReaderMoveToFirstAttribute(reader);
    while (1 == ret)
    {
      const int attributeName = tokenizer(reinterpret_cast<const char *>(xmlTextReaderConstLocalName(reader)));
      const int attributeNameSpace = tokenizer(reinterpret_cast<const char *>(xmlTextReaderConstNamespaceUri(reader)));
      if ((attributeNameSpace == ns) && (attributeName == name))
        value = reinterpret_cast<const char *>(xmlTextReaderConstValue(reader));

      ret = xmlTextReaderMoveToNextAttribute(reader);
    }
  }

  return value;
}


bool probeKeynote1XML(const RVNGInputStreamPtr_t &input, unsigned &version, const xmlTextReader_t &reader)
{
  // TODO: implement me
  (void) input;
  (void) version;
  (void) reader;
  return false;
}

bool probeKeynote2XML(const RVNGInputStreamPtr_t &input, unsigned &version, const xmlTextReader_t &sharedReader)
{
  if (input->isEnd())
    return false;

  const KEY2Tokenizer tokenizer = KEY2Tokenizer();
  const xmlTextReaderPtr reader = sharedReader.get();
  if (!bool(reader))
    throw XMLException();

  int ret = 0;
  do
  {
    ret = xmlTextReaderRead(reader);
  }
  while ((1 == ret) && (XML_READER_TYPE_ELEMENT != xmlTextReaderNodeType(reader)));

  if (1 != ret)
    return false;

  const int name = tokenizer(reinterpret_cast<const char *>(xmlTextReaderConstLocalName(reader)));
  const int ns = tokenizer(reinterpret_cast<const char *>(xmlTextReaderConstNamespaceUri(reader)) ? reinterpret_cast<const char *>(xmlTextReaderConstNamespaceUri(reader)) : "");
  assert((0 == ns) || (ns > name));

  if ((KEY2Token::NS_URI_KEY | KEY2Token::presentation) == (name | ns))
  {

    const std::string v = queryAttribute(reader, KEY2Token::version, KEY2Token::NS_URI_KEY, tokenizer);

    switch (tokenizer(v.c_str()))
    {
    case KEY2Token::VERSION_STR_2 :
      version = 2;
      return true;
    case KEY2Token::VERSION_STR_3 :
      version = 3;
      return true;
    case KEY2Token::VERSION_STR_4 :
      version = 4;
      return true;
    case KEY2Token::VERSION_STR_5 :
      version = 5;
      return true;
    }
  }

  return false;
}

bool probeKeynoteXML(const RVNGInputStreamPtr_t &input, unsigned &version, const xmlTextReader_t &reader)
{
  if (probeKeynote2XML(input, version, reader))
    return true;

  input->seek(0, RVNG_SEEK_SET);

  return probeKeynote1XML(input, version, reader);
}

bool probeNumbersXML(const RVNGInputStreamPtr_t &input, unsigned &version, const xmlTextReader_t &sharedReader)
{
  if (input->isEnd())
    return false;

  const NUMTokenizer tokenizer = NUMTokenizer();
  const xmlTextReaderPtr reader = sharedReader.get();
  if (!bool(reader))
    throw XMLException();

  int ret = 0;
  do
  {
    ret = xmlTextReaderRead(reader);
  }
  while ((1 == ret) && (XML_READER_TYPE_ELEMENT != xmlTextReaderNodeType(reader)));

  if (1 != ret)
    throw XMLException();

  const int name = tokenizer(reinterpret_cast<const char *>(xmlTextReaderConstLocalName(reader)));
  const int ns = tokenizer(reinterpret_cast<const char *>(xmlTextReaderConstNamespaceUri(reader)) ? reinterpret_cast<const char *>(xmlTextReaderConstNamespaceUri(reader)) : "");
  assert((0 == ns) || (ns > name));

  if (NUMToken::NS_URI_LS == ns)
  {

    const std::string v = queryAttribute(reader, NUMToken::version, NUMToken::NS_URI_LS, tokenizer);

    switch (tokenizer(v.c_str()))
    {
    case NUMToken::VERSION_STR_2 :
      version = 2;
      return true;
    }
  }

  return false;
}

bool probePagesXML(const RVNGInputStreamPtr_t &input, unsigned &version, const xmlTextReader_t &sharedReader)
{
  if (input->isEnd())
    return false;

  const PAGTokenizer tokenizer = PAGTokenizer();
  const xmlTextReaderPtr reader = sharedReader.get();
  if (!bool(reader))
    throw XMLException();

  int ret = 0;
  do
  {
    ret = xmlTextReaderRead(reader);
  }
  while ((1 == ret) && (XML_READER_TYPE_ELEMENT != xmlTextReaderNodeType(reader)));

  if (1 != ret)
    throw XMLException();

  const int name = tokenizer(reinterpret_cast<const char *>(xmlTextReaderConstLocalName(reader)));
  const int ns = tokenizer(reinterpret_cast<const char *>(xmlTextReaderConstNamespaceUri(reader)) ? reinterpret_cast<const char *>(xmlTextReaderConstNamespaceUri(reader)) : "");
  assert((0 == ns) || (ns > name));

  if ((PAGToken::NS_URI_SL | PAGToken::document) == (name | ns))
  {

    const std::string v = queryAttribute(reader, PAGToken::version, PAGToken::NS_URI_SL, tokenizer);

    switch (tokenizer(v.c_str()))
    {
    case PAGToken::VERSION_STR_4 :
      version = 4;
      return true;
    }
  }

  return false;
}

bool probeXMLImpl(const RVNGInputStreamPtr_t &input, const ProbeXMLFun_t probe, const EtonyekDocument::Type type, DetectionInfo &info, const xmlTextReader_t &reader)
{
  if (probe(input, info.version, reader))
  {
    info.type = type;
    return true;
  }

  return false;
}

bool probeXML(const ProbeXMLFun_t probe, const EtonyekDocument::Type type, tribool &isGzipped, DetectionInfo &info, const xmlTextReader_t &reader)
{
  if (isGzipped || indeterminate(isGzipped))
  {
    try
    {
      const RVNGInputStreamPtr_t uncompressed(new IWORKZlibStream(info.input));
      isGzipped = true;

      if (probeXMLImpl(uncompressed, probe, type, info, reader))
      {
        info.input = uncompressed;
        return true;
      }
      else
      {
        return false; // compressed, but invalid format
      }
    }
    catch (...)
    {
      if (isGzipped) // decompression failed? most probably a broken file...
        return false;

      isGzipped = false;
    }

    info.input->seek(0, RVNG_SEEK_SET);
  }

  assert(!isGzipped);

  return probeXMLImpl(info.input, probe, type, info, reader);
}

bool detect(const RVNGInputStreamPtr_t &input, unsigned checkTypes, DetectionInfo &info)
{
  info.confidence = EtonyekDocument::CONFIDENCE_SUPPORTED_PART;
  const xmlTextReader_t reader = shared_ptr<xmlTextReader>(xmlReaderForIO(readFromStream, closeStream, input.get(), "", 0, 0), xmlFreeTextReader);
  bool isXML = true;
  tribool isGzipped = indeterminate;
  tribool isKeynote1 = indeterminate;

  if (input->isStructured())
  {
    info.package = input;

    // check which format it might be
    if (CHECK_TYPE_KEYNOTE & checkTypes)
    {
      if (input->existsSubStream("index.apxl"))
      {
        checkTypes = CHECK_TYPE_KEYNOTE;
        isGzipped = false;
        isKeynote1 = false;
        info.input.reset(input->getSubStreamByName("index.apxl"));
      }
      else if (input->existsSubStream("index.apxl.gz"))
      {
        checkTypes = CHECK_TYPE_KEYNOTE;
        isGzipped = true;
        isKeynote1 = false;
        info.input.reset(input->getSubStreamByName("index.apxl.gz"));
      }
      else if (input->existsSubStream("presentation.apxl"))
      {
        checkTypes = CHECK_TYPE_KEYNOTE;
        isGzipped = false;
        isKeynote1 = true;
        info.input.reset(input->getSubStreamByName("presentation.apxl"));
      }
      else if (input->existsSubStream("presentation.apxl.gz"))
      {
        checkTypes = CHECK_TYPE_KEYNOTE;
        isGzipped = true;
        isKeynote1 = true;
        info.input.reset(input->getSubStreamByName("presentation.apxl.gz"));
      }
    }

    if ((CHECK_TYPE_NUMBERS | CHECK_TYPE_PAGES) & checkTypes)
    {
      if (input->existsSubStream("index.xml"))
      {
        checkTypes &= (CHECK_TYPE_NUMBERS | CHECK_TYPE_PAGES);
        isGzipped = false;
        info.input.reset(input->getSubStreamByName("index.xml"));
      }
      else if (input->existsSubStream("index.xml.gz"))
      {
        checkTypes &= (CHECK_TYPE_NUMBERS | CHECK_TYPE_PAGES);
        isGzipped = false;
        info.input.reset(input->getSubStreamByName("index.xml.gz"));
      }
    }

    if (!info.input && (CHECK_TYPE_ANY & checkTypes))
    {
      if (input->existsSubStream("Index.zip"))
      {
        isXML = false;
        info.input.reset(input->getSubStreamByName("Index.zip"));
      }
    }

    if (!info.input)
    {
      // nothing detected
      // TODO: this might also be Index.zip...
      return EtonyekDocument::CONFIDENCE_NONE;
    }

    info.confidence = EtonyekDocument::CONFIDENCE_EXCELLENT; // this is either a valid package of a false positive
  }
  else
  {
    info.input = input;
  }

  assert(bool(info.input));

  if (isXML)
  {
    assert(CHECK_TYPE_ANY & checkTypes);
    assert(!info.input->isStructured());

    info.input->seek(0, RVNG_SEEK_SET);

    if (CHECK_TYPE_KEYNOTE & checkTypes)
    {
      const ProbeXMLFun_t probe = (isKeynote1 ? probeKeynote1XML : ((!isKeynote1) ? probeKeynote2XML : probeKeynoteXML));
      if (probeXML(probe, EtonyekDocument::TYPE_KEYNOTE, isGzipped, info, reader))
        return true;

      info.input->seek(0, RVNG_SEEK_SET);
    }

    if (CHECK_TYPE_NUMBERS & checkTypes)
    {
      if (probeXML(probeNumbersXML, EtonyekDocument::TYPE_NUMBERS, isGzipped, info, reader))
        return true;

      info.input->seek(0, RVNG_SEEK_SET);
    }

    if (CHECK_TYPE_PAGES & checkTypes)
    {
      if (probeXML(probePagesXML, EtonyekDocument::TYPE_PAGES, isGzipped, info, reader))
        return true;
    }
  }
  else
  {
    // TODO: detect type in binary format
  }

  return EtonyekDocument::CONFIDENCE_NONE;
}

}

namespace
{

shared_ptr<IWORKParser> makeKeynoteParser(const unsigned version, const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, KEYCollector *const collector, KEYDictionary &dict)
{
  shared_ptr<IWORKParser> parser;

  if (1 == version)
    parser.reset(new KEY1Parser(input, package, collector));
  else if ((2 <= version) && (5 >= version))
    parser.reset(new KEY2Parser(input, package, collector, dict));
  else
    assert(0);

  return parser;
}

}

ETONYEKAPI EtonyekDocument::Confidence EtonyekDocument::isSupported(librevenge::RVNGInputStream *const input, EtonyekDocument::Type *type) try
{
  if (!input)
    return CONFIDENCE_NONE;

  if (type)
    *type = TYPE_UNKNOWN;

  DetectionInfo info;

  if (detect(RVNGInputStreamPtr_t(input, EtonyekDummyDeleter()), CHECK_TYPE_ANY, info))
  {
    assert(TYPE_UNKNOWN != info.type);
    assert(CONFIDENCE_NONE != info.confidence);
    assert(bool(info.input));

    if (type)
      *type = info.type;
    return info.confidence;
  }

  return CONFIDENCE_NONE;
}
catch (...)
{
  return CONFIDENCE_NONE;
}

ETONYEKAPI bool EtonyekDocument::parse(librevenge::RVNGInputStream *const input, librevenge::RVNGPresentationInterface *const generator) try
{
  if (!input || !generator)
    return false;

  DetectionInfo info;

  if (!detect(RVNGInputStreamPtr_t(input, EtonyekDummyDeleter()), CHECK_TYPE_KEYNOTE, info))
    return false;

  assert(TYPE_UNKNOWN != info.type);
  assert(CONFIDENCE_NONE != info.confidence);
  assert(bool(info.input));
  assert(0 != info.version);

  KEYDictionary dict;
  IWORKSize presentationSize;

  info.input->seek(0, librevenge::RVNG_SEEK_SET);

  KEYThemeCollector themeCollector(presentationSize);
  shared_ptr<IWORKParser> parser = makeKeynoteParser(info.version, info.input, info.package, &themeCollector, dict);
  if (!parser->parse())
    return false;

  info.input->seek(0, librevenge::RVNG_SEEK_SET);

  dict.m_locked = true; // do not add refs again
  KEYContentCollector contentCollector(generator, presentationSize);
  parser = makeKeynoteParser(info.version, info.input, info.package, &contentCollector, dict);
  return parser->parse();
}
catch (...)
{
  return false;
}

ETONYEKAPI bool EtonyekDocument::parse(librevenge::RVNGInputStream *const input, librevenge::RVNGSpreadsheetInterface *const document) try
{
  if (!input || !document)
    return false;

  DetectionInfo info;

  if (!detect(RVNGInputStreamPtr_t(input, EtonyekDummyDeleter()), CHECK_TYPE_NUMBERS, info))
    return false;

  assert(TYPE_UNKNOWN != info.type);
  assert(CONFIDENCE_NONE != info.confidence);
  assert(bool(info.input));
  assert(0 != info.version);

  info.input->seek(0, librevenge::RVNG_SEEK_SET);

  NUMCollector collector(document);
  NUMParser parser(info.input, info.package, &collector);
  return parser.parse();
}
catch (...)
{
  return false;
}

ETONYEKAPI bool EtonyekDocument::parse(librevenge::RVNGInputStream *const input, librevenge::RVNGTextInterface *const document) try
{
  if (!input || !document)
    return false;

  DetectionInfo info;

  if (!detect(RVNGInputStreamPtr_t(input, EtonyekDummyDeleter()), CHECK_TYPE_PAGES, info))
    return false;

  assert(TYPE_UNKNOWN != info.type);
  assert(CONFIDENCE_NONE != info.confidence);
  assert(bool(info.input));
  assert(0 != info.version);

  info.input->seek(0, librevenge::RVNG_SEEK_SET);

  PAGCollector collector(document);
  PAGDictionary dict;
  PAGParser parser(info.input, info.package, &collector, &dict);
  return parser.parse();
}
catch (...)
{
  return false;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
