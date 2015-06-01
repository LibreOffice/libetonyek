/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "PAG1ParserState.h"

#include "PAG1Parser.h"
#include "PAGCollector.h"
#include "PAGDictionary.h"

namespace libetonyek
{

PAG1ParserState::PAG1ParserState(PAG1Parser &parser, PAGCollector &collector, PAGDictionary &dict)
  : IWORKXMLParserState(parser, collector, dict)
  , m_collector(collector)
  , m_dict(dict)
{
}

PAGCollector &PAG1ParserState::getCollector()
{
  return m_collector;
}

PAGDictionary &PAG1ParserState::getDictionary()
{
  return m_dict;
}

}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
