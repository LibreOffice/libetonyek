/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cassert>

#include <librevenge-stream/librevenge-stream.h>

#include "libetonyek_xml.h"
#include "KEYXMLReader.h"
#include "PAGESParser.h"
#include "PAGESCollector.h"
#include "PAGESToken.h"

namespace libetonyek
{

PAGESParser::PAGESParser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, PAGESCollector *const collector)
  : m_input(input)
  , m_package(package)
  , m_collector(collector)
{
}

bool PAGESParser::parse() try
{
  const KEYXMLReader reader(m_input.get(), PAGESTokenizer());
  parseDocument(reader);
  return true;
}
catch (...)
{
  return false;
}

void PAGESParser::parseDocument(const KEYXMLReader &reader)
{
  assert((PAGESToken::NS_URI_SL | PAGESToken::document) == getId(reader));

  m_collector->startDocument();

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (getId(element))
    {
    case PAGESToken::NS_URI_SF | PAGESToken::metadata :
      parseMetadata(element);
      break;
    case PAGESToken::NS_URI_SL | PAGESToken::section_prototypes :
      parseSectionPrototypes(element);
      break;
    case PAGESToken::NS_URI_SL | PAGESToken::stylesheet :
      parseStylesheet(reader);
      break;
    case PAGESToken::NS_URI_SF | PAGESToken::headers :
      parseHeaders(element);
      break;
    case PAGESToken::NS_URI_SF | PAGESToken::footers :
      parseFooters(element);
      break;
    case PAGESToken::NS_URI_SF | PAGESToken::text_storage :
      parseTextStorage(element);
      break;
    default :
      skipElement(element);
      break;
    }
  }

  m_collector->endDocument();
}

void PAGESParser::parseMetadata(const KEYXMLReader &reader)
{
  assert((PAGESToken::NS_URI_SF | PAGESToken::metadata) == getId(reader));
  // TODO: implement me
  skipElement(reader);
}

void PAGESParser::parseSectionPrototypes(const KEYXMLReader &reader)
{
  assert((PAGESToken::NS_URI_SL | PAGESToken::section_prototypes) == getId(reader));
  // TODO: implement me
  skipElement(reader);
}

void PAGESParser::parseStylesheet(const KEYXMLReader &reader)
{
  assert((PAGESToken::NS_URI_SL | PAGESToken::stylesheet) == getId(reader));
  // TODO: implement me
  skipElement(reader);
}

void PAGESParser::parseHeaders(const KEYXMLReader &reader)
{
  assert((PAGESToken::NS_URI_SF | PAGESToken::headers) == getId(reader));
  // TODO: implement me
  skipElement(reader);
}

void PAGESParser::parseFooters(const KEYXMLReader &reader)
{
  assert((PAGESToken::NS_URI_SF | PAGESToken::footers) == getId(reader));
  // TODO: implement me
  skipElement(reader);
}

void PAGESParser::parseTextStorage(const KEYXMLReader &reader)
{
  assert((PAGESToken::NS_URI_SF | PAGESToken::text_storage) == getId(reader));

  TextStorageKind kind = TEXT_STORAGE_KIND_UNKNOWN;

  KEYXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((PAGESToken::NS_URI_SF | PAGESToken::kind) == getId(attr))
    {
      switch (getValueId(attr))
      {
      case PAGESToken::body :
        kind = TEXT_STORAGE_KIND_BODY;
        break;
      case PAGESToken::cell :
        kind = TEXT_STORAGE_KIND_CELL;
        break;
      case PAGESToken::footnote :
        kind = TEXT_STORAGE_KIND_FOOTNOTE;
        break;
      case PAGESToken::header :
        kind = TEXT_STORAGE_KIND_HEADER;
        break;
      case PAGESToken::note :
        kind = TEXT_STORAGE_KIND_NOTE;
        break;
      case PAGESToken::textbox :
        kind = TEXT_STORAGE_KIND_TEXTBOX;
        break;
      default :
        assert(!"unknown storage value");
        break;
      }
    }
  }

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (getId(element))
    {
    case PAGESToken::NS_URI_SF | PAGESToken::text_body :
      parseTextBody(reader, kind);
      break;
    default :
      skipElement(reader);
      break;
    }
  }
}

void PAGESParser::parseTextBody(const KEYXMLReader &reader, const TextStorageKind kind)
{
  assert((PAGESToken::NS_URI_SF | PAGESToken::text_body) == getId(reader));

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (kind)
    {
    case TEXT_STORAGE_KIND_BODY :
      if ((PAGESToken::NS_URI_SF | PAGESToken::section) == getId(element))
        parseSection(element);
      else
        skipElement(reader);
      break;
    default :
      skipElement(reader);
      break;
    }
  }
}

void PAGESParser::parseSection(const KEYXMLReader &reader)
{
  assert((PAGESToken::NS_URI_SF | PAGESToken::section) == getId(reader));

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if ((PAGESToken::NS_URI_SF | PAGESToken::layout) == getId(element))
      parseLayout(element);
    else
      skipElement(reader);
  }
}

void PAGESParser::parseLayout(const KEYXMLReader &reader)
{
  assert((PAGESToken::NS_URI_SF | PAGESToken::layout) == getId(reader));

  KEYXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if ((PAGESToken::NS_URI_SF | PAGESToken::p) == getId(element))
      parseP(element);
    else
      skipElement(reader);
  }
}

void PAGESParser::parseP(const KEYXMLReader &reader)
{
  assert((PAGESToken::NS_URI_SF | PAGESToken::p) == getId(reader));

  m_collector->startParagraph();

  KEYXMLReader::MixedIterator mixed(reader);
  while (mixed.next())
  {
    if (mixed.isElement())
    {
      switch (getId(mixed))
      {
      case PAGESToken::NS_URI_SF | PAGESToken::br :
      case PAGESToken::NS_URI_SF | PAGESToken::lnbr :
        parseBr(mixed);
        break;
      case PAGESToken::NS_URI_SF | PAGESToken::span :
        parseSpan(mixed);
        break;
      case PAGESToken::NS_URI_SF | PAGESToken::tab :
        parseTab(mixed);
        break;
      default :
        skipElement(reader);
      }
    }
    else
    {
      m_collector->collectText(mixed.getText());
    }
  }

  m_collector->endParagraph();
}

void PAGESParser::parseSpan(const KEYXMLReader &reader)
{
  assert((PAGESToken::NS_URI_SF | PAGESToken::span) == getId(reader));

  m_collector->startSpan();

  KEYXMLReader::MixedIterator mixed(reader);
  while (mixed.next())
  {
    if (mixed.isElement())
    {
      switch (getId(mixed))
      {
      case PAGESToken::NS_URI_SF | PAGESToken::br :
      case PAGESToken::NS_URI_SF | PAGESToken::lnbr :
        parseBr(mixed);
        break;
      case PAGESToken::NS_URI_SF | PAGESToken::tab :
        parseTab(mixed);
        break;
      default :
        skipElement(reader);
      }
    }
    else
    {
      m_collector->collectText(mixed.getText());
    }
  }

  m_collector->endSpan();
}

void PAGESParser::parseTab(const KEYXMLReader &reader)
{
  assert((PAGESToken::NS_URI_SF | PAGESToken::tab) == getId(reader));

  checkEmptyElement(reader);

  m_collector->collectTab();
}

void PAGESParser::parseBr(const KEYXMLReader &reader)
{
  assert(((PAGESToken::NS_URI_SF | PAGESToken::br) == getId(reader))
         || ((PAGESToken::NS_URI_SF | PAGESToken::lnbr) == getId(reader)));

  checkEmptyElement(reader);

  m_collector->collectLineBreak();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
