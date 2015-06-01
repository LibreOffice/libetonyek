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
#include "KEYCollector.h"

namespace libetonyek
{

KEY1Parser::KEY1Parser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, KEYCollector &/*collector*/)
  : IWORKParser(input, package)
{
}

KEY1Parser::~KEY1Parser()
{
}

IWORKXMLContextPtr_t KEY1Parser::createDocumentContext()
{
  // TODO: implement me
  return IWORKXMLContextPtr_t();
}

IWORKXMLContextPtr_t KEY1Parser::createDiscardContext()
{
  // TODO: implement me
  return IWORKXMLContextPtr_t();
}

const IWORKTokenizer &KEY1Parser::getTokenizer() const
{
  return KEY1Token::getTokenizer();
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
