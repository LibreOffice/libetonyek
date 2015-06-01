/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PAG1PARSERSTATE_H_INCLUDED
#define PAG1PARSERSTATE_H_INCLUDED

#include "IWORKXMLParserState.h"

namespace libetonyek
{

class PAG1Parser;
class PAGCollector;
struct PAGDictionary;

class PAG1ParserState : public IWORKXMLParserState
{
  // not copyable
  PAG1ParserState(const PAG1ParserState &);
  PAG1ParserState &operator=(const PAG1ParserState &);

public:
  PAG1ParserState(PAG1Parser &parser, PAGCollector &collector, PAGDictionary &dict);

  PAGCollector &getCollector();
  PAGDictionary &getDictionary();

private:
  PAGCollector &m_collector;
  PAGDictionary &m_dict;
};

}

#endif // PAG1PARSERSTATE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
