/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "NUMParser.h"

#include <cassert>

#include <librevenge-stream/librevenge-stream.h>

#include "libetonyek_xml.h"
#include "IWORKToken.h"
#include "IWORKXMLReader.h"
#include "NUMCollector.h"
#include "NUMToken.h"

namespace libetonyek
{

NUMParser::NUMParser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, NUMCollector *const collector)
  : m_input(input)
  , m_package(package)
  , m_collector(collector)
{
}

bool NUMParser::parse() try
{
  const IWORKXMLReader reader(m_input.get(), IWORKXMLReader::ChainedTokenizer(NUMTokenizer(), IWORKTokenizer()));
  parseDocument(reader);
  return true;
}
catch (...)
{
  return false;
}

void NUMParser::parseDocument(const IWORKXMLReader &reader)
{
  // assert((NUMToken::NS_URI_LS | NUMToken::document) == getId(reader));

  m_collector->startDocument();

  IWORKXMLReader::ElementIterator element(reader);
  while (element.next())
  {
    switch (getId(element))
    {
    default :
      skipElement(element);
      break;
    }
  }

  m_collector->endDocument();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
