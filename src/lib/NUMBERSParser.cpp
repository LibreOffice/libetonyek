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
#include "NUMBERSParser.h"
#include "NUMBERSCollector.h"
#include "NUMBERSToken.h"

namespace libetonyek
{

NUMBERSParser::NUMBERSParser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, NUMBERSCollector *const collector)
  : m_input(input)
  , m_package(package)
  , m_collector(collector)
{
}

bool NUMBERSParser::parse() try
{
  const KEYXMLReader reader(m_input.get(), NUMBERSTokenizer());
  parseDocument(reader);
  return true;
}
catch (...)
{
  return false;
}

void NUMBERSParser::parseDocument(const KEYXMLReader &reader)
{
  // assert((NUMBERSToken::NS_URI_LS | NUMBERSToken::document) == getId(reader));

  m_collector->startDocument();

  KEYXMLReader::ElementIterator element(reader);
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
