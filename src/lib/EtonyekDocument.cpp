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
#include "IWORKPresentationRedirector.h"
#include "IWORKSpreadsheetRedirector.h"
#include "IWORKTextRedirector.h"
#include "IWORKTokenizer.h"
#include "IWORKZlibStream.h"
#include "KEY1Parser.h"
#include "KEY2Parser.h"
#include "KEY2Token.h"
#include "KEYCollector.h"
#include "KEYDictionary.h"
#include "NUMCollector.h"
#include "NUMDictionary.h"
#include "NUM1Parser.h"
#include "NUM1Token.h"
#include "PAGCollector.h"
#include "PAG1Parser.h"
#include "PAG1Token.h"
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

typedef bool (*ProbeXMLFun_t)(const RVNGInputStreamPtr_t &, unsigned &, xmlTextReaderPtr);


std::string queryAttribute(xmlTextReaderPtr reader, const int name, const int ns, const IWORKTokenizer &tokenizer)
{
  if (xmlTextReaderHasAttributes(reader))
  {
    int ret = xmlTextReaderMoveToFirstAttribute(reader);
    while (1 == ret)
    {
      const int id = tokenizer.getQualifiedId(char_cast(xmlTextReaderConstLocalName(reader)), char_cast(xmlTextReaderConstNamespaceUri(reader)));
      if ((ns | name) == id)
        return char_cast(xmlTextReaderConstValue(reader));

      ret = xmlTextReaderMoveToNextAttribute(reader);
    }
  }

  return "";
}


bool probeKeynote1XML(const RVNGInputStreamPtr_t &input, unsigned &version, xmlTextReaderPtr reader)
{
  // TODO: implement me
  (void) input;
  (void) version;
  (void) reader;
  return false;
}

bool probeKeynote2XML(const RVNGInputStreamPtr_t &input, unsigned &version, xmlTextReaderPtr reader)
{
  if (input->isEnd())
    return false;

  const IWORKTokenizer &tokenizer(KEY2Token::getTokenizer());
  assert(reader);

  const int id = tokenizer.getQualifiedId(char_cast(xmlTextReaderConstLocalName(reader)), char_cast(xmlTextReaderConstNamespaceUri(reader)));

  if ((KEY2Token::NS_URI_KEY | KEY2Token::presentation) == id)
  {
    const std::string v = queryAttribute(reader, KEY2Token::version, KEY2Token::NS_URI_KEY, tokenizer);

    switch (tokenizer.getId(v.c_str()))
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

bool probeKeynoteXML(const RVNGInputStreamPtr_t &input, unsigned &version, xmlTextReaderPtr reader)
{
  if (probeKeynote2XML(input, version, reader))
    return true;

  input->seek(0, RVNG_SEEK_SET);

  return probeKeynote1XML(input, version, reader);
}

bool probeNumbersXML(const RVNGInputStreamPtr_t &input, unsigned &version, xmlTextReaderPtr reader)
{
  if (input->isEnd())
    return false;

  const IWORKTokenizer &tokenizer(NUM1Token::getTokenizer());
  assert(reader);

  const int id = tokenizer.getQualifiedId(char_cast(xmlTextReaderConstLocalName(reader)), char_cast(xmlTextReaderConstNamespaceUri(reader)));

  if ((NUM1Token::NS_URI_LS | NUM1Token::document) == id)
  {
    const std::string v = queryAttribute(reader, NUM1Token::version, NUM1Token::NS_URI_LS, tokenizer);

    switch (tokenizer.getId(v.c_str()))
    {
    case NUM1Token::VERSION_STR_2 :
      version = 2;
      return true;
    }
  }

  return false;
}

bool probePagesXML(const RVNGInputStreamPtr_t &input, unsigned &version, xmlTextReaderPtr reader)
{
  if (input->isEnd())
    return false;

  const IWORKTokenizer &tokenizer(PAG1Token::getTokenizer());
  assert(reader);

  const int id = tokenizer.getQualifiedId(char_cast(xmlTextReaderConstLocalName(reader)), char_cast(xmlTextReaderConstNamespaceUri(reader)));

  if ((PAG1Token::NS_URI_SL | PAG1Token::document) == id)
  {
    const std::string v = queryAttribute(reader, PAG1Token::version, PAG1Token::NS_URI_SL, tokenizer);

    switch (tokenizer.getId(v.c_str()))
    {
    case PAG1Token::VERSION_STR_4 :
      version = 4;
      return true;
    }
  }

  return false;
}

bool probeXMLImpl(const RVNGInputStreamPtr_t &input, const ProbeXMLFun_t probe, const EtonyekDocument::Type type, DetectionInfo &info)
{
  const shared_ptr<xmlTextReader> reader(xmlReaderForIO(readFromStream, closeStream, input.get(), "", 0, 0), xmlFreeTextReader);
  if (!reader)
    return false;

  int ret = 0;
  do
  {
    ret = xmlTextReaderRead(reader.get());
  }
  while ((1 == ret) && (XML_READER_TYPE_ELEMENT != xmlTextReaderNodeType(reader.get())));

  if (1 != ret)
    return false;

  if (probe(input, info.version, reader.get()))
  {
    info.type = type;
    return true;
  }

  return false;
}

bool probeXML(const ProbeXMLFun_t probe, const EtonyekDocument::Type type, tribool &isGzipped, DetectionInfo &info)
{
  if (isGzipped || indeterminate(isGzipped))
  {
    try
    {
      const RVNGInputStreamPtr_t uncompressed(new IWORKZlibStream(info.input));
      isGzipped = true;

      if (probeXMLImpl(uncompressed, probe, type, info))
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

  assert(isGzipped == false);

  return probeXMLImpl(info.input, probe, type, info);
}

bool detect(const RVNGInputStreamPtr_t &input, unsigned checkTypes, DetectionInfo &info)
{
  info.confidence = EtonyekDocument::CONFIDENCE_SUPPORTED_PART;
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
      if (probeXML(probe, EtonyekDocument::TYPE_KEYNOTE, isGzipped, info))
        return true;

      info.input->seek(0, RVNG_SEEK_SET);
    }

    if (CHECK_TYPE_NUMBERS & checkTypes)
    {
      if (probeXML(probeNumbersXML, EtonyekDocument::TYPE_NUMBERS, isGzipped, info))
        return true;

      info.input->seek(0, RVNG_SEEK_SET);
    }

    if (CHECK_TYPE_PAGES & checkTypes)
    {
      if (probeXML(probePagesXML, EtonyekDocument::TYPE_PAGES, isGzipped, info))
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

  info.input->seek(0, librevenge::RVNG_SEEK_SET);

  KEYDictionary dict;
  IWORKPresentationRedirector redirector(generator);
  KEYCollector collector(&redirector);
  const shared_ptr<IWORKParser> parser = makeKeynoteParser(info.version, info.input, info.package, &collector, dict);
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

  IWORKSpreadsheetRedirector redirector(document);
  NUMCollector collector(&redirector);
  NUMDictionary dict;
  NUM1Parser parser(info.input, info.package, &collector, &dict);
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

  IWORKTextRedirector redirector(document);
  PAGCollector collector(&redirector);
  PAGDictionary dict;
  PAG1Parser parser(info.input, info.package, &collector, &dict);
  return parser.parse();
}
catch (...)
{
  return false;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
