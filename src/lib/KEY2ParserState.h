/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef KEY2PARSERSTATE_H_INCLUDED
#define KEY2PARSERSTATE_H_INCLUDED

#include "IWORKXMLParserState.h"

namespace libetonyek
{

class KEY2Parser;
class KEYCollector;
struct KEYDictionary;

class KEY2ParserState : public IWORKXMLParserState
{
  // not copyable
  KEY2ParserState(const KEY2ParserState &);
  KEY2ParserState &operator=(const KEY2ParserState &);

public:
  KEY2ParserState(KEY2Parser &parser, KEYCollector *collector, KEYDictionary &dict);

  KEYDictionary &getDictionary();
  KEYCollector *getCollector();

private:
  KEYDictionary &m_dict;
  KEYCollector *const m_collector;
};

}

#endif // KEY2PARSERSTATE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
