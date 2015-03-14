/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "IWORKXMLParserState.h"

#include "IWORKDictionary.h"
#include "IWORKParser.h"
#include "KEYCollector.h"

namespace libetonyek
{

IWORKXMLParserState::IWORKXMLParserState(IWORKParser &parser, IWORKDictionary &dict, const IWORKXMLReader::TokenizerFunction_t &tokenizer)
  : m_parser(parser)
  , m_dict(dict)
  , m_tokenizer(tokenizer)
{
}

IWORKParser &IWORKXMLParserState::getParser()
{
  return m_parser;
}

IWORKDictionary &IWORKXMLParserState::getDictionary()
{
  return m_dict;
}

KEYCollector *IWORKXMLParserState::getCollector() const
{
  return m_parser.getCollector();
}

const IWORKXMLReader::TokenizerFunction_t &IWORKXMLParserState::getTokenizer() const
{
  return m_tokenizer;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
