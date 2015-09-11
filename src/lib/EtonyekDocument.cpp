/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <libetonyek/libetonyek.h>

#include <cassert>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

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

using boost::scoped_ptr;
using boost::shared_ptr;
using std::string;

using librevenge::RVNG_SEEK_SET;

namespace libetonyek
{

namespace
{

enum Format
{
  FORMAT_UNKNOWN,
  FORMAT_XML1,
  FORMAT_XML2,
  FORMAT_BINARY
};

struct DetectionInfo
{
  explicit DetectionInfo(EtonyekDocument::Type type = EtonyekDocument::TYPE_UNKNOWN);

  RVNGInputStreamPtr_t m_input;
  RVNGInputStreamPtr_t m_package;
  EtonyekDocument::Confidence m_confidence;
  EtonyekDocument::Type m_type;
  Format m_format;
};

DetectionInfo::DetectionInfo(const EtonyekDocument::Type type)
  : m_input()
  , m_package()
  , m_confidence(EtonyekDocument::CONFIDENCE_NONE)
  , m_type(type)
  , m_format(FORMAT_UNKNOWN)
{
}

bool probeXMLFormat(const Format format, const EtonyekDocument::Type type, const int docId,
                    const IWORKTokenizer &tokenizer, const char *const name, const char *const ns,
                    DetectionInfo &info)
{
  if (((info.m_format == format) || (info.m_format == FORMAT_UNKNOWN))
      && ((info.m_type == type) || info.m_type == EtonyekDocument::TYPE_UNKNOWN))
  {
    if (tokenizer.getQualifiedId(name, ns) == docId)
    {
      info.m_format = format;
      info.m_type = type;
      return true;
    }
  }
  return false;
}

bool probeXML(DetectionInfo &info)
{
  const shared_ptr<xmlTextReader> reader(xmlReaderForIO(readFromStream, closeStream, info.m_input.get(), "", 0, 0), xmlFreeTextReader);
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

  const char *const name = char_cast(xmlTextReaderConstLocalName(reader.get()));
  const char *const ns = char_cast(xmlTextReaderConstNamespaceUri(reader.get()));

  if (probeXMLFormat(FORMAT_XML2, EtonyekDocument::TYPE_KEYNOTE, KEY2Token::NS_URI_KEY | KEY2Token::presentation,
                     KEY2Token::getTokenizer(), name, ns, info))
    return true;
  if (probeXMLFormat(FORMAT_XML2, EtonyekDocument::TYPE_NUMBERS, NUM1Token::NS_URI_LS | NUM1Token::document,
                     NUM1Token::getTokenizer(), name, ns, info))
    return true;
  if (probeXMLFormat(FORMAT_XML2, EtonyekDocument::TYPE_PAGES, PAG1Token::NS_URI_SL | PAG1Token::document,
                     PAG1Token::getTokenizer(), name, ns, info))
    return true;

  return false;
}

RVNGInputStreamPtr_t getSubStream(const RVNGInputStreamPtr_t &input, const char *const name)
{
  return RVNGInputStreamPtr_t(input->getSubStreamByName(name));
}

RVNGInputStreamPtr_t getUncompressedSubStream(const RVNGInputStreamPtr_t &input, const char *const name) try
{
  const RVNGInputStreamPtr_t compressed(input->getSubStreamByName(name));
  assert(bool(compressed));
  return RVNGInputStreamPtr_t(new IWORKZlibStream(compressed));
}
catch (...)
{
  return RVNGInputStreamPtr_t();
}

bool detect(const RVNGInputStreamPtr_t &input, DetectionInfo &info)
{
  if (input->isStructured())
  {
    info.m_package = input;

    if ((info.m_format == FORMAT_XML2) || (info.m_format == FORMAT_UNKNOWN))
    {
      if ((info.m_type == EtonyekDocument::TYPE_KEYNOTE) || (info.m_type == EtonyekDocument::TYPE_UNKNOWN))
      {
        if (input->existsSubStream("index.apxl"))
        {
          info.m_format = FORMAT_XML2;
          info.m_type = EtonyekDocument::TYPE_KEYNOTE;
          info.m_input = getSubStream(input, "index.apxl");
        }
        else if (input->existsSubStream("index.apxl.gz"))
        {
          info.m_format = FORMAT_XML2;
          info.m_type = EtonyekDocument::TYPE_KEYNOTE;
          info.m_input = getUncompressedSubStream(input, "index.apxl.gz");
        }
      }

      if ((info.m_type == EtonyekDocument::TYPE_NUMBERS) || (info.m_type == EtonyekDocument::TYPE_PAGES) || (info.m_type == EtonyekDocument::TYPE_UNKNOWN))
      {
        if (input->existsSubStream("index.xml"))
        {
          info.m_format = FORMAT_XML2;
          info.m_input = getSubStream(input, "index.xml");
        }
        else if (input->existsSubStream("index.xml.gz"))
        {
          info.m_format = FORMAT_XML2;
          info.m_input = getUncompressedSubStream(input, "index.xml.gz");
        }
      }
    }

    if ((info.m_format == FORMAT_XML1) || (info.m_format == FORMAT_UNKNOWN))
    {
      if (input->existsSubStream("presentation.apxl"))
      {
        info.m_type = EtonyekDocument::TYPE_KEYNOTE;
        info.m_format = FORMAT_XML1;
        info.m_input = getSubStream(input, "presentation.apxl");
      }
      else if (input->existsSubStream("presentation.apxl.gz"))
      {
        info.m_type = EtonyekDocument::TYPE_KEYNOTE;
        info.m_format = FORMAT_XML1;
        info.m_input = getUncompressedSubStream(input, "presentation.apxl.gz");
      }
    }

    if ((info.m_format == FORMAT_BINARY) || (info.m_format == FORMAT_UNKNOWN))
    {
      if (input->existsSubStream("Index.zip"))
      {
        info.m_format = FORMAT_BINARY;
        info.m_input = getSubStream(input, "Index.zip");
      }
    }
  }
  else
  {
    try
    {
      info.m_input.reset(new IWORKZlibStream(input));
    }
    catch (...)
    {
      info.m_input = input;
    }
  }

  if (bool(info.m_input))
  {
    assert(!info.m_input->isStructured());
    info.m_input->seek(0, RVNG_SEEK_SET);

    if (info.m_format == FORMAT_BINARY)
    {
      // TODO: detect type in binary format
    }
    else
    {
      if (probeXML(info))
        info.m_confidence = bool(info.m_package) ? EtonyekDocument::CONFIDENCE_EXCELLENT : EtonyekDocument::CONFIDENCE_SUPPORTED_PART;
    }
  }

  return info.m_confidence != EtonyekDocument::CONFIDENCE_NONE;
}

}

namespace
{

shared_ptr<IWORKParser> makeKeynoteParser(const unsigned version, const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, KEYCollector &collector, KEYDictionary &dict)
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

  if (detect(RVNGInputStreamPtr_t(input, EtonyekDummyDeleter()), info))
  {
    assert(TYPE_UNKNOWN != info.m_type);
    assert(FORMAT_UNKNOWN != info.m_format);
    assert(CONFIDENCE_NONE != info.m_confidence);
    assert(bool(info.m_input));

    if (type)
      *type = info.m_type;
    return info.m_confidence;
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

  DetectionInfo info(EtonyekDocument::TYPE_KEYNOTE);

  if (!detect(RVNGInputStreamPtr_t(input, EtonyekDummyDeleter()), info))
    return false;

  assert(TYPE_UNKNOWN != info.m_type);
  assert(CONFIDENCE_NONE != info.m_confidence);
  assert(bool(info.m_input));
  assert(FORMAT_UNKNOWN != info.m_format);

  info.m_input->seek(0, librevenge::RVNG_SEEK_SET);

  KEYDictionary dict;
  IWORKPresentationRedirector redirector(generator);
  KEYCollector collector(&redirector);
  const shared_ptr<IWORKParser> parser = makeKeynoteParser(info.m_format, info.m_input, info.m_package, collector, dict);
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

  DetectionInfo info(EtonyekDocument::TYPE_NUMBERS);

  if (!detect(RVNGInputStreamPtr_t(input, EtonyekDummyDeleter()), info))
    return false;

  assert(TYPE_UNKNOWN != info.m_type);
  assert(CONFIDENCE_NONE != info.m_confidence);
  assert(bool(info.m_input));
  assert(FORMAT_UNKNOWN != info.m_format);

  info.m_input->seek(0, librevenge::RVNG_SEEK_SET);

  IWORKSpreadsheetRedirector redirector(document);
  NUMCollector collector(&redirector);
  NUMDictionary dict;
  NUM1Parser parser(info.m_input, info.m_package, collector, &dict);
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

  DetectionInfo info(EtonyekDocument::TYPE_PAGES);

  if (!detect(RVNGInputStreamPtr_t(input, EtonyekDummyDeleter()), info))
    return false;

  assert(TYPE_UNKNOWN != info.m_type);
  assert(CONFIDENCE_NONE != info.m_confidence);
  assert(bool(info.m_input));
  assert(FORMAT_UNKNOWN != info.m_format);

  info.m_input->seek(0, librevenge::RVNG_SEEK_SET);

  IWORKTextRedirector redirector(document);
  PAGCollector collector(&redirector);
  PAGDictionary dict;
  PAG1Parser parser(info.m_input, info.m_package, collector, &dict);
  return parser.parse();
}
catch (...)
{
  return false;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
