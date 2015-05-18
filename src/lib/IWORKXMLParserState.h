/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef IWORKXMLPARSERSTATE_H_INCLUDED
#define IWORKXMLPARSERSTATE_H_INCLUDED

namespace libetonyek
{

class IWORKCollector;
struct IWORKDictionary;
class IWORKParser;
class IWORKTokenizer;

class IWORKXMLParserState
{
  // not copyable
  IWORKXMLParserState(const IWORKXMLParserState &);
  IWORKXMLParserState &operator=(const IWORKXMLParserState &);

public:
  IWORKXMLParserState(IWORKParser &parser, IWORKCollector *collector, IWORKDictionary &dict);

  IWORKParser &getParser();
  IWORKDictionary &getDictionary();
  IWORKCollector *getCollector() const;
  const IWORKTokenizer &getTokenizer() const;

private:
  IWORKParser &m_parser;
  IWORKCollector *const m_collector;
  IWORKDictionary &m_dict;
};

}

#endif // IWORKXMLPARSERSTATE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
