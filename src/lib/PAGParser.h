/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PAGPARSER_H_INCLUDED
#define PAGPARSER_H_INCLUDED

#include "IWORKParser.h"
#include "PAGParserState.h"

namespace libetonyek
{

class PAGCollector;
class PAGDictionary;

class PAGParser: public IWORKParser
{
public:
  PAGParser(const RVNGInputStreamPtr_t &input, const RVNGInputStreamPtr_t &package, PAGCollector *collector, PAGDictionary *dict);
  virtual ~PAGParser();

private:
  virtual IWORKXMLContextPtr_t createDocumentContext();
  virtual IWORKXMLReader::TokenizerFunction_t getTokenizer() const;

private:
  PAGParserState m_state;
  unsigned m_version;
};

} // namespace libetonyek

#endif //  PAGPARSER_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
