/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "KEY2ParserState.h"
#include "KEYDictionary.h"

namespace libetonyek
{

KEY2ParserState::KEY2ParserState(IWORKParser &parser, KEYDictionary &dict, const TokenizerFunction_t &tokenizer)
  : IWORKXMLParserState(parser, dict, tokenizer)
  , m_dict(dict)
{
}

KEYDictionary &KEY2ParserState::getDictionary()
{
  return m_dict;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
