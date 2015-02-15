/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY1PARSER_H_INCLUDED
#define KEY1PARSER_H_INCLUDED

#include "KEYParser.h"

namespace libetonyek
{

class KEY1Parser : public KEYParser
{
public:
  KEY1Parser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, KEYCollector *collector, const KEYDefaults &defaults);
  virtual ~KEY1Parser();

private:
  virtual IWORKXMLContextPtr_t makeDocumentContext(IWORKXMLParserState &state) const;
  virtual IWORKXMLReader::TokenizerFunction_t getTokenizer() const;
};

}

#endif //  KEY1PARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
