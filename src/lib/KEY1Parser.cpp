/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEY1Parser.h"
#include "KEY1Token.h"
#include "KEYXMLReader.h"

namespace libetonyek
{

KEY1Parser::KEY1Parser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &, KEYCollector *const collector, const KEYDefaults &defaults)
  : KEYParser(input, collector, defaults)
{
}

KEY1Parser::~KEY1Parser()
{
}

void KEY1Parser::processXmlNode(const KEYXMLReader &reader)
{
  // TODO: implement me
  (void) reader;
}

KEYXMLReader::TokenizerFunction_t KEY1Parser::getTokenizer() const
{
  return KEY1Tokenizer();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
