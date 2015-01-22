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
#include "IWORKXMLReader.h"
#include "PAGParser.h"
#include "PAGCollector.h"
#include "PAGToken.h"

namespace libetonyek
{

PAGParser::PAGParser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, PAGCollector *const collector)
  : m_input(input)
  , m_package(package)
  , m_collector(collector)
{
}

bool PAGParser::parse() try
{
  const IWORKXMLReader reader(m_input.get(), PAGTokenizer());
  parseDocument(reader);
  return true;
}
catch (...)
{
  return false;
}

void PAGParser::parseDocument(const IWORKXMLReader &reader)
{
  assert((PAGToken::NS_URI_SL | PAGToken::document) == getId(reader));

  m_collector->startDocument();

  IWORKXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (getId(element))
    {
    case PAGToken::NS_URI_SF | PAGToken::metadata :
      parseMetadata(element);
      break;
    case PAGToken::NS_URI_SL | PAGToken::section_prototypes :
      parseSectionPrototypes(element);
      break;
    case PAGToken::NS_URI_SL | PAGToken::stylesheet :
      parseStylesheet(reader);
      break;
    case PAGToken::NS_URI_SF | PAGToken::headers :
      parseHeaders(element);
      break;
    case PAGToken::NS_URI_SF | PAGToken::footers :
      parseFooters(element);
      break;
    case PAGToken::NS_URI_SF | PAGToken::text_storage :
      parseTextStorage(element);
      break;
    default :
      skipElement(element);
      break;
    }
  }

  m_collector->endDocument();
}

void PAGParser::parseMetadata(const IWORKXMLReader &reader)
{
  assert((PAGToken::NS_URI_SF | PAGToken::metadata) == getId(reader));
  // TODO: implement me
  skipElement(reader);
}

void PAGParser::parseSectionPrototypes(const IWORKXMLReader &reader)
{
  assert((PAGToken::NS_URI_SL | PAGToken::section_prototypes) == getId(reader));
  // TODO: implement me
  skipElement(reader);
}

void PAGParser::parseStylesheet(const IWORKXMLReader &reader)
{
  assert((PAGToken::NS_URI_SL | PAGToken::stylesheet) == getId(reader));
  // TODO: implement me
  skipElement(reader);
}

void PAGParser::parseHeaders(const IWORKXMLReader &reader)
{
  assert((PAGToken::NS_URI_SF | PAGToken::headers) == getId(reader));
  // TODO: implement me
  skipElement(reader);
}

void PAGParser::parseFooters(const IWORKXMLReader &reader)
{
  assert((PAGToken::NS_URI_SF | PAGToken::footers) == getId(reader));
  // TODO: implement me
  skipElement(reader);
}

void PAGParser::parseTextStorage(const IWORKXMLReader &reader)
{
  assert((PAGToken::NS_URI_SF | PAGToken::text_storage) == getId(reader));

  TextStorageKind kind = TEXT_STORAGE_KIND_UNKNOWN;

  IWORKXMLReader::AttributeIterator attr(reader);
  while (attr.next())
  {
    if ((PAGToken::NS_URI_SF | PAGToken::kind) == getId(attr))
    {
      switch (getValueId(attr))
      {
      case PAGToken::body :
        kind = TEXT_STORAGE_KIND_BODY;
        break;
      case PAGToken::cell :
        kind = TEXT_STORAGE_KIND_CELL;
        break;
      case PAGToken::footnote :
        kind = TEXT_STORAGE_KIND_FOOTNOTE;
        break;
      case PAGToken::header :
        kind = TEXT_STORAGE_KIND_HEADER;
        break;
      case PAGToken::note :
        kind = TEXT_STORAGE_KIND_NOTE;
        break;
      case PAGToken::textbox :
        kind = TEXT_STORAGE_KIND_TEXTBOX;
        break;
      default :
        assert(!"unknown storage value");
        break;
      }
    }
  }

  IWORKXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (getId(element))
    {
    case PAGToken::NS_URI_SF | PAGToken::text_body :
      parseTextBody(reader, kind);
      break;
    default :
      skipElement(reader);
      break;
    }
  }
}

void PAGParser::parseTextBody(const IWORKXMLReader &reader, const TextStorageKind kind)
{
  assert((PAGToken::NS_URI_SF | PAGToken::text_body) == getId(reader));

  IWORKXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (kind)
    {
    case TEXT_STORAGE_KIND_BODY :
      if ((PAGToken::NS_URI_SF | PAGToken::section) == getId(element))
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

void PAGParser::parseSection(const IWORKXMLReader &reader)
{
  assert((PAGToken::NS_URI_SF | PAGToken::section) == getId(reader));

  IWORKXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if ((PAGToken::NS_URI_SF | PAGToken::layout) == getId(element))
      parseLayout(element);
    else
      skipElement(reader);
  }
}

void PAGParser::parseLayout(const IWORKXMLReader &reader)
{
  assert((PAGToken::NS_URI_SF | PAGToken::layout) == getId(reader));

  IWORKXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    if ((PAGToken::NS_URI_SF | PAGToken::p) == getId(element))
      parseP(element);
    else
      skipElement(reader);
  }
}

void PAGParser::parseP(const IWORKXMLReader &reader)
{
  assert((PAGToken::NS_URI_SF | PAGToken::p) == getId(reader));

  m_collector->startParagraph();

  IWORKXMLReader::MixedIterator mixed(reader);
  while (mixed.next())
  {
    if (mixed.isElement())
    {
      switch (getId(mixed))
      {
      case PAGToken::NS_URI_SF | PAGToken::br :
      case PAGToken::NS_URI_SF | PAGToken::lnbr :
        parseBr(mixed);
        break;
      case PAGToken::NS_URI_SF | PAGToken::span :
        parseSpan(mixed);
        break;
      case PAGToken::NS_URI_SF | PAGToken::tab :
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

void PAGParser::parseSpan(const IWORKXMLReader &reader)
{
  assert((PAGToken::NS_URI_SF | PAGToken::span) == getId(reader));

  m_collector->startSpan();

  IWORKXMLReader::MixedIterator mixed(reader);
  while (mixed.next())
  {
    if (mixed.isElement())
    {
      switch (getId(mixed))
      {
      case PAGToken::NS_URI_SF | PAGToken::br :
      case PAGToken::NS_URI_SF | PAGToken::lnbr :
        parseBr(mixed);
        break;
      case PAGToken::NS_URI_SF | PAGToken::tab :
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

void PAGParser::parseTab(const IWORKXMLReader &reader)
{
  assert((PAGToken::NS_URI_SF | PAGToken::tab) == getId(reader));

  checkEmptyElement(reader);

  m_collector->collectTab();
}

void PAGParser::parseBr(const IWORKXMLReader &reader)
{
  assert(((PAGToken::NS_URI_SF | PAGToken::br) == getId(reader))
         || ((PAGToken::NS_URI_SF | PAGToken::lnbr) == getId(reader)));

  checkEmptyElement(reader);

  m_collector->collectLineBreak();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
