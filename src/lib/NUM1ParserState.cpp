/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "NUM1ParserState.h"

#include "NUM1Parser.h"
#include "NUMCollector.h"
#include "NUMDictionary.h"

namespace libetonyek
{

NUM1ParserState::NUM1ParserState(NUM1Parser &parser, NUMCollector *const collector, NUMDictionary &dict, const TokenizerFunction_t &tokenizer)
  : IWORKXMLParserState(parser, collector, dict, tokenizer)
  , m_collector(collector)
  , m_dict(dict)
{
}

NUMCollector *NUM1ParserState::getCollector()
{
  return m_collector;
}

NUMDictionary &NUM1ParserState::getDictionary()
{
  return m_dict;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
