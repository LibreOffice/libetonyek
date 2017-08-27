/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PAG1PARSER_H_INCLUDED
#define PAG1PARSER_H_INCLUDED

#include "IWORKParser.h"
#include "PAG1ParserState.h"

namespace libetonyek
{

class PAGCollector;
struct PAG1Dictionary;

class PAG1Parser: public IWORKParser
{
public:
  PAG1Parser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, PAGCollector &collector, PAG1Dictionary *dict);
  ~PAG1Parser() override;

private:
  IWORKXMLContextPtr_t createDocumentContext() override;
  IWORKXMLContextPtr_t createDiscardContext() override;
  const IWORKTokenizer &getTokenizer() const override;

private:
  PAG1ParserState m_state;
};

} // namespace libetonyek

#endif //  PAG1PARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
