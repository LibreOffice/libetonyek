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
#include <cstring>
#include <memory>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/optional.hpp>

#include <libxml/xmlreader.h>

#include "libetonyek_utils.h"
#include "libetonyek_xml.h"
#include "IWAMessage.h"
#include "IWASnappyStream.h"
#include "IWORKPresentationRedirector.h"
#include "IWORKSpreadsheetRedirector.h"
#include "IWORKSubDirStream.h"
#include "IWORKTextRedirector.h"
#include "IWORKTokenizer.h"
#include "IWORKZlibStream.h"
#include "KEY1Dictionary.h"
#include "KEY1Parser.h"
#include "KEY1Token.h"
#include "KEY2Dictionary.h"
#include "KEY2Parser.h"
#include "KEY2Token.h"
#include "KEY6Parser.h"
#include "KEYCollector.h"
#include "NUMCollector.h"
#include "NUM1Dictionary.h"
#include "NUM1Parser.h"
#include "NUM1Token.h"
#include "NUM3Parser.h"
#include "PAGCollector.h"
#include "PAG1Dictionary.h"
#include "PAG1Parser.h"
#include "PAG1Token.h"
#include "PAG5Parser.h"


using std::shared_ptr;
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
  RVNGInputStreamPtr_t m_fragments;
  EtonyekDocument::Confidence m_confidence;
  EtonyekDocument::Type m_type;
  Format m_format;
};

DetectionInfo::DetectionInfo(const EtonyekDocument::Type type)
  : m_input()
  , m_package()
  , m_fragments()
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

namespace
{
void handleError(void * /*arg*/, const char * /*msg*/, xmlParserSeverities /*severity*/, xmlTextReaderLocatorPtr /*locator*/)
{
}
}

bool probeXML(DetectionInfo &info)
{
  const shared_ptr<xmlTextReader> reader(xmlReaderForIO(readFromStream, closeStream, info.m_input.get(), "", nullptr, 0), xmlFreeTextReader);
  if (!reader)
    return false;

  xmlTextReaderSetErrorHandler(reader.get(), handleError, nullptr);

  int ret = 0;
  bool checkAPXL=false;
  do
  {
    ret = xmlTextReaderRead(reader.get());
    if (ret==1 && XML_READER_TYPE_DOCUMENT_TYPE==xmlTextReaderNodeType(reader.get()))
    {
      auto name = xmlTextReaderConstName(reader.get());
      if (name) checkAPXL=std::string((char const *)name)=="APXL";
    }
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
  // Keynote 1 files define the document type with <!DOCTYPE APXL>
  if (probeXMLFormat(FORMAT_XML1, EtonyekDocument::TYPE_KEYNOTE, KEY1Token::NS_URI_KEY | KEY1Token::presentation,
                     KEY1Token::getTokenizer(), name, (ns||!checkAPXL) ? ns : "http://developer.apple.com/schemas/APXL", info))
    return true;
  return false;
}

bool probeBinary(DetectionInfo &info)
{
  const uint64_t headerLen = readUVar(info.m_input);
  if (headerLen < 8)
    return false;

  EtonyekDocument::Type detected = EtonyekDocument::TYPE_UNKNOWN;

  const auto pos = uint64_t(info.m_input->tell());
  const IWAMessage header(info.m_input, (unsigned long) headerLen);

  if (header.uint32(1) && header.message(2) && header.message(2).uint32(1) && (header.uint32(1).get() == 1))
  {
    switch (header.message(2).uint32(1).get())
    {
    case 1 :
      if (header.message(2).uint32(3))
      {
        const auto dataLen = get(header.message(2).uint32(3));
        const IWAMessage data(info.m_input, long(pos + headerLen), long(pos + headerLen + dataLen));
        // keynote: presentation ref in 2
        // number: sheet ref in 1
        if (!data.message(1))
          detected = EtonyekDocument::TYPE_KEYNOTE;
        else if (!data.message(2))
          detected = EtonyekDocument::TYPE_NUMBERS;
        else
        {
          unsigned potentialRef[2];
          for (unsigned test=1; test<=2; ++test)
          {
            auto ref=data.message(test).uint32(1).optional();
            if (!ref)
            {
              detected = test==1 ? EtonyekDocument::TYPE_KEYNOTE : EtonyekDocument::TYPE_NUMBERS;
              break;
            }
            potentialRef[test-1]=get(ref);
          }
          if (detected != EtonyekDocument::TYPE_UNKNOWN)
            break;
          // undecise, try to find the first ref
          IWAObjectIndex objIndex(info.m_fragments, info.m_package);
          objIndex.parse();
          auto type = objIndex.getObjectType(potentialRef[0]);
          detected = type && get(type)==2 ?
                     EtonyekDocument::TYPE_NUMBERS : EtonyekDocument::TYPE_KEYNOTE;
        }
      }
      break;
    case 10000 :
      detected = EtonyekDocument::TYPE_PAGES;
      break;
    default:
      break;
    }
  }

  if ((info.m_type == EtonyekDocument::TYPE_UNKNOWN) || (info.m_type == detected))
  {
    info.m_type = detected;
    return true;
  }
  return false;
}

RVNGInputStreamPtr_t getSubStream(const RVNGInputStreamPtr_t &input, const char *const name)
{
  return RVNGInputStreamPtr_t(input->getSubStreamByName(name));
}

RVNGInputStreamPtr_t getUncompressedSubStream(const RVNGInputStreamPtr_t &input, const char *const name, bool snappy = false) try
{
  const RVNGInputStreamPtr_t compressed(input->getSubStreamByName(name));
  if (bool(compressed))
  {
    if (snappy)
      return RVNGInputStreamPtr_t(new IWASnappyStream(compressed));
    return RVNGInputStreamPtr_t(new IWORKZlibStream(compressed));
  }
  return RVNGInputStreamPtr_t();
}
catch (...)
{
  return RVNGInputStreamPtr_t();
}

bool detectBinary(RVNGInputStreamPtr_t input, DetectionInfo &info)
{
  assert(input->isStructured());

  if (input->existsSubStream("Metadata/DocumentIdentifier"))
    info.m_package = input;

  if (input->existsSubStream("Index.zip"))
  {
    RVNGInputStreamPtr_t zipInput = getSubStream(input, "Index.zip");
    if (bool(zipInput))
      input = zipInput;
  }

  const bool hasDocument = input->existsSubStream("Index/Document.iwa");

  if (hasDocument)
  {
    info.m_format = FORMAT_BINARY;
    info.m_fragments = input;
    info.m_input = getUncompressedSubStream(input, "Index/Document.iwa", true);
  }

  return hasDocument;
}

RVNGInputStreamPtr_t queryTopDirStream(const RVNGInputStreamPtr_t &input)
{
  assert(input->isStructured());

  string top;

  // find the common top level dir of all substream names, if there is one
  for (unsigned i = 0; i < input->subStreamCount(); ++i)
  {
    const char *const path = input->subStreamName(i);
    if (path)
    {
      if (top.empty())
      {
        // initialize top dir
        const char *const pos = std::strchr(path, '/');
        if (pos)
          top.assign(path, pos - path);
        else
          top = path;
      }
      else
      {
        // check that the current path starts with top dir
        if (!boost::starts_with(path, top))
          return RVNGInputStreamPtr_t();
        const char end = path[top.size()];
        if (end != '/' && end != '\0')
          return RVNGInputStreamPtr_t();
      }
    }
  }

  RVNGInputStreamPtr_t stream;
  if (!top.empty())
    stream.reset(new IWORKSubDirStream(input, top));

  return stream;
}

bool detect(const RVNGInputStreamPtr_t &input, DetectionInfo &info)
{
  if (input->isStructured())
  {
    if ((info.m_format == FORMAT_BINARY) || (info.m_format == FORMAT_UNKNOWN))
    {
      if (!detectBinary(input, info))
      {
        RVNGInputStreamPtr_t dir = queryTopDirStream(input);
        if (dir)
          detectBinary(dir, info);
      }
    }

    if ((info.m_format == FORMAT_XML2) || (info.m_format == FORMAT_UNKNOWN))
    {
      info.m_package = input;

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

    if (info.m_format == FORMAT_XML1 || info.m_format == FORMAT_UNKNOWN)
    {
      info.m_package = input;

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
  }
  else
  {
    try
    {
      info.m_input = std::make_shared<IWORKZlibStream>(input);
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

    bool supported = false;
    if (info.m_format == FORMAT_BINARY)
      supported = probeBinary(info);
    else
      supported = probeXML(info);
    if (supported)
      info.m_confidence = bool(info.m_package) ? EtonyekDocument::CONFIDENCE_EXCELLENT : EtonyekDocument::CONFIDENCE_SUPPORTED_PART;
  }

  if (info.m_confidence != EtonyekDocument::CONFIDENCE_NONE)
  {
    assert(EtonyekDocument::TYPE_UNKNOWN != info.m_type);
    assert(FORMAT_UNKNOWN != info.m_format);
    assert(bool(info.m_input));
    if (info.m_confidence == EtonyekDocument::CONFIDENCE_EXCELLENT)
    {
      assert(bool(info.m_package));
    }
  }

  return info.m_confidence != EtonyekDocument::CONFIDENCE_NONE;
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

  info.m_input->seek(0, librevenge::RVNG_SEEK_SET);

  IWORKPresentationRedirector redirector(generator);
  KEYCollector collector(&redirector);
  if (info.m_format == FORMAT_XML1)
  {
    KEY1Dictionary dict;
    input->seek(0, librevenge::RVNG_SEEK_SET);
    shared_ptr<KEY1Parser> key1Parser(new KEY1Parser(info.m_input, info.m_package, collector, dict));
    return key1Parser->parse();
  }
  else if (info.m_format == FORMAT_XML2)
  {
    KEY2Dictionary dict;
    shared_ptr<KEY2Parser> key2Parser(new KEY2Parser(info.m_input, info.m_package, collector, dict));
    return key2Parser->parse();
  }
  else if (info.m_format == FORMAT_BINARY)
  {
    KEY6Parser parser(info.m_fragments, info.m_package, collector);
    return parser.parse();
  }

  ETONYEK_DEBUG_MSG(("EtonyekDocument::parse: unhandled format %d\n", info.m_format));
  return false;
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

  info.m_input->seek(0, librevenge::RVNG_SEEK_SET);

  IWORKSpreadsheetRedirector redirector(document);
  NUMCollector collector(&redirector);
  if (info.m_format == FORMAT_XML2)
  {
    NUM1Dictionary dict;
    NUM1Parser parser(info.m_input, info.m_package, collector, &dict);
    return parser.parse();
  }
  else if (info.m_format == FORMAT_BINARY)
  {
    NUM3Parser parser(info.m_fragments, info.m_package, collector);
    return parser.parse();
  }

  ETONYEK_DEBUG_MSG(("EtonyekDocument::parse: unhandled format %d\n", info.m_format));
  return false;
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

  info.m_input->seek(0, librevenge::RVNG_SEEK_SET);

  IWORKTextRedirector redirector(document);
  PAGCollector collector(&redirector);
  if (info.m_format == FORMAT_XML2)
  {
    PAG1Dictionary dict;
    PAG1Parser parser(info.m_input, info.m_package, collector, &dict);
    return parser.parse();
  }
  else if (info.m_format == FORMAT_BINARY)
  {
    PAG5Parser parser(info.m_fragments, info.m_package, collector);
    return parser.parse();
  }

  ETONYEK_DEBUG_MSG(("EtonyekDocument::parse: unhandled format %d\n", info.m_format));
  return false;
}
catch (...)
{
  return false;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
