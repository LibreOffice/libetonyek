/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef NUM1PARSER_H_INCLUDED
#define NUM1PARSER_H_INCLUDED

#include "IWORKParser.h"
#include "NUM1ParserState.h"

namespace libetonyek
{

class NUMCollector;
struct NUM1Dictionary;

class NUM1Parser: public IWORKParser
{
public:
  NUM1Parser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, NUMCollector &collector, NUM1Dictionary *dict);
  virtual ~NUM1Parser();

private:
  virtual IWORKXMLContextPtr_t createDocumentContext();
  virtual IWORKXMLContextPtr_t createDiscardContext();
  virtual const IWORKTokenizer &getTokenizer() const;

private:
  NUM1ParserState m_state;
};

} // namespace libetonyek

#endif //  NUM1PARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
