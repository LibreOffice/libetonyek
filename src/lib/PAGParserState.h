/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libetonyek project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef PAGPARSERSTATE_H_INCLUDED
#define PAGPARSERSTATE_H_INCLUDED

#include "IWORKXMLParserState.h"

namespace libetonyek
{

struct PAGDictionary;

class PAGParserState : public IWORKXMLParserState
{
  // not copyable
  PAGParserState(const PAGParserState &);
  PAGParserState &operator=(const PAGParserState &);

public:
  PAGParserState(IWORKParser &parser, PAGDictionary &dict, const IWORKXMLReader::TokenizerFunction_t &tokenizer);

  PAGDictionary &getDictionary();

private:
  PAGDictionary &m_dict;
};

}

#endif // PAGPARSERSTATE_H_INCLUDED

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
