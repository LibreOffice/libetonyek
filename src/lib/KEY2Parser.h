/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY2PARSER_H_INCLUDED
#define KEY2PARSER_H_INCLUDED

#include "IWORKParser.h"
#include "KEY2ParserState.h"

namespace libetonyek
{

class KEYCollector;
struct KEY2Dictionary;

class KEY2Parser : public IWORKParser
{
public:
  KEY2Parser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, KEYCollector &collector, KEY2Dictionary &dict);
  ~KEY2Parser() override;

private:
  IWORKXMLContextPtr_t createDocumentContext() override;
  IWORKXMLContextPtr_t createDiscardContext() override;
  const IWORKTokenizer &getTokenizer() const override;

private:
  KEY2ParserState m_state;
};

}

#endif //  KEY2PARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
