/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "NUM1Parser.h"

#include <cassert>

#include <librevenge-stream/librevenge-stream.h>

#include "libetonyek_xml.h"
#include "IWORKToken.h"
#include "NUMCollector.h"
#include "NUM1Token.h"

namespace libetonyek
{

NUM1Parser::NUM1Parser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, NUMCollector *const collector)
  : m_input(input)
  , m_package(package)
  , m_collector(collector)
{
}

bool NUM1Parser::parse() try
{
  // Todo
  // const IWORKXMLReader reader(m_input.get(), IWORKXMLReader::ChainedTokenizer(NUM1Tokenizer(), IWORKTokenizer()));
  // parseDocument(reader);
  return true;
}
catch (...)
{
  return false;
}

// void NUM1Parser::parseDocument(const IWORKXMLReader &reader)
// {

// Todo
// assert((NUM1Token::NS_URI_LS | NUM1Token::document) == getId(reader));

// m_collector->startDocument();

// IWORKXMLReader::ElementIterator element(reader);
// while (element.next())
// {
//   switch (getId(element))
//   {
//   default :
//     skipElement(element);
//     break;
//   }
// }

// m_collector->endDocument();
// }

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
