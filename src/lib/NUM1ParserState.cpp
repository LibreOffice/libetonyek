/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "NUM1ParserState.h"

#include "NUM1Dictionary.h"
#include "NUM1Parser.h"
#include "NUMCollector.h"

namespace libetonyek
{

NUM1ParserState::NUM1ParserState(NUM1Parser &parser, NUMCollector &collector, NUM1Dictionary &dict)
  : IWORKXMLParserState(parser, collector, dict)
  , m_collector(collector)
  , m_dict(dict)
{
}

NUMCollector &NUM1ParserState::getCollector()
{
  return m_collector;
}

NUM1Dictionary &NUM1ParserState::getDictionary()
{
  return m_dict;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
