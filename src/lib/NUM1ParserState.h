/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef NUM1PARSERSTATE_H_INCLUDED
#define NUM1PARSERSTATE_H_INCLUDED

#include "IWORKXMLParserState.h"

namespace libetonyek
{

class NUM1Parser;
class NUMCollector;
struct NUM1Dictionary;

class NUM1ParserState : public IWORKXMLParserState
{
  // not copyable
  NUM1ParserState(const NUM1ParserState &);
  NUM1ParserState &operator=(const NUM1ParserState &);

public:
  NUM1ParserState(NUM1Parser &parser, NUMCollector &collector, NUM1Dictionary &dict);

  NUMCollector &getCollector();
  NUM1Dictionary &getDictionary();

private:
  NUMCollector &m_collector;
  NUM1Dictionary &m_dict;
};

}

#endif // NUM1PARSERSTATE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
